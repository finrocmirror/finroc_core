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

#ifndef CORE__PORT__STD__TPORTBASE_H
#define CORE__PORT__STD__TPORTBASE_H

#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/port/tAbstractPort.h"
#include "core/port/std/tPortQueue.h"
#include "core/port/std/tPortListener.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/std/tPortDataReference.h"
#include "core/port/std/tPortQueueFragment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/tFrameworkElement.h"
#include "core/port/tMultiTypePortDataBufferPool.h"
#include "core/port/std/tPortDataBufferPool.h"
#include "core/port/tPortFlags.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/std/tPublishCache.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
class tDataType;
class tPullRequestHandler;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for buffer ports.
 *
 * Convention: Protected Methods do not perform any necessary synchronization
 * concerning calling threads (that they are called only once at the same time)
 * This has to be done by all public methods.
 */
class tPortBase : public tAbstractPort
{
  template<typename T>
  friend class tPort;

protected:
  /*implements Callable<PullCall>*/

  /*! Edges emerging from this port */
  tAbstractPort::tEdgeList<tPortBase*> edges_src;

  /*! Edges ending at this port */
  tAbstractPort::tEdgeList<tPortBase*> edges_dest;

  /*! default value - invariant: must never be null if used */
  tPortData* default_value;

  /*!
   * current value (set by main thread) - invariant: must never be null - sinnvoll(?)
   * In C++, other lock information is stored in in the last 3 bit - therefore
   * setValueInternal() and getValueInternal() should be used in the common cases.
   */
  util::tAtomicPtr<tPortDataReference> value;

  /*! Current type of port data - relevant for ports with multi type buffer pool */
  tDataType* cur_data_type;

  /*! Pool with reusable buffers that are published to this port... by any thread */
  tPortDataBufferPool* buffer_pool;

  /*! Pool with different types of reusable buffers that are published to this port... by any thread - either of these pointers in null */
  tMultiTypePortDataBufferPool* multi_buffer_pool;

  /*!
   * Is data assigned to port in standard way? Otherwise - for instance, when using queues -
   * the virtual method
   *
   * Hopefully compiler will optimize this, since it's final/const
   */
  const bool standard_assign;

  /*! Queue for ports with incoming value queue */
  tPortQueue<tPortData>* queue;

  /*!
   * Optimization - if this is not null that means:
   * - this port is an output port and has one active receiver (stored in this variable)
   * - both ports are standard-assigned
   */
  //public @Ptr PortBase std11CaseReceiver; // should not need to be volatile

  /*! Object that handles pull requests - null if there is none (typical case) */
  tPullRequestHandler* pull_request_handler;

  /*! Listens to port value changes - may be null */
  tPortListenerManager<tPortData> port_listener;

private:

  /*!
   * Set current value to default value
   */
  inline void ApplyDefaultValue()
  {
    Publish(default_value);
  }

  // helper for direct member initialization in C++
  static tPortData* CreateDefaultValue(tDataType* dt);

  inline void NotifyListeners(tPublishCache* pc)
  {
    port_listener.Notify(this, pc->cur_ref->GetData());
  }

  /*! makes adjustment to flags passed through constructor */
  static tPortCreationInfo& ProcessPci(tPortCreationInfo& pci);

  template <bool cREVERSE, int8 cCHANGE_CONSTANT, bool cINFORM_LISTENERS>
  /*!
   * (only for use by port classes)
   *
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * It should not be modified thereafter.
   * Should only be called on output ports
   *
   * \param cnc Data buffer acquired from a port using getUnusedBuffer
   * \param reverse Publish in reverse direction? (typical is forward)
   * \param changed_constant changedConstant to use
   * \param inform_listeners Inform this port's listeners on change? (usually only when value comes from browser)
   */
  inline void PublishImpl(const tPortData* data, bool reverse = false, int8 changed_constant = cCHANGED, bool inform_listeners = false)
  {
    assert((data->GetType() != NULL) && "Port data type not initialized");
    assert((data->GetManager() != NULL) && "Only port data obtained from a port can be sent");
    assert(IsInitialized() || cINFORM_LISTENERS);

    // assign
    util::tArrayWrapper<tPortBase*>* dests = cREVERSE ? edges_dest.GetIterable() : edges_src.GetIterable();

    tPublishCache pc;

    pc.lock_estimate = 2 + dests->Size();  // 2 to make things safe with respect to listeners
    pc.set_locks = 0;  // this port
    pc.cur_ref = data->GetCurReference();
    pc.cur_ref_counter = pc.cur_ref->GetRefCounter();
    pc.cur_ref_counter->SetOrAddLocks(static_cast<int8>(pc.lock_estimate));
    assert((pc.cur_ref->IsLocked()));
    Assign(pc);

    // inform listeners?
    if (cINFORM_LISTENERS)
    {
      NotifyListeners(&(pc));
    }

    // later optimization (?) - unroll loops for common short cases
    for (size_t i = 0u; i < dests->Size(); i++)
    {
      tPortBase* dest = dests->Get(i);
      bool push = (dest != NULL) && dest->WantsPush<cREVERSE, cCHANGE_CONSTANT>(cREVERSE, cCHANGE_CONSTANT);
      if (push)
      {
        dest->Receive<cREVERSE, cCHANGE_CONSTANT>(pc, this, cREVERSE, cCHANGE_CONSTANT);
      }
    }

    // release any locks that were acquired too much
    pc.ReleaseObsoleteLocks();
  }

  /*!
   * Pull/read current value from source port.
   * When multiple source ports are available an arbitrary one of them is used.
   * (Returned value in publish cache has one lock for caller)
   *
   * \param pc Publish Cache
   * \param intermediate_assign Assign pulled value to ports in between?
   * \param first Call on first/originating port?
   */
  const void PullValueRawImpl(tPublishCache& pc, bool intermediate_assign, bool first);

  /*!
   * Update statistics if this is enabled
   *
   * \param tc Initialized ThreadLocalCache
   */
  inline void UpdateStatistics(tPublishCache& pc, tPortBase* source, tPortBase* target)
  {
    if (tRuntimeSettings::cCOLLECT_EDGE_STATISTICS)    // const, so method can be optimized away completely
    {
      UpdateEdgeStatistics(source, target, pc.cur_ref->GetData());
    }
  }

protected:

  void AddLock(tPublishCache& pc);

  /*!
   * Publishes new data to port.
   * Releases and unlocks old data.
   * Lock on new data has to be set before
   *
   * \param pdr New Data
   */
  inline void Assign(tPublishCache& pc)
  {
    AddLock(pc);
    tPortDataReference* old = value.GetAndSet(pc.cur_ref);
    old->GetRefCounter()->ReleaseLock();
    if (!standard_assign)
    {
      NonStandardAssign(pc);
    }
  }

  virtual void ClearQueueImpl()
  {
    queue->Clear(true);
  }

  virtual int GetMaxQueueLengthImpl() const
  {
    return queue->GetMaxLength();
  }

  /*!
   * \return Is SPECIAL_REUSE_QUEUE flag set (see PortFlags)?
   */
  inline bool HasSpecialReuseQueue() const
  {
    return (this->const_flags & tPortFlags::cSPECIAL_REUSE_QUEUE) > 0;
  }

  // quite similar to publish
  virtual void InitialPushTo(tAbstractPort* target, bool reverse);

  inline const tPortData* LockCurrentValueForRead() const
  {
    return LockCurrentValueForRead(static_cast<int8>(1))->GetData();
  }

  /*!
   * Lock current buffer for safe read access.
   *
   * \param add_locks number of locks to add
   * \return Locked buffer (caller will have to take care of unlocking) - (clean c++ pointer)
   */
  inline tPortDataReference* LockCurrentValueForRead(int8 add_locks) const
  {
    // AtomicInteger source code style
    for (; ;)
    {
      tPortDataReference* cur_value = value.Get();

      if (cur_value->GetRefCounter()->TryLocks(add_locks))
      {
        // successful
        return cur_value;
      }
    }
  }

  /*!
   * Custom special assignment to port.
   * Used, for instance, in queued ports.
   *
   * \param pdr New Data
   */
  virtual void NonStandardAssign(tPublishCache& pc);

  virtual void PrintStructure(int indent, rrlib::logging::tLogStream& output);

  /*!
   * Publish data
   *
   * \param data Data to publish
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void Publish(const tPortData* data, bool reverse, int8 changed_constant)
  {
    if (!reverse)
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<false, cCHANGED, false>(data);
      }
      else
      {
        PublishImpl<false, cCHANGED_INITIAL, false>(data);
      }
    }
    else
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<true, cCHANGED, false>(data);
      }
      else
      {
        PublishImpl<true, cCHANGED_INITIAL, false>(data);
      }
    }

  }

  /*!
   * Pull/read current value from source port
   * When multiple source ports are available an arbitrary one of them is used.
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   * \return Locked port data
   */
  inline const tPortData* PullValueRaw()
  {
    return PullValueRaw(true);
  }

  /*!
   * Pull/read current value from source port
   * When multiple source ports are available an arbitrary one of them is used.
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   * \return Locked port data
   */
  const tPortData* PullValueRaw(bool intermediate_assign);

  template <bool cREVERSE, int8 cCHANGE_CONSTANT>
  /*!
   * \param pc Publish cache readily set up
   * \param origin Port that value was received from
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void Receive(tPublishCache& pc, tPortBase* origin, bool reverse, int8 changed_constant)
  {
    Assign(pc);
    SetChanged(cCHANGE_CONSTANT);
    NotifyListeners(&(pc));
    UpdateStatistics(pc, origin, this);

    if (!cREVERSE)
    {
      // forward
      util::tArrayWrapper<tPortBase*>* dests = edges_src.GetIterable();
      for (int i = 0, n = dests->Size(); i < n; i++)
      {
        tPortBase* dest = dests->Get(i);
        bool push = (dest != NULL) && dest->WantsPush<false, cCHANGE_CONSTANT>(false, cCHANGE_CONSTANT);
        if (push)
        {
          dest->Receive<false, cCHANGE_CONSTANT>(pc, this, false, cCHANGE_CONSTANT);
        }
      }

      // reverse
      dests = edges_dest.GetIterable();
      for (int i = 0, n = dests->Size(); i < n; i++)
      {
        tPortBase* dest = dests->Get(i);
        bool push = (dest != NULL) && dest->WantsPush<true, cCHANGE_CONSTANT>(false, cCHANGE_CONSTANT);
        if (push && dest != origin)
        {
          dest->Receive<true, cCHANGE_CONSTANT>(pc, this, true, cCHANGE_CONSTANT);
        }
      }
    }
  }

  virtual void SetMaxQueueLengthImpl(int length);

public:

  /*!
   * \param pci PortCreationInformation
   */
  tPortBase(tPortCreationInfo pci);

  /*!
   * \param listener Listener to add
   */
  inline void AddPortListenerRaw(tPortListener<>* listener)
  {
    port_listener.Add(listener);
  }

  /*!
   * Publish buffer through port
   * (not in normal operation, but from browser; difference: listeners on this port will be notified)
   *
   * \param buffer Buffer with data (must be owned by current thread)
   */
  void BrowserPublish(const tPortData* data);

  /*!
   * \return Does port contain default value?
   */
  inline bool ContainsDefaultValue()
  {
    return value.Get()->GetData() == default_value;
  }

  virtual ~tPortBase();

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  void DequeueAllRaw(tPortQueueFragment<tPortData>& fragment);

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
  tPortData* DequeueSingleAutoLockedRaw();

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
  tPortData* DequeueSingleUnsafeRaw();

  virtual void ForwardData(tAbstractPort* other);

  /*!
   * \return current auto-locked Port data (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const tPortData* GetAutoLockedRaw()
  {
    const tPortData* pd = GetLockedUnsafeRaw();
    tThreadLocalCache::Get()->AddAutoLock(pd);
    return pd;
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline tPortData* GetDefaultBufferRaw()
  {
    assert(((!IsReady())) && "please set default value _before_ initializing port");
    return default_value;
  }

  /*!
   * (careful: typically not meant for use by clients (not type-safe, no auto-release of locks))
   *
   * \return current locked port data
   */
  inline const tPortData* GetLockedUnsafeRaw()
  {
    if (PushStrategy())
    {
      return LockCurrentValueForRead();
    }
    else
    {
      return PullValueRaw(true);
    }
  }

  /*!
   * Pulls port data (regardless of strategy)
   * (careful: no auto-release of lock)
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  virtual const tPortData* GetPullLockedUnsafe(bool intermediate_assign)
  {
    return PullValueRaw(intermediate_assign);
  }

  virtual tPortData* GetUnusedBuffer(tDataType* dt)
  {
    assert((multi_buffer_pool != NULL));
    return multi_buffer_pool->GetUnusedBuffer(dt);
  }

  /*!
   * \return Unused buffer from send buffers for writing.
   * (Using this method, typically no new buffers/objects need to be allocated)
   */
  inline tPortData* GetUnusedBufferRaw()
  {
    return buffer_pool == NULL ? multi_buffer_pool->GetUnusedBuffer(cur_data_type) : buffer_pool->GetUnusedBuffer();
  }

  virtual void NotifyDisconnect();

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * It should not be modified thereafter.
   * Should only be called on output ports
   *
   * \param cnc Data buffer acquired from a port using getUnusedBuffer (or locked data received from another port)
   */
  inline void Publish(const tPortData* data)
  {
    PublishImpl<false, cCHANGED, false>(data);
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListenerRaw(tPortListener<>* listener)
  {
    port_listener.Remove(listener);
  }

  /*!
   * \param pull_request_handler Object that handles pull requests - null if there is none (typical case)
   */
  void SetPullRequestHandler(tPullRequestHandler* pull_request_handler_);

  /*!
   * Does port (still) have this value?
   * (calling this is only safe, when pd is locked)
   *
   * \param pd Port value
   * \return Answer
   */
  inline bool ValueIs(const tPortData* pd) const
  {
    return value.Get()->GetData() == pd;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTBASE_H
