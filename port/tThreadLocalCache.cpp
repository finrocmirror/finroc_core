/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
 *   Robotics Research Lab, University of Kaiserslautern
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "rrlib/rtti/rtti.h"
#include "rrlib/design_patterns/singleton.h"

#include "core/port/tThreadLocalCache.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/tAbstractPort.h"
#include "core/port/std/tPortDataReference.h"
#include "core/plugin/tPlugin.h"

namespace finroc
{
namespace core
{
namespace internal
{
template <typename T>
struct ThreadLocalCacheInfosCreator
{
  static T* Create()
  {
    return new T();
  }
  static void Destroy(T* object)
  {
    delete tThreadLocalCache::info;
    delete object;
  }
};

struct tCacheInfo
{
  std::vector<tThreadLocalCache*> list;
  rrlib::thread::tRecursiveMutex mutex;

  tCacheInfo() : list(), mutex("Cache Info Lock", tLockOrderLevels::cINNER_MOST - 100) {}
};

static inline unsigned int GetLongevity(tCacheInfo*)
{
  return 0xEFFFFFFE; // delete after thread cleanup
}

}

typedef rrlib::design_patterns::tSingletonHolder<internal::tCacheInfo, rrlib::design_patterns::singleton::Longevity, internal::ThreadLocalCacheInfosCreator> tThreadLocalCacheInfos;

__thread tThreadLocalCache* tThreadLocalCache::info = NULL;
rrlib::thread::tRecursiveMutex* tThreadLocalCache::infos_mutex = &tThreadLocalCacheInfos::Instance().mutex;
util::tAtomicInt tThreadLocalCache::thread_uid_counter(1);

tThreadLocalCache::tThreadLocalCache() :
  thread_uid(thread_uid_counter.GetAndIncrement()),
  auto_locks(),
  cc_auto_locks(),
  cc_inter_auto_locks(),
  data(NULL),
  ref(NULL),
  last_written_to_port(tCoreRegister<tAbstractPort*>::GetMaximumNumberOfElements()),
  cc_type_pools(tFinrocTypeInfo::cMAX_CCTYPES),
  pq_fragments(new util::tReusablesPool<tPortQueueElement>()),
  ccpq_fragments(new util::tReusablesPool<tCCPortQueueElement>()),
  input_packet_processor(),
  thread_id(rrlib::thread::tThread::CurrentThreadId()),
  port_register(tRuntimeEnvironment::GetInstance()->GetPorts())
{
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Creating ThreadLocalCache for thread ", rrlib::thread::tThread::CurrentThread().GetName());
}

tThreadLocalCache::~tThreadLocalCache()
{
  internal::tCacheInfo& infos = tThreadLocalCacheInfos::Instance();
  rrlib::thread::tLock l(infos.mutex);
  infos.list.erase(std::remove(infos.list.begin(), infos.list.end(), this), infos.list.end());

  FinalDelete();
}

void tThreadLocalCache::AddAutoLock(rrlib::rtti::tGenericObject* obj)
{
  rrlib::rtti::tGenericObjectManager* mgr = obj->GetManager();
  if (tFinrocTypeInfo::IsCCType(obj->GetType()))
  {
    if (typeid(*mgr) == typeid(tCCPortDataManager))
    {
      AddAutoLock(static_cast<tCCPortDataManager*>(mgr));
    }
    else
    {
      AddAutoLock(static_cast<tCCPortDataManagerTL*>(mgr));
    }
  }
  else
  {
    AddAutoLock(static_cast<tPortDataManager*>(mgr));
  }
}

void tThreadLocalCache::AddAutoLock(tCCPortDataManagerTL* obj)
{
  assert((obj != NULL));
  assert((obj->GetOwnerThread() == rrlib::thread::tThread::CurrentThreadId()));
  cc_auto_locks.push_back(obj);
}

tCCPortDataBufferPool* tThreadLocalCache::CreateCCPool(const rrlib::rtti::tDataTypeBase& data_type, int16 uid)
{
  tCCPortDataBufferPool* pool = new tCCPortDataBufferPool(data_type, 10);  // create 10 buffers by default
  cc_type_pools[uid] = pool;
  return pool;
}

tThreadLocalCache* tThreadLocalCache::CreateThreadLocalCacheForThisThread()
{
  internal::tCacheInfo& infos = tThreadLocalCacheInfos::Instance();
  rrlib::thread::tLock lock4(infos.mutex);
  tThreadLocalCache* tli = new tThreadLocalCache();
  infos.list.push_back(tli);
  info = tli;
  if (infos.list.size() > 1)
  {
    rrlib::thread::tThread::CurrentThread().LockObject(std::shared_ptr<tThreadLocalCache>(tli)); // for auto-deleting after thread finishes
  }
  return tli;
}

void tThreadLocalCache::DeleteInfoForPort(int port_index)
{
  assert((port_index >= 0 && port_index <= tCoreRegister<tAbstractPort*>::GetMaximumNumberOfElements()));
  {
    internal::tCacheInfo& infos = tThreadLocalCacheInfos::Instance();
    rrlib::thread::tLock lock2(infos.mutex);
    for (int i = 0, n = infos.list.size(); i < n; i++)
    {
      tThreadLocalCache* tli = infos.list[i];

      // Release port data lock
      tCCPortDataManagerTL* pd = tli->last_written_to_port[port_index];
      if (pd != NULL)
      {
        tli->last_written_to_port[port_index] = NULL;
        pd->NonOwnerLockRelease(tli->GetCCPool(pd->GetObject()->GetType()));
      }

    }
  }
}

void tThreadLocalCache::FinalDelete()
{
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Deleting ThreadLocalCache for thread ", rrlib::thread::tThread::CurrentThread().GetName());

  /*! Delete local port data buffer pools */
  for (size_t i = 0u; i < cc_type_pools.length; i++)
  {
    if (cc_type_pools[i] != NULL)
    {
      cc_type_pools[i]->ControlledDelete();
    }
  }

  pq_fragments->ControlledDelete();
  ccpq_fragments->ControlledDelete();

  /*! Transfer ownership of remaining port data to ports */
  {
    internal::tCacheInfo& infos = tThreadLocalCacheInfos::Instance();
    rrlib::thread::tLock lock2(infos.mutex);  // big lock - to make sure no ports are deleted at the same time which would result in a mess (note that CCPortBase desctructor has synchronized operation on infos)
    for (size_t i = 0u; i < last_written_to_port.length; i++)
    {
      if (last_written_to_port[i] != NULL)
      {
        // this is safe, because we locked runtime (even the case if managedDelete has already been called - because destructor needs runtime lock and unregisters)
        (static_cast<tCCPortBase*>(port_register->GetByRawIndex(i)))->TransferDataOwnership(last_written_to_port[i]);
      }
    }
  }
}

tCCPortDataManager* tThreadLocalCache::GetUnusedInterThreadBuffer(const rrlib::rtti::tDataTypeBase& data_type)
{
  tCCPortDataManager* buf = GetCCPool(data_type)->GetUnusedInterThreadBuffer();
  //System.out.println("Getting unused interthread buffer: " + buf.hashCode());
  return buf;
}

void tThreadLocalCache::ReleaseAllLocks()
{
  for (auto it = auto_locks.begin(); it != auto_locks.end(); ++it)
  {
    (*it)->GetCurReference()->GetRefCounter()->ReleaseLock();
  }
  auto_locks.clear();
  for (auto it = cc_auto_locks.begin(); it != cc_auto_locks.end(); ++it)
  {
    (*it)->ReleaseLock();
  }
  cc_auto_locks.clear();
  for (auto it = cc_inter_auto_locks.begin(); it != cc_inter_auto_locks.end(); ++it)
  {
    (*it)->Recycle2();
  }
  cc_inter_auto_locks.clear();
}

namespace internal
{
class tThreadLocalCachePlugin : public tPlugin
{
  virtual void Init()
  {
    tThreadLocalCache::Get(); // Init for main thread
  }
};

static tThreadLocalCachePlugin thread_local_cache_plugin;
}

} // namespace finroc
} // namespace core

