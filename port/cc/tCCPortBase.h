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

#ifndef core__port__cc__tCCPortBase_h__
#define core__port__cc__tCCPortBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortQueue.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/tFrameworkElement.h"
#include "rrlib/serialization/tGenericObjectManager.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/tRuntimeSettings.h"

namespace rrlib
{
namespace serialization
{
class tDataTypeBase;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tCCPortDataRef;
class tCCPullRequestHandler;
class tUnit;
class tCCQueueFragmentRaw;

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
  friend class tPort;

protected:
  /*implements Callable<PullCall>*/

  /*! Edges emerging from this port */
  tAbstractPort::tEdgeList<tCCPortBase*> edges_src;

  /*! Edges ending at this port */
  tAbstractPort::tEdgeList<tCCPortBase*> edges_dest;

  /*! default value - invariant: must never be null if used (must always be copied, too) */
  tCCPortDataManager* default_value;

  /*!
   * current value (set by main thread) - invariant: must never be null - sinnvoll(?)
   * In C++, other lock information is stored in in the last 3 bit - therefore
   * setValueInternal() and getValueInternal() should be used in the common cases.
   */
  tCCPortDataRef* volatile value;

  /*!
   * Data that is currently owned - used to belong to a terminated thread
   */
  tCCPortDataManagerTL* owned_data;

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
  tCCPortQueue* queue;

  /*! Listens to port value changes - may be null */
  tPortListenerManager port_listener;

  /*! Object that handles pull requests - null if there is none (typical case) */
  tCCPullRequestHandler* pull_request_handler;

  /*! Unit of port (currently only used for numeric ports) */
  tUnit* unit;

private:

  // helper for direct member initialization in C++
  inline static tCCPortDataManager* CreateDefaultValue(const rrlib::serialization::tDataTypeBase& dt)
  {
    return static_cast<tCCPortDataManager*>(dt.CreateInstanceGeneric<tCCPortDataManager>()->GetManager());
  }

  inline void NotifyListeners(tThreadLocalCache* tc)
  {
    port_listener.Notify(this, tc->data);
  }

  template <bool cREVERSE, int8 cCHANGE_CONSTANT, bool cINFORM_LISTENERS>
  /*!
   * Publish data
   *
   * \param tc ThreadLocalCache
   * \param data Data to publish
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   * \param inform_listeners Inform this port's listeners on change? (usually only when value comes from browser)
   */
  inline void PublishImpl(tThreadLocalCache* tc, tCCPortDataManagerTL* data, bool reverse = false, int8 changed_constant = cCHANGED, bool inform_listeners = false)
  {
    assert((data->GetObject()->GetType() != NULL) && "Port data type not initialized");
    assert((IsInitialized() || cINFORM_LISTENERS) && "Port not initialized");

    util::tArrayWrapper<tCCPortBase*>* dests = cREVERSE ? edges_dest.GetIterable() : edges_src.GetIterable();

    // assign
    tc->data = data;
    tc->ref = data->GetCurrentRef();
    Assign(tc);

    // inform listeners?
    if (cINFORM_LISTENERS)
    {
      NotifyListeners(tc);
    }

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
      UpdateEdgeStatistics(source, target, tc->data->GetObject());
    }
  }

protected:

  virtual void ClearQueueImpl()
  {
    queue->Clear(true);
  }

  /*!
   * Get current data in container owned by this thread with a lock.
   * Attention: User needs to take care of unlocking.
   *
   * \return Container (non-const - public wrapper should return it const)
   */
  tCCPortDataManagerTL* GetLockedUnsafeInContainer();

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
  inline tCCPortDataManagerTL* GetUnusedBuffer(tThreadLocalCache* tc)
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
  inline void Publish(tThreadLocalCache* tc, tCCPortDataManagerTL* data)
  {
    PublishImpl<false, cCHANGED, false>(tc, data);
  }

  /*!
   * Publish data
   *
   * \param tc ThreadLocalCache
   * \param data Data to publish
   * \param reverse Value received in reverse direction?
   * \param changed_constant changedConstant to use
   */
  inline void Publish(tThreadLocalCache* tc, tCCPortDataManagerTL* data, bool reverse, int8 changed_constant)
  {
    if (!reverse)
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<false, cCHANGED, false>(tc, data);
      }
      else
      {
        PublishImpl<false, cCHANGED_INITIAL, false>(tc, data);
      }
    }
    else
    {
      if (changed_constant == cCHANGED)
      {
        PublishImpl<true, cCHANGED, false>(tc, data);
      }
      else
      {
        PublishImpl<true, cCHANGED_INITIAL, false>(tc, data);
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
  inline tCCPortDataManagerTL* PullValueRaw()
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
  tCCPortDataManagerTL* PullValueRaw(bool intermediate_assign);

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
    tCCPortDataManagerTL* old_data = tc->data;
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
  inline void AddPortListenerRaw(tPortListenerRaw* listener)
  {
    port_listener.Add(listener);
  }

  /*!
   * Set current value to default value
   */
  void ApplyDefaultValue();

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
    tCCPortDataManagerTL* pdc = tc->last_written_to_port[port_index];
    if (pdc != NULL)
    {
      pdc->ReleaseLock();
    }
    tc->last_written_to_port[port_index] = tc->data;
    value = tc->ref;
  }

  /*!
   * Publish buffer through port
   * (not in normal operation, but from browser; difference: listeners on this port will be notified)
   *
   * \param buffer Buffer with data (must be owned by current thread)
   */
  void BrowserPublishRaw(tCCPortDataManagerTL* buffer);

  /*!
   * \return Does port contain default value?
   */
  bool ContainsDefaultValue();

  virtual ~tCCPortBase();

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  void DequeueAllRaw(tCCQueueFragmentRaw& fragment);

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
  inline rrlib::serialization::tGenericObject* DequeueSingleAutoLockedRaw()
  {
    tCCPortDataManager* result = DequeueSingleUnsafeRaw();
    if (result == NULL)
    {
      return NULL;
    }
    tThreadLocalCache::Get()->AddAutoLock(result);
    return result->GetObject();
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
  tCCPortDataManager* DequeueSingleUnsafeRaw();

  virtual void ForwardData(tAbstractPort* other);

  /*!
   * \return Current data with auto-lock (can only be unlocked with ThreadLocalCache auto-unlock)
   */
  inline const rrlib::serialization::tGenericObject* GetAutoLockedRaw()
  {
    tThreadLocalCache* tc = tThreadLocalCache::Get();

    if (PushStrategy())
    {
      tCCPortDataManagerTL* mgr = GetLockedUnsafeInContainer();
      tc->AddAutoLock(mgr);
      return mgr->GetObject();

    }
    else
    {
      tCCPortDataManager* mgr = GetInInterThreadContainer();
      tc->AddAutoLock(mgr);
      return mgr->GetObject();
    }
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline rrlib::serialization::tGenericObject* GetDefaultBufferRaw()
  {
    assert(((!IsReady())) && "please set default value _before_ initializing port");
    return default_value->GetObject();
  }

  /*!
   * \return Current data in CC Interthread-container. Needs to be recycled manually.
   */
  tCCPortDataManager* GetInInterThreadContainer();

  /*!
   * Pulls port data (regardless of strategy) and returns it in interhread container
   * (careful: no auto-release of lock)
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  tCCPortDataManager* GetPullInInterthreadContainerRaw(bool intermediate_assign);

  /*!
   * Copy current value to buffer (Most efficient get()-version)
   *
   * \param buffer Buffer to copy current data
   */
  inline void GetRaw(rrlib::serialization::tGenericObjectManager* buffer)
  {
    GetRaw(buffer->GetObject());
  }

  /*!
   * Copy current value to buffer (Most efficient get()-version)
   *
   * \param buffer Buffer to copy current data to
   */
  void GetRaw(rrlib::serialization::tGenericObject* buffer);

  /*!
   * \return Unit of port
   */
  inline tUnit* GetUnit()
  {
    return unit;
  }

  virtual void NotifyDisconnect();

  /*!
   * Publish buffer through port
   *
   * \param read_object Buffer with data (must be owned by current thread)
   */
  inline void Publish(tCCPortDataManagerTL* buffer)
  {
    assert((buffer->GetOwnerThread() == util::sThreadUtil::GetCurrentThreadId()));
    Publish(tThreadLocalCache::GetFast(), buffer);
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListenerRaw(tPortListenerRaw* listener)
  {
    port_listener.Remove(listener);
  }

  /*!
   * \param pull_request_handler Object that handles pull requests - null if there is none (typical case)
   */
  void SetPullRequestHandler(tCCPullRequestHandler* pull_request_handler_);

  /*!
   * Transfers data ownership to port after a thread has been deleted
   * (Needs to be called with lock on ThreadLocalCache::infos)
   *
   * \param port_data_container port data container to transfer ownership of
   */
  void TransferDataOwnership(tCCPortDataManagerTL* port_data_container);

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortBase_h__
