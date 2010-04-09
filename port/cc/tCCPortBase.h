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

#ifndef CORE__PORT__CC__TCCPORTBASE_H
#define CORE__PORT__CC__TCCPORTBASE_H

#include "core/portdatabase/tDataType.h"
#include "core/port/tAbstractPort.h"
#include "core/port/cc/tCCPortDataRef.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPortQueue.h"
#include "core/port/cc/tCCPortData.h"
#include "core/port/cc/tCCPortListener.h"
#include "core/port/cc/tCCPullRequestHandler.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCQueueFragment.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "finroc_core_utils/thread/sThreadUtil.h"
#include "core/tRuntimeSettings.h"

#include "finroc_core_utils/container/tSafeConcurrentlyIterableList.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for buffer ports.
 *
 * Convention: Protected Methods do not perform any necessary synchronization
 * concerning calling threads (that they are called only once at the same time)
 * This has to be done by all public methods.
 */
class tCCPortBase : public tAbstractPort
{
  template<typename T>
  friend class tCCPort;

protected:

  /*! Edges emerging from this port */
  tAbstractPort::tEdgeList<tCCPortBase*> edges_src;

  /*! Edges ending at this port */
  tAbstractPort::tEdgeList<tCCPortBase*> edges_dest;

  /*! default value - invariant: must never be null if used */
  tCCPortDataRef* default_value;

  /*!
   * current value (set by main thread) - invariant: must never be null - sinnvoll(?)
   * In C++, other lock information is stored in in the last 3 bit - therefore
   * setValueInternal() and getValueInternal() should be used in the common cases.
   */
  tCCPortDataRef* volatile value;

  /*!
   * Data that is currently owned - used to belong to a terminated thread
   */
  tCCPortDataContainer<>* owned_data;

  /*!
   * Is data assigned to port in standard way? Otherwise - for instance, when using queues -
   * the virtual method nonstandardassign will be invoked
   *
   * Hopefully compiler will optimize this, since it's final/const
   */
  const bool standard_assign;

  /*!
   * Optimization - if this is not null that means:
   * - this port is an output port and has one active receiver (stored in this variable)
   * - both ports are standard-assigned
   */
  //public @Ptr CCPortBase std11CaseReceiver; // should not need to be volatile

  /*!
   * Port Index - derived from handle - for speed reasons
   */
  const int port_index;

  /*! Queue for ports with incoming value queue */
  tCCPortQueue<tCCPortData>* queue;

  /*! Listens to port value changes - may be null */
  tCCPortListenerManager<tCCPortData> port_listener;

  /*! Object that handles pull requests - null if there is none (typical case) */
  tCCPullRequestHandler* pull_request_handler;

private:

  /*!
   * Set current value to default value
   */
  inline void ApplyDefaultValue()
  {
    Publish(tThreadLocalCache::Get(), default_value->GetContainer());
  }

  // helper for direct member initialization in C++
  inline static tCCPortDataContainer<>* CreateDefaultValue(tDataType* dt)
  {
    return static_cast<tCCPortDataContainer<>*>(dt->CreateInstance());
  }

  //  protected void receive(@Ptr PortData data, @SizeT int dataRaw, @Ptr CCPortBase origin, @Ptr ThreadLocalCache tli) {
  //    if (standardAssign) {
  //      data.getManager().addReadLock();
  //      tli.setLastWrittenToPort(handle, data);
  //
  //      // JavaOnlyBlock
  //      value = data;
  //
  //       value = data_raw;
  //
  //      changed = true;
  //      notifyListeners();
  //    } else {
  //      nonStandardAssign(data, tli);
  //    }
  //
  //    @Ptr ArrayWrapper<CCPortBase> dests = edgesSrc.getIterable();
  //    for (int i = 0, n = dests.size(); i < n; i++) {
  //      @Ptr CCPortBase pb = dests.get(i);
  //      if (pb != null && (pb.flags | PortFlags.PUSH_STRATEGY) > 0) {
  //        pb.receive(data, dataRaw, this, tli);
  //      }
  //    }
  //
  //    dests = edgesDest.getIterable();
  //    for (int i = 0, n = dests.size(); i < n; i++) {
  //      @Ptr CCPortBase pb = dests.get(i);
  //      if (pb != null && pb != origin && (pb.flags | PortFlags.PUSH_STRATEGY_REVERSE) > 0) {
  //        pb.receiveReverse(data, dataRaw, tli);
  //      }
  //    }
  //  }
  //
  //  protected void receiveAsOwner(@Ptr PortData data, @SizeT int dataRaw, @Ptr CCPortBase origin, @Ptr ThreadLocalCache tli) {
  //    if (standardAssign) {
  //      data.getManager().addOwnerLock();
  //      tli.newLastWrittenToPortByOwner(handle, data);
  //
  //      // JavaOnlyBlock
  //      value = data;
  //
  //       value = data_raw;
  //
  //      changed = true;
  //      notifyListeners();
  //    } else {
  //      nonStandardAssign(data, tli);
  //    }
  //
  //    @Ptr ArrayWrapper<CCPortBase> dests = edgesSrc.getIterable();
  //    for (int i = 0, n = dests.size(); i < n; i++) {
  //      @Ptr CCPortBase pb = dests.get(i);
  //      if (pb != null && (pb.flags | PortFlags.PUSH_STRATEGY) > 0) {
  //        pb.receiveAsOwner(data, dataRaw, this, tli);
  //      }
  //    }
  //
  //    dests = edgesDest.getIterable();
  //    for (int i = 0, n = dests.size(); i < n; i++) {
  //      CCPortBase pb = dests.get(i);
  //      if (pb != null && pb != origin && (pb.flags | PortFlags.PUSH_STRATEGY_REVERSE) > 0) {
  //        pb.receiveReverse(data, dataRaw, tli);
  //      }
  //    }
  //  }
  //
  //  private void receiveReverse(@Ptr PortData data, @SizeT int dataRaw, @Ptr ThreadLocalCache tli) {
  //    if (standardAssign) {
  //      data.getManager().addReadLock();
  //      tli.setLastWrittenToPort(handle, data);
  //
  //      // JavaOnlyBlock
  //      value = data;
  //
  //       value = data_raw;
  //
  //      changed = true;
  //      notifyListeners();
  //    } else {
  //      nonStandardAssign(data, tli);
  //    }
  //  }
  //

  inline void NotifyListeners(tThreadLocalCache* tc)
  {
    port_listener.Notify(this, ((tCCPortData*)tc->data->GetDataPtr()));
  }

  template <bool cREVERSE, int8 cCHANGE_CONSTANT>
  /*!
   * Publish data
   *
   * \param tc ThreadLocalCache
   * \param data Data to publish
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void PublishImpl(tThreadLocalCache* tc, tCCPortDataContainer<>* data, bool reverse, int8 changed_constant)
  {
    assert((data->GetType() != NULL) && "Port data type not initialized");
    assert((IsInitialized()) && "Port not initialized");

    util::tArrayWrapper<tCCPortBase*>* dests = cREVERSE ? edges_dest.GetIterable() : edges_src.GetIterable();

    // assign
    tc->data = data;
    tc->ref = data->GetCurrentRef();
    Assign(tc);

    // later optimization (?) - unroll loops for common short cases
    for (size_t i = 0u; i < dests->Size(); i++)
    {
      tCCPortBase* dest = dests->Get(i);
      bool push = (dest != NULL) && dest->WantsPush<cREVERSE, cCHANGE_CONSTANT>(cREVERSE, cCHANGE_CONSTANT);
      if (push)
      {
        dest->Receive<cREVERSE, cCHANGE_CONSTANT>(tc, this, cREVERSE, cCHANGE_CONSTANT);
      }
    }
  }

  /*!
   * Pull/read current value from source port
   * When multiple source ports are available an arbitrary one of them is used.
   *
   * \param tc ThreadLocalCache
   * \param intermediate_assign Assign pulled value to ports in between?
   * \param first Call on first/originating port?
   * \return Locked port data
   */
  void PullValueRawImpl(tThreadLocalCache* tc, bool intermediate_assign, bool first);

  /*!
   * Update statistics if this is enabled
   *
   * \param tc Initialized ThreadLocalCache
   */
  inline void UpdateStatistics(tThreadLocalCache* tc, tCCPortBase* source, tCCPortBase* target)
  {
    if (tRuntimeSettings::cCOLLECT_EDGE_STATISTICS)    // const, so method can be optimized away completely
    {
      UpdateEdgeStatistics(source, target, tc->data);
    }
  }

protected:

  virtual void ClearQueueImpl()
  {
    queue->Clear(true);
  }

  //  @Override
  //  public TypedObject universalGetAutoLocked() {
  //    CCPortDataRef val = value;
  //    CCPortDataContainer<?> valC = val.getContainer();
  //    if (valC.getOwnerThread() == ThreadUtil.getCurrentThreadId()) { // if same thread: simply add read lock
  //      valC.addLock();
  //      return valC;
  //    }
  //
  //    // not the same thread: create auto-locked inter-thread container
  //    ThreadLocalCache tc = ThreadLocalCache.get();
  //    CCInterThreadContainer<?> ccitc = tc.getUnusedInterThreadBuffer(getDataType());
  //    tc.addAutoLock(ccitc);
  //    for(;;) {
  //      ccitc.assign(valC.getDataPtr());
  //      if (val == value) { // still valid??
  //        return ccitc;
  //      }
  //      val = value;
  //      valC = val.getContainer();
  //    }
  //  }

  /*!
   * Get current data in container owned by this thread with a lock.
   * Attention: User needs to take care of unlocking.
   *
   * \return Container (non-const - public wrapper should return it const)
   */
  tCCPortDataContainer<>* GetLockedUnsafeInContainer();

  virtual int GetMaxQueueLengthImpl() const
  {
    return queue->GetMaxLength();
  }

  /*!
   * (Meant for internal use)
   *
   * \param tc ThreadLocalCache
   * \return Unused buffer for writing
   */
  inline tCCPortDataContainer<>* GetUnusedBuffer(tThreadLocalCache* tc)
  {
    return tc->GetUnusedBuffer(this->data_type);
  }

  virtual void InitialPushTo(tAbstractPort* target, bool reverse);

  /*!
   * Custom special assignment to port.
   * Used, for instance, in queued ports.
   *
   * \param tc ThreadLocalCache with tc.data set
   */
  virtual void NonStandardAssign(tThreadLocalCache* tc);

  /*!
   * Publish data
   *
   * \param tc ThreadLocalCache
   * \param data Data to publish
   */
  inline void Publish(tThreadLocalCache* tc, tCCPortDataContainer<>* data)
  {
    PublishImpl<false, cCHANGED>(tc, data, false, cCHANGED);
  }

  /*!
   * Publish data
   *
   * \param tc ThreadLocalCache
   * \param data Data to publish
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void Publish(tThreadLocalCache* tc, tCCPortDataContainer<>* data, bool reverse, int8 changed_constant)
  {
    if (!reverse)
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<false, cCHANGED>(tc, data, false, cCHANGED);
      }
      else
      {
        PublishImpl<false, cCHANGED_INITIAL>(tc, data, false, cCHANGED_INITIAL);
      }
    }
    else
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<true, cCHANGED>(tc, data, true, cCHANGED);
      }
      else
      {
        PublishImpl<true, cCHANGED_INITIAL>(tc, data, true, cCHANGED_INITIAL);
      }
    }

  }

  /*!
   * Pull/read current value from source port
   * When multiple source ports are available an arbitrary one of them is used.
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   * \return Locked port data (non-const!)
   */
  inline tCCPortDataContainer<>* PullValueRaw()
  {
    return PullValueRaw(true);
  }

  /*!
   * Pull/read current value from source port
   * When multiple source ports are available, an arbitrary one of them is used.
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   * \return Locked port data (current thread is owner; there is one additional lock for caller; non-const(!))
   */
  tCCPortDataContainer<>* PullValueRaw(bool intermediate_assign);

  template <bool cREVERSE, int8 cCHANGE_CONSTANT>
  /*!
   * Receive data from another port
   *
   * \param tc Initialized ThreadLocalCache
   * \param origin Port data originates from
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void Receive(tThreadLocalCache* tc, tCCPortBase* origin, bool reverse, int8 changed_constant)
  {
    // Backup tc references (in case it is modified - e.g. in BoundedNumberPort)
    tCCPortDataContainer<>* old_data = tc->data;
    tCCPortDataRef* old_ref = tc->ref;

    Assign(tc);
    SetChanged(cCHANGE_CONSTANT);
    NotifyListeners(tc);
    UpdateStatistics(tc, origin, this);

    if (!cREVERSE)
    {
      // forward
      util::tArrayWrapper<tCCPortBase*>* dests = edges_src.GetIterable();
      for (int i = 0, n = dests->Size(); i < n; i++)
      {
        tCCPortBase* dest = dests->Get(i);
        bool push = (dest != NULL) && dest->WantsPush<false, cCHANGE_CONSTANT>(false, cCHANGE_CONSTANT);
        if (push)
        {
          dest->Receive<false, cCHANGE_CONSTANT>(tc, this, false, cCHANGE_CONSTANT);
        }
      }

      // reverse
      dests = edges_dest.GetIterable();
      for (int i = 0, n = dests->Size(); i < n; i++)
      {
        tCCPortBase* dest = dests->Get(i);
        bool push = (dest != NULL) && dest->WantsPush<true, cCHANGE_CONSTANT>(true, cCHANGE_CONSTANT);
        if (push && dest != origin)
        {
          dest->Receive<true, cCHANGE_CONSTANT>(tc, this, true, cCHANGE_CONSTANT);
        }
      }
    }

    // restore tc references
    tc->data = old_data;
    tc->ref = old_ref;
  }

  virtual void SetMaxQueueLengthImpl(int length);

public:

  /*!
   * \param pci PortCreationInformation
   */
  tCCPortBase(tPortCreationInfo pci);

  /*!
   * \param listener Listener to add
   */
  inline void AddPortListenerRaw(tCCPortListener<>* listener)
  {
    port_listener.Add(listener);
  }

  /*!
   * Publishes new data to port.
   * Releases and unlocks old data.
   * Lock on new data has to be set before
   *
   * \param pdr ThreadLocalCache with tc.data set
   */
  inline void Assign(tThreadLocalCache* tc)
  {
    if (!standard_assign)
    {
      NonStandardAssign(tc);
    }

    // assign anyway
    tc->data->AddLock();
    tCCPortDataContainer<>* pdc = tc->last_written_to_port[port_index];
    if (pdc != NULL)
    {
      pdc->ReleaseLock();
    }
    tc->last_written_to_port[port_index] = tc->data;
    value = tc->ref;
  }

  virtual ~tCCPortBase();

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  void DequeueAllRaw(tCCQueueFragment<tCCPortData>& fragment);

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * Container is autoLocked and is recycled with next ThreadLocalCache.get().releaseAllLocks()
   * (Use only with ports that have a input queue)
   *
   * \return Dequeued first/oldest element in queue
   */
  inline tCCPortData* DequeueSingleAutoLockedRaw()
  {
    tCCInterThreadContainer<>* result = DequeueSingleUnsafeRaw();
    if (result == NULL)
    {
      return NULL;
    }
    tThreadLocalCache::Get()->AddAutoLock(result);
    return result->GetData();
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * Container needs to be recycled manually by caller!
   * (Use only with ports that have a input queue)
   *
   * \return Dequeued first/oldest element in queue
   */
  tCCInterThreadContainer<>* DequeueSingleUnsafeRaw();

  /*!
   * \return Current data with auto-lock (can only be unlocked with ThreadLocalCache auto-unlock)
   */
  inline const tCCPortData* GetAutoLockedRaw()
  {
    tCCPortDataRef* val = value;
    tCCPortDataContainer<>* val_c = val->GetContainer();
    if (val_c->GetOwnerThread() == util::sThreadUtil::GetCurrentThreadId())    // if same thread: simply add read lock
    {
      val_c->AddLock();
      return ((tCCPortData*)val_c->GetDataPtr());
    }

    // not the same thread: create auto-locked inter-thread container
    tThreadLocalCache* tc = tThreadLocalCache::Get();
    tCCInterThreadContainer<>* ccitc = tc->GetUnusedInterThreadBuffer(GetDataType());
    tc->AddAutoLock(ccitc);
    for (; ;)
    {
      ccitc->Assign(((tCCPortData*)val_c->GetDataPtr()));
      if (val == value)    // still valid??
      {
        return ((tCCPortData*)ccitc->GetDataPtr());
      }
      val = value;
      val_c = val->GetContainer();
    }
  }

  /*!
   * \return Current data in CC Interthread-container. Needs to be recycled manually.
   */
  tCCInterThreadContainer<>* GetInInterThreadContainer();

  //  /**
  //   * Pulls port data (regardless of strategy)
  //   * (careful: no auto-release of lock)
  //   * \param intermediateAssign Assign pulled value to ports in between?
  //   *
  //   * \return Pulled locked data
  //   */
  //  public @Const CCPortDataContainer<?> getPullLockedUnsafeRaw(boolean intermediateAssign) {
  //    return pullValueRaw(intermediateAssign);
  //  }

  /*!
   * Pulls port data (regardless of strategy) and returns it in interhread container
   * (careful: no auto-release of lock)
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  tCCInterThreadContainer<>* GetPullInInterthreadContainerRaw(bool intermediate_assign);

  /*!
   * Copy current value to buffer (Most efficient get()-version)
   *
   * \param buffer Buffer to copy current data
   */
  void GetRaw(tCCInterThreadContainer<>* buffer);

  /*!
   * Copy current value to buffer (Most efficient get()-version)
   *
   * \param buffer Buffer to copy current data
   */
  void GetRaw(tCCPortDataContainer<>* buffer);

  /*!
   * Copy current value to buffer (Most efficient get()-version)
   *
   * \param buffer Buffer to copy current data to
   */
  void GetRaw(tCCPortData* buffer);

  virtual void NotifyDisconnect();

  /*!
   * Publish buffer through port
   *
   * \param read_object Buffer with data (must be owned by current thread)
   */
  inline void Publish(tCCPortDataContainer<>* buffer)
  {
    assert((buffer->GetOwnerThread() == util::sThreadUtil::GetCurrentThreadId()));
    Publish(tThreadLocalCache::GetFast(), buffer);
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListenerRaw(tCCPortListener<>* listener)
  {
    port_listener.Remove(listener);
  }

  //  @Override
  //  public void invokeCall(PullCall call) {
  //    if (pullValueRaw(call, ThreadLocalCache.get())) {
  //      SynchMethodCallLogic.handleMethodReturn(call);
  //    }
  //  }
  //
  //  /**
  //   * Pull/read current value from source port
  //   * When multiple source ports are available an arbitrary one of them is used.
  //   * (Should only be called by framework-internal classes)
  //   *
  //   * \param pc Various parameters
  //   * \param tc Thread's ThreadLocalCache instance
  //   * \return already returning pulled value (in same thread)
  //   */
  //  @Virtual public boolean pullValueRaw(PullCall call, ThreadLocalCache tc) {
  //    @Ptr ArrayWrapper<CCPortBase> sources = edgesDest.getIterable();
  //    if (pullRequestHandler != null) {
  //      call.data = tc.getUnusedInterThreadBuffer(getDataType());
  //      call.setStatusReturn();
  //      pullRequestHandler.pullRequest(this, call.data.getDataPtr());
  //      call.setupThreadLocalCache();
  //      assign(tc);
  //    } else {
  //
  //      // continue with next-best connected source port
  //      for (@SizeT int i = 0, n = sources.size(); i < n; i++) {
  //        CCPortBase pb = sources.get(i);
  //        if (pb != null) {
  //          call.pushCaller(this);
  //          boolean returning = pb.pullValueRaw(call, tc);
  //          if (returning) {
  //            @CppUnused
  //            int x = call.popCaller(); // we're already returning, so we can remove ourselves from caller stack again
  //            assert(x == getHandle());
  //            if (!value.getContainer().contentEquals(call.data.getDataPtr())) { // exploit thread for the calls he made anyway
  //              call.setupThreadLocalCache();
  //              assign(tc);
  //            }
  //          }
  //          if (call.getStatus() != AbstractCall.CONNECTION_EXCEPTION) {
  //            return returning;
  //          }
  //        }
  //      }
  //
  //      // no connected source port... pull current value
  //      call.data = getInInterThreadContainer();
  //      call.setStatusReturn();
  //    }
  //    return true;
  //  }
  //
  //  @Override
  //  public void handleCallReturn(AbstractCall call) {
  //    assert(call.isReturning(true));
  //
  //    PullCall pc = (PullCall)call;
  //    if (!value.getContainer().contentEquals(pc.data.getDataPtr())) {
  //      pc.setupThreadLocalCache();
  //      assign(pc.tc);
  //    }
  //
  //    // continue assignments
  //    if (pc.callerStackSize() > 0) {
  //      pc.returnToCaller();
  //    } else {
  //      SynchMethodCallLogic.handleMethodReturn(pc);
  //    }
  //  }

  /*!
   * \param pull_request_handler Object that handles pull requests - null if there is none (typical case)
   */
  void SetPullRequestHandler(tCCPullRequestHandler* pull_request_handler_);

  /*!
   * Transfers data ownership to port after a thread has been deleted
   *
   * \param port_data_container port data container to transfer ownership of
   */
  void TransferDataOwnership(tCCPortDataContainer<>* port_data_container);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCPORTBASE_H
