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
#include "rrlib/finroc_core_utils/tJCBase.h"
#include "core/portdatabase/tDataType.h"

#ifndef CORE__PORT__TTHREADLOCALCACHE_H
#define CORE__PORT__TTHREADLOCALCACHE_H

#include "rrlib/finroc_core_utils/tGarbageCollector.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tPullCall.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/tCoreRegister.h"
#include "core/port/cc/tCCPortDataBufferPool.h"
#include "rrlib/finroc_core_utils/container/tReusablesPool.h"
#include "core/buffers/tCoreInput.h"
#include "rrlib/finroc_core_utils/container/tSimpleListWithMutex.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/cc/tCCPortQueueElement.h"
#include "rrlib/finroc_core_utils/container/tBoundedQElementContainer.h"
#include "core/port/std/tPortQueueElement.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace finroc
{
namespace core
{
class tCCPortDataRef;
class tMethodCallSyncher;
class tPortData;
class tAbstractPort;
class tTypedObject;

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
class tThreadLocalCache : public util::tLogUser
{
  friend class tRuntimeEnvironment;
private:

  /*! Object to gain fast access to the thread local information */
  static util::tFastStaticThreadLocal<tThreadLocalCache, tThreadLocalCache, util::tGarbageCollector::tFunctor> info;

  /*! List with all ThreadLocalInfo objects... necessary for cleaning up... is deleted with last thread */
  static ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > infos;

  /*! Lock to above - for every cache */
  ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > infos_lock;

  /*! object to help synchronize method calls - lazily initialized */
  tMethodCallSyncher* method_syncher;

  /*! Uid of thread - unique and constant for runtime of program */
  const int thread_uid;

  /*! Object to retrieve uids from */
  static util::tAtomicInt thread_uid_counter;

  /*! Automatic locks - are released/recycled with releaseAllLocks() */
  util::tSimpleList<const tPortData*> auto_locks;

  util::tSimpleList<tCCPortDataContainer<>*> cc_auto_locks;

  util::tSimpleList<tCCInterThreadContainer<>*> cc_inter_auto_locks;

protected:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "thread_local_cache");

public:

  // maybe TODO: reuse old ThreadLocalInfo objects for other threads - well - would cause a lot of "Verschnitt"

  // at the beginning: diverse cached information

  tCCPortDataContainer<>* data;

  tCCPortDataRef* ref;

  // ThreadLocal port information

  /*! Contains port data that was last written to every port - list index is last part of port handle (see CoreRegister) */
  ::finroc::util::tArrayWrapper<tCCPortDataContainer<>*> last_written_to_port;

  /*! Thread-local pools of buffers for every "cheap-copy" port data type */
  ::finroc::util::tArrayWrapper<tCCPortDataBufferPool*> cc_type_pools;

  /*! Reusable objects representing a method call */
  util::tReusablesPool<tMethodCall>* method_calls;

  /*! Reusable objects representing a pull call */
  util::tReusablesPool<tPullCall>* pull_calls;

  /*! Queue fragment chunks that are reused */
  util::tReusablesPool<tPortQueueElement>* pq_fragments;

  /*! CC Queue fragment chunks that are reused */
  util::tReusablesPool<tCCPortQueueElement>* ccpq_fragments;

  /*! CoreInput for Input packet processor */
  tCoreInput input_packet_processor;

  /*! Thread ID as reuturned by ThreadUtil::getCurrentThreadId() */
  int64 thread_id;

  /*! Port Register - we need to have this for clean thread cleanup */
  ::std::tr1::shared_ptr<const tCoreRegister<tAbstractPort*> > port_register;

private:

  tThreadLocalCache();

  tCCPortDataBufferPool* CreateCCPool(tDataType* data_type, int16 uid);

  tMethodCall* CreateMethodCall();

  tPullCall* CreatePullCall();

  /*!
   * Delete Object and cleanup entries in arrays
   */
  void FinalDelete();

  inline tCCPortDataBufferPool* GetCCPool(tDataType* data_type)
  {
    int16 uid = data_type->GetUid();
    tCCPortDataBufferPool* pool = cc_type_pools[uid];
    if (pool == NULL)
    {
      pool = CreateCCPool(data_type, uid);
    }
    return pool;
  }

public:

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  void AddAutoLock(tTypedObject* obj);

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  inline void AddAutoLock(const tPortData* obj)
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
  void AddAutoLock(tCCPortDataContainer<>* obj);

  /*!
   * Add object that will be automatically unlocked/recycled
   * when releaseAllLocks() is called
   *
   * \param obj Object
   */
  inline void AddAutoLock(tCCInterThreadContainer<>* obj)
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
    tThreadLocalCache* tli = info.GetFast();
    if (tli == NULL)
    {
      {
        util::tLock lock4(infos);
        tli = new tThreadLocalCache();

        infos->Add(tli);

        info.Set(tli);
      }
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
    assert((info.GetFast() != NULL));
    return info.GetFast();
  }

  /*!
   * \return Shared Pointer to List with all ThreadLocalInfo objects... necessary for clean cleaning up
   *
   * (should only be called by CCPortDataBufferPool)
   */
  inline ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > GetInfosLock()
  {
    return infos_lock;
  }

  tMethodCallSyncher* GetMethodSyncher();

  /*!
   * \return Uid of thread - unique and constant for runtime of program
   */
  inline int GetThreadUid()
  {
    return thread_uid;
  }

  inline tCCPortDataContainer<>* GetUnusedBuffer(tDataType* data_type)
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

  tCCInterThreadContainer<>* GetUnusedInterThreadBuffer(tDataType* data_type);

  inline tMethodCall* GetUnusedMethodCall()
  {
    tMethodCall* pf = method_calls->GetUnused();
    if (pf == NULL)
    {
      pf = CreateMethodCall();
    }
    //pf.responsibleThread = ThreadUtil.getCurrentThreadId();
    return pf;
  }

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

  inline tPullCall* GetUnusedPullCall()
  {
    tPullCall* pf = pull_calls->GetUnused();
    if (pf == NULL)
    {
      pf = CreatePullCall();
    }
    //pf.responsibleThread = ThreadUtil.getCurrentThreadId();
    //System.out.println("Dequeueing pull call: " + pf.toString());
    return pf;
  }

  /*!
   * Releases locks on objects that have been auto-locked and possibly recycle them
   */
  void ReleaseAllLocks();

  virtual ~tThreadLocalCache()
  {
    {
      util::tLock l(infos);
      tThreadLocalCache* tmp = this; // to cleanly remove const modifier
      infos->RemoveElem(tmp);
    }

    FinalDelete();
  }

  static ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > StaticInit();

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__TTHREADLOCALCACHE_H
