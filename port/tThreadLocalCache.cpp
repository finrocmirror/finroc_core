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
#include "core/port/tThreadLocalCache.h"
#include "core/tRuntimeEnvironment.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "rrlib/serialization/tGenericObjectManager.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/rpc/tMethodCallSyncher.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/tAbstractPort.h"
#include "core/port/std/tPortDataReference.h"

namespace finroc
{
namespace core
{
// This 'lock' ensures that Thread info is deallocated after last ThreadLocalCache
util::tThreadInfoLock thread_info_lock = util::tThread::GetThreadInfoLock();
// This 'lock' ensures that static AutoDeleter instance is deallocated after last ThreadLocalCache
std::shared_ptr<util::tAutoDeleter> auto_deleter_lock(util::tAutoDeleter::GetStaticInstance());
util::tFastStaticThreadLocal<tThreadLocalCache, tThreadLocalCache, util::tGarbageCollector::tFunctor> tThreadLocalCache::info;

std::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > tThreadLocalCache::infos;
util::tAtomicInt tThreadLocalCache::thread_uid_counter(1);

tThreadLocalCache::tThreadLocalCache() :
    infos_lock(infos),
    method_syncher(NULL),
    thread_uid(thread_uid_counter.GetAndIncrement()),
    auto_locks(),
    cc_auto_locks(),
    cc_inter_auto_locks(),
    data(NULL),
    ref(NULL),
    last_written_to_port(tCoreRegister<>::cMAX_ELEMENTS),
    cc_type_pools(tFinrocTypeInfo::cMAX_CCTYPES),
    method_calls(new util::tReusablesPool<tMethodCall>()),
    pull_calls(new util::tReusablesPool<tPullCall>()),
    pq_fragments(new util::tReusablesPool<tPortQueueElement>()),
    ccpq_fragments(new util::tReusablesPool<tCCPortQueueElement>()),
    input_packet_processor(),
    thread_id(util::sThreadUtil::GetCurrentThreadId()),
    port_register(tRuntimeEnvironment::GetInstance()->GetPorts())
{
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain, "Creating ThreadLocalCache for thread ", util::tThread::CurrentThread()->GetName());
}

void tThreadLocalCache::AddAutoLock(rrlib::serialization::tGenericObject* obj)
{
  rrlib::serialization::tGenericObjectManager* mgr = obj->GetManager();
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
  assert((obj->GetOwnerThread() == util::sThreadUtil::GetCurrentThreadId()));
  cc_auto_locks.Add(obj);
}

tCCPortDataBufferPool* tThreadLocalCache::CreateCCPool(const rrlib::serialization::tDataTypeBase& data_type, int16 uid)
{
  tCCPortDataBufferPool* pool = new tCCPortDataBufferPool(data_type, 10);  // create 10 buffers by default
  cc_type_pools[uid] = pool;
  return pool;
}

tMethodCall* tThreadLocalCache::CreateMethodCall()
{
  tMethodCall* result = new tMethodCall();
  method_calls->Attach(result, false);
  return result;
}

tPullCall* tThreadLocalCache::CreatePullCall()
{
  tPullCall* result = new tPullCall();
  pull_calls->Attach(result, false);
  return result;
}

void tThreadLocalCache::DeleteInfoForPort(int port_index)
{
  assert((port_index >= 0 && port_index <= tCoreRegister<>::cMAX_ELEMENTS));
  {
    util::tLock lock2(infos);
    for (int i = 0, n = infos->Size(); i < n; i++)
    {
      tThreadLocalCache* tli = infos->Get(i);

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
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain, "Deleting ThreadLocalCache");

  /*! Return MethodCallSyncher to pool */
  if (method_syncher != NULL && (!tRuntimeEnvironment::ShuttingDown()))
  {
    method_syncher->Release();
  }

  /*! Delete local port data buffer pools */
  for (size_t i = 0u; i < cc_type_pools.length; i++)
  {
    if (cc_type_pools[i] != NULL)
    {
      cc_type_pools[i]->ControlledDelete();
    }
  }

  method_calls->ControlledDelete();
  pq_fragments->ControlledDelete();
  pull_calls->ControlledDelete();
  ccpq_fragments->ControlledDelete();

  /*! Transfer ownership of remaining port data to ports */
  {
    util::tLock lock2(infos);  // big lock - to make sure no ports are deleted at the same time which would result in a mess (note that CCPortBase desctructor has synchronized operation on infos)
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

tMethodCallSyncher* tThreadLocalCache::GetMethodSyncher()
{
  if (method_syncher == NULL)
  {
    method_syncher = tMethodCallSyncher::GetFreeInstance(this);
  }
  return method_syncher;
}

tCCPortDataManager* tThreadLocalCache::GetUnusedInterThreadBuffer(const rrlib::serialization::tDataTypeBase& data_type)
{
  tCCPortDataManager* buf = GetCCPool(data_type)->GetUnusedInterThreadBuffer();
  //System.out.println("Getting unused interthread buffer: " + buf.hashCode());
  return buf;
}

void tThreadLocalCache::ReleaseAllLocks()
{
  for (size_t i = 0u, n = auto_locks.Size(); i < n; i++)
  {
    auto_locks.Get(i)->GetCurReference()->GetRefCounter()->ReleaseLock();
  }
  auto_locks.Clear();
  for (size_t i = 0u, n = cc_auto_locks.Size(); i < n; i++)
  {
    cc_auto_locks.Get(i)->ReleaseLock();
  }
  cc_auto_locks.Clear();
  for (size_t i = 0u, n = cc_inter_auto_locks.Size(); i < n; i++)
  {
    cc_inter_auto_locks.Get(i)->Recycle2();
  }
  cc_inter_auto_locks.Clear();
}

std::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > tThreadLocalCache::StaticInit()
{
  infos.reset(new util::tSimpleListWithMutex<tThreadLocalCache*>(tLockOrderLevels::cINNER_MOST - 100));

  return infos;
}

} // namespace finroc
} // namespace core

