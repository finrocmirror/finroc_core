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

#ifndef core__port__tThreadLocalCache_h__
#define core__port__tThreadLocalCache_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/tGarbageCollector.h"
#include "rrlib/finroc_core_utils/container/tReusablesPool.h"
#include "rrlib/finroc_core_utils/container/tBoundedQElementContainer.h"
#include "rrlib/finroc_core_utils/container/tSimpleListWithMutex.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/serialization/tInputStream.h"

#include "core/tAnnotatable.h"
#include "core/tCoreRegister.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortDataBufferPool.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortQueueElement.h"
#include "core/port/std/tPortQueueElement.h"

namespace finroc
{
namespace core
{
namespace internal
{
class tThreadLocalCachePlugin;
}
class tCCPortDataRef;
class tPortDataManager;
class tCCPortDataManager;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Various (cached) information that exists for every thread.
 * This class should reside on a thread's stack and is passed
 * through port publishing methods - caching all kinds of
 * information...
 * This should lead to a close-to-optimum optimization.
 *
 * Obviously, this class is somewhat critical for overall performance.
 */
class tThreadLocalCache : public tAnnotatable
{
  friend class tRuntimeEnvironment;
  friend class internal::tThreadLocalCachePlugin;
  friend class tCCPortBase;
  friend class tCCPortDataBufferPool;
private:

  /*! to gain fast access to the thread local information */
  static __thread tThreadLocalCache* info;

  /*! Pointer to aboves list mutex */
  static util::tMutexLockOrder* infos_mutex;

  /*! Uid of thread - unique and constant for runtime of program */
  const int thread_uid;

  /*! Object to retrieve uids from */
  static util::tAtomicInt thread_uid_counter;

  /*! Automatic locks - are released/recycled with releaseAllLocks() */
  util::tSimpleList<tPortDataManager*> auto_locks;

  util::tSimpleList<tCCPortDataManagerTL*> cc_auto_locks;

  util::tSimpleList<tCCPortDataManager*> cc_inter_auto_locks;

public:

  // maybe TODO: reuse old ThreadLocalInfo objects for other threads - well - would cause a lot of "Verschnitt"

  // at the beginning: diverse cached information

  tCCPortDataManagerTL* data;

  tCCPortDataRef* ref;

  // ThreadLocal port information

  /*! Contains port data that was last written to every port - list index is last part of port handle (see CoreRegister) */
  ::finroc::util::tArrayWrapper<tCCPortDataManagerTL*> last_written_to_port;

  /*! Thread-local pools of buffers for every "cheap-copy" port data type */
  ::finroc::util::tArrayWrapper<tCCPortDataBufferPool*> cc_type_pools;

  /*! Queue fragment chunks that are reused */
  util::tReusablesPool<tPortQueueElement>* pq_fragments;

  /*! CC Queue fragment chunks that are reused */
  util::tReusablesPool<tCCPortQueueElement>* ccpq_fragments;

  /*! CoreInput for Input packet processor */
  rrlib::serialization::tInputStream input_packet_processor;

  /*! Thread ID as returned by ThreadUtil::getCurrentThreadId() */
  int64 thread_id;

  /*! Port Register - we need to have this for clean thread cleanup */
  std::shared_ptr<const tCoreRegister<tAbstractPort*>> port_register;

private:

  tThreadLocalCache();

  tCCPortDataBufferPool* CreateCCPool(const rrlib::rtti::tDataTypeBase& data_type, int16 uid);

  static tThreadLocalCache* CreateThreadLocalCacheForThisThread();

  /*!
   * Delete Object and cleanup entries in arrays
   */
  void FinalDelete();

  inline tCCPortDataBufferPool* GetCCPool(const rrlib::rtti::tDataTypeBase& data_type)
  {
    return GetCCPool(tFinrocTypeInfo::Get(data_type).GetCCIndex());
  }

  inline tCCPortDataBufferPool* GetCCPool(int16 cc_type_index)
  {
    assert((cc_type_index >= 0));
    tCCPortDataBufferPool* pool = cc_type_pools[cc_type_index];
    if (pool == NULL)
    {
      pool = CreateCCPool(tFinrocTypeInfo::GetFromCCIndex(cc_type_index), cc_type_index);
    }
    return pool;
  }

public:

  virtual ~tThreadLocalCache();

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  void AddAutoLock(rrlib::rtti::tGenericObject* obj);

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  inline void AddAutoLock(tPortDataManager* obj)
  {
    assert((obj != NULL));
    auto_locks.Add(obj);
  }

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  void AddAutoLock(tCCPortDataManagerTL* obj);

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  inline void AddAutoLock(tCCPortDataManager* obj)
  {
    assert((obj != NULL));
    cc_inter_auto_locks.Add(obj);
  }

  /*!
   * (Should only be called by port)
   *
   * Delete all information regarding port with specified handle
   *
   * \param port_index Port's raw index
   */
  static void DeleteInfoForPort(int port_index);

  /*!
   * \return Thread local information
   */
  inline static tThreadLocalCache* Get()
  {
    tThreadLocalCache* tli = info;
    if (!tli)
    {
      tli = CreateThreadLocalCacheForThisThread();
    }
    return tli;
  }

  /*!
   * (quick version of above)
   * (potentially unsafe: ThreadLocalCache of current thread must have been initialized)
   *
   * \return Thread local information
   */
  inline static tThreadLocalCache* GetFast()
  {
    assert(info);
    return info;
  }

  /*!
   * \return Uid of thread - unique and constant for runtime of program
   */
  inline int GetThreadUid()
  {
    return thread_uid;
  }

  inline tCCPortDataManagerTL* GetUnusedBuffer(int16 cc_type_index)
  {
    return GetCCPool(cc_type_index)->GetUnusedBuffer();
  }

  inline tCCPortDataManagerTL* GetUnusedBuffer(const rrlib::rtti::tDataTypeBase& data_type)
  {
    return GetCCPool(data_type)->GetUnusedBuffer();
  }

  inline tCCPortQueueElement* GetUnusedCCPortQueueFragment()
  {
    tCCPortQueueElement* pf = ccpq_fragments->GetUnused();
    if (pf == NULL)
    {
      pf = new tCCPortQueueElement();
      ccpq_fragments->Attach(pf, false);
      assert((pf->next2.Get()->IsDummy()));
    }
    //      assert(pf.recycled);
    //      assert(pf.next2.get().isDummy());
    return pf;
  }

  tCCPortDataManager* GetUnusedInterThreadBuffer(const rrlib::rtti::tDataTypeBase& data_type);

  inline tPortQueueElement* GetUnusedPortQueueFragment()
  {
    tPortQueueElement* pf = pq_fragments->GetUnused();
    if (pf == NULL)
    {
      pf = new tPortQueueElement();
      pq_fragments->Attach(pf, false);
    }
    return pf;
  }

  /*!
   * Releases locks on objects that have been auto-locked and possibly recycle them
   */
  void ReleaseAllLocks();

};

} // namespace finroc
} // namespace core

#endif // core__port__tThreadLocalCache_h__
