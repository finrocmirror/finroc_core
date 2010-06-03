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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__TTHREADLOCALCACHE_H
#define CORE__PORT__TTHREADLOCALCACHE_H

#include "core/portdatabase/tDataType.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPortDataRef.h"
#include "core/tCoreRegister.h"
#include "core/port/cc/tCCPortDataBufferPool.h"
#include "finroc_core_utils/container/tReusablesPool.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tPullCall.h"
#include "core/port/std/tPortQueueElement.h"
#include "core/port/cc/tCCPortQueueElement.h"
#include "core/buffers/tCoreInput.h"
#include "finroc_core_utils/container/tSimpleListWithMutex.h"
#include "finroc_core_utils/container/tSimpleList.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/tAbstractPort.h"
#include "core/tRuntimeEnvironment.h"
#include "core/portdatabase/tTypedObjectImpl.h"

#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
class tMethodCallSyncher;

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
class tThreadLocalCache : public util::tUncopyableObject
{
  friend class tRuntimeEnvironment;
private:

  //  @Struct
  //  public class PortInfo {
  //
  //      /** Element that was last written to port by this thread */
  //      @Ptr PortData lastWrittenToPort;
  //
  //      /** thread-owned data buffer pool for writing to port */
  //      @Ptr PortDataBufferPool dataBufferPool;
  //  }
  //
  //  /** Thread local port information - list index is last part of port handle */
  //  @InCpp("PortInfo portInfo[CoreRegister<>::MAX_ELEMENTS];")
  //  public final PortInfo[] portInfo = new PortInfo[CoreRegister.MAX_ELEMENTS];

  /*! Object to gain fast access to the thread local information */
  static util::tFastStaticThreadLocal<tThreadLocalCache, tThreadLocalCache, util::tGarbageCollector::tFunctor> info;

  /*! List with all ThreadLocalInfo objects... necessary for cleaning up... is deleted with last thread */
  static ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > infos;

  /*! Lock to above - for every cache */
  ::std::tr1::shared_ptr<util::tSimpleListWithMutex<tThreadLocalCache*> > infos_lock;

  //new SimpleList<WeakReference<ThreadLocalCache>>(); // = new SimpleList<WeakReference<ThreadLocalInfo>>(RuntimeSettings.MAX_THREADS.get());

  /*! Index of ThreadLocalCache - unique as long as thread exists */
  // dangerous! index changes, because infos is simple list
  //private final @SizeT int index;

  /*! Thread that info belongs to */
  //private final @SharedPtr Thread thread;

  /*! List with locks... */
  //private final SimpleList<PortData> curLocks = new SimpleList<PortData>();

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

public:

  // maybe TODO: reuse old ThreadLocalInfo objects for other threads - well - would cause a lot of "Verschnitt"

  // at the beginning: diverse cached information

  // pointers to current port information - portInfo1 is typically the publishing port */
  /*public @Ptr PortInfo portInfo1, portInfo2;

  //int iteration;
  //PortDataManager mgr;
  int maskIndex;
  int counterMask;
  int counterIncrement;*/

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

  //  // internal methods for universal access in Java and C++
  //  @InCpp("return portInfo[index].lastWrittenToPort;")
  //  @Inline @ConstMethod private PortData getLastWrittenToPortRaw(@SizeT int index) {
  //      return lastWrittenToPort[index];
  //  }
  //
  //  @InCpp("portInfo[index].lastWrittenToPort = data;")
  //  @Inline private void setLastWrittenToPortRaw(@SizeT int index, @Ptr PortData data) {
  //      lastWrittenToPort[index] = data;
  //  }
  //
  //  @InCpp("return portInfo[index].dataBufferPool;")
  //  @Inline @ConstMethod private @Ptr PortDataBufferPool getPoolRaw(@SizeT int index) {
  //      return dataBufferPools[index];
  //  }
  //
  //  @InCpp("portInfo[index].dataBufferPool = data;")
  //  @Inline private void setPoolRaw(@SizeT int index, @Ptr PortDataBufferPool data) {
  //      dataBufferPools[index] = data;
  //  }
  //
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * \param portHandle Handle of Port
  //   * \return Data that was last written the port by this thread
  //   */
  //  @Inline @ConstMethod public PortData getLastWrittenToPort(int portHandle) {
  //      return getLastWrittenToPortRaw(portHandle & CoreRegister.ELEM_INDEX_MASK);
  //  }
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * Set data that was last written to a port by this thread
  //   *
  //   * \param portHandle Handle of Port
  //   * \param data Data
  //   */
  //  @Inline public void setLastWrittenToPort(int portHandle, PortData data) {
  //      setLastWrittenToPortRaw(portHandle & CoreRegister.ELEM_INDEX_MASK, data);
  //  }
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * Thread needs to be the owner of this buffer
  //   *
  //   * Replace lastWrittenToPort value with new.
  //   * Decrease reference counter of the old one.
  //   *
  //   * \param portHandle Handle of Port
  //   * \param data Data
  //   */
  //  @Inline public void newLastWrittenToPortByOwner(int portHandle, PortData data) {
  //      assert (ThreadUtil.getCurrentThreadId() == data.getManager().getOwnerThread()) : "Thread is not owner";
  //      int index = portHandle & CoreRegister.ELEM_INDEX_MASK;
  //      PortData old = getLastWrittenToPortRaw(index);
  //      if (old != null) {
  //          old.getManager().releaseOwnerLock();
  //      }
  //      setLastWrittenToPort(index, data);
  //  }
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * Thread needs to be the owner of this buffer
  //   *
  //   * Replace lastWrittenToPort value with new.
  //   * Decrease reference counter of the old one.
  //   *
  //   * \param portHandle Handle of Port
  //   * \param data Data
  //   */
  //  @Inline public void newLastWrittenToPort(int portHandle, PortData data) {
  //      int index = portHandle & CoreRegister.ELEM_INDEX_MASK;
  //      PortData old = getLastWrittenToPortRaw(index);
  //      if (old != null) {
  //          old.getManager().releaseLock();
  //      }
  //      setLastWrittenToPortRaw(index, data);
  //  }
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * \param portHandle Handle of Port
  //   * \return Buffer pool for port. May be null, if no data has been written to port yet.
  //   */
  //  @Inline @ConstMethod @Ptr public PortDataBufferPool getBufferPool(int portHandle) {
  //      return getPoolRaw(portHandle & CoreRegister.ELEM_INDEX_MASK);
  //  }
  //
  //  /**
  //   * (Should only be called by port implementations)
  //   *
  //   * \param PortHandle Handle of Port
  //   * \param pool Buffer pool for port.
  //   */
  //  @Inline public void setBufferPool(int portHandle, @Ptr PortDataBufferPool pool) {
  //      setPoolRaw(portHandle & CoreRegister.ELEM_INDEX_MASK, pool);
  //  }
  //
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

  //
  //  /**
  //   * (Should only be called by garbage collector) - outdated method: now handled in cleanup
  //   *
  //   * Check if some threads have stopped. Enqueue their info for deletion.
  //   *
  //   * \param handle Port Handle
  //   */
  //  /*static void cleanupThreads() {
  //      synchronized(infos) {
  //          for (int i = 0, n = infos.size(); i < n; i++) {
  //              final ThreadLocalInfo tli = infos.get(i);
  //              if (!tli.thread.isAlive()) {
  //                  GarbageCollector.deleteDeferred(tli);
  //                  infos.remove(i);
  //                  i--;
  //              }
  //          }
  //      }
  //  }*/
  //

  //
  //  /**
  //   * Cache port management data for specified port data
  //   *
  //   * \param data to cache management data for
  //   */
  //  @Inline public PortDataManager cachePortData(PortData data) {
  //      PortDataManager mgr = data.getManager();
  //      maskIndex = mgr.ownerRefCounter & 0x3;
  //      counterMask = PortDataManager.refCounterMasks[maskIndex];
  //      counterIncrement = PortDataManager.refCounterIncrement[maskIndex];
  //      return mgr;
  //  }

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

  //  /**
  //   * \param pd return port data from other thread
  //   */
  //  @Inline private void returnPortData(CCPortDataContainer<?> pd) {
  //      if (pd.getOwnerThread() == ThreadUtil.getCurrentThreadId()) {
  //          pd.releaseLock();
  //      } else {
  //          returnedBuffers.enqueue(pd);
  //      }
  //  }

  //  void reclaimReturnedBuffers() {
  //      if (!returnedBuffers.isEmpty()) { // does not need to be synchronized, wa?
  //          synchronized(returnedBuffers) {
  //              for (int i = 0, n = returnedBuffers.size(); i < n; i++) {
  //                  returnedBuffers.get(i).releaseLock();
  //              }
  //          }
  //      }
  //  }

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
    //      //System.out.println("dq " + pf.getRegisterIndex());
    //      assert(pf.next2.get().isDummy());
    //      pf.recycled = false;
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
