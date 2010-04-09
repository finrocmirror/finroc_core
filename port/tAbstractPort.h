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

#ifndef CORE__PORT__TABSTRACTPORT_H
#define CORE__PORT__TABSTRACTPORT_H

#include "core/portdatabase/tDataType.h"
#include "finroc_core_utils/container/tSimpleList.h"
#include "core/port/tPortFlags.h"
#include "core/port/tPortCreationInfo.h"
#include "core/tFrameworkElement.h"
#include "core/portdatabase/tTypedObjectImpl.h"

#include "finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
class tLinkEdge;
class tEdgeAggregator;
class tNetPort;
class tPortData;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for all ports (and port sets)
 *
 * Convention: Protected Methods do not perform any necessary synchronization
 * concerning calling threads (that they are called only once at the same time)
 * This has to be done by all public methods.
 */
class tAbstractPort : public tFrameworkElement
{
protected:

  /*!
   * List class for edges
   */
  template < typename T = tAbstractPort* >
  class tEdgeList : public util::tSafeConcurrentlyIterableList<T, tRuntimeSettings::cEDGE_LIST_SIZE_INCREASE_FACTOR>
  {
  public:
    tEdgeList() :
        util::tSafeConcurrentlyIterableList<T, tRuntimeSettings::cEDGE_LIST_SIZE_INCREASE_FACTOR>(tRuntimeSettings::cEDGE_LIST_DEFAULT_SIZE)
    {}

  };

private:

  /*! Has port changed since last reset? (see constants above) */
  volatile int8 changed;

  /*! Edges emerging from this port - raw lists seem the most reasonable approach here */
  tEdgeList<>* edges_src;

  /*! Edges ending at this port */
  tEdgeList<>* edges_dest;

  //  /** Contains names of any links to this port - for efficient destruction */
  //  protected @Ptr SimpleList<String> linksTo;

  /*! Contains any link edges created by this port */
  util::tSimpleList<tLinkEdge*>* link_edges;

  /*!
   * Strategy to use, when this port is target
   * -1:     not connected at all
   * 0:      pull strategy
   * n >= 1: push strategy for queue with n elements (queue length makes no difference locally, but network ports need to buffer this amount of elements)
   */
  int16 strategy;

  /*! Constant for bulk and express flag */
  static const int cBULK_N_EXPRESS = tPortFlags::cIS_BULK_PORT | tPortFlags::cIS_EXPRESS_PORT;

protected:

  /*! Type of port data */
  tDataType* const data_type;

  //  /** Counter for pull & method calls in this port */
  //  protected final AtomicInt callIndex = new AtomicInt(0);

  /*! Minimum network update interval. Value < 0 means default for this type */
  int16 min_net_update_time;

public:

  /*! Timeout for pull operations */
  static const int cPULL_TIMEOUT = 1000;

  /*! constants for port change status */
  static const int8 cNO_CHANGE = 0, cCHANGED = 1, cCHANGED_INITIAL = 2;

private:

  /*!
   * Should be called in situations where there might need to be an initial push
   * (e.g. connecting or strategy change)
   *
   * \param target Potential Target port
   */
  void ConsiderInitialReversePush(tAbstractPort* target);

  /*!
   * Transforms (possibly relative link) to absolute link
   *
   * \param rel_link possibly relative link (absolute if it starts with '/')
   * \return absolute link
   */
  util::tString MakeAbsoluteLink(const util::tString& rel_link);

  /*!
   * Make some auto-adjustments to flags at construction time
   *
   * \param pci Port creation info
   * \return processed flags
   */
  static int ProcessFlags(const tPortCreationInfo& pci);

  static void RemoveInternal(tAbstractPort* src, tAbstractPort* dest);

protected:

  /*!
   * Clear queue and unlock contents
   */
  virtual void ClearQueueImpl() = 0;

  /*!
   * Send information about changed Minimum Network Update Interval to clients.
   */
  void CommitUpdateTimeChange();

  /*!
   * Called whenever a connection to this port was removed
   * (meant to be overridden by subclasses)
   *
   * \param partner Port at other end of connection
   */
  virtual void ConnectionRemoved(tAbstractPort* partner)
  {
  }

  //      if (!acceptsReverseData()) {
  //        if (strategy > 0 && max <= 0) { // reset INITIAL_PUSH_RECEIVED flag, when we switch to a pull strategy (so that switch to push strategy will cause a push again)
  //          setFlag(PortFlags.INITIAL_PUSH_RECEIVED, false);
  //        } else if (strategy <= 0 && max > 0) { // we should consider an initial push
  //          if ((!hasIncomingEdges()) && considerPush) {
  //            for (int i = 0, n = it.size(); i < n; i++) {
  //              @Ptr AbstractPort port = it.get(i);
  //              if (port != null && port.pushStrategy()) {
  //                considerInitialPush(port);
  //              }
  //            }
  //          }
  //        }
  //      }

  /*!
   * Forward current strategy to source ports (helper for above - and possibly variations of above)
   *
   * \param strategy2 New Strategy of this port
   * \param push_wanter Port that "wants" an initial push and from whom this call originates - null if there's no port that wants as push
   */
  void ForwardStrategy(int16 strategy2, tAbstractPort* push_wanter);

  /*!
   * \return Maximum queue length
   */
  virtual int GetMaxQueueLengthImpl() const = 0;

  /*!
   * \return Returns minimum strategy requirement (for this port in isolation) - typically 0 for non-input-ports
   */
  virtual int16 GetStrategyRequirement() const;

  /*!
   * Push initial value to the specified port
   * (checks etc. have been done by AbstractPort class)
   *
   * \param target Port to push data to
   * \param reverse Is this a reverse push?
   */
  virtual void InitialPushTo(tAbstractPort* target, bool reverse) = 0;

  /*!
   * Can this port be connected to specified target port?
   * (additional non-standard checks by subclass)
   *
   * \param target Target port?
   * \return Answer
   */
  virtual bool MayConnectTo2(tAbstractPort* target)
  {
    return true;
  }

  /*!
   * Called whenever a new connection to this port was established
   * (meant to be overridden by subclasses)
   *
   * \param partner Port at other end of connection
   */
  virtual void NewConnection(tAbstractPort* partner)
  {
  }

  virtual ~tAbstractPort();

  virtual void PrepareDelete();

  /*!
   * Propagates max target queue length to sources
   * (call on target with new connections)
   *
   * \param push_wanter Port that "wants" an initial push and from whom this call originates - null if there's no port that wants as push
   * \param new_connection_partner When a new connection is created - The new port that is connected to this (target) port
   * \return Did Strategy for this port change?
   */
  virtual bool PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner);

  /*!
   * Connect port to specified target port - called after all tests
   * succeeded
   *
   * \param target Target to connect to
   */
  virtual void RawConnectToTarget(tAbstractPort* target);

  /*!
   * Sets special change flag for initial push data
   */
  inline void SetChangedInitial()
  {
    changed = cCHANGED_INITIAL;
    /*if (parent instanceof PortSet) {
      ((PortSet)parent).childChanged();
    }*/
  }

  /*!
   * Set maximum queue length
   * (only implementation - does not set flags or propagate strategy)
   *
   * \param length Maximum queue length (values <= 1 mean that there is no queue)
   */
  virtual void SetMaxQueueLengthImpl(int length) = 0;

  //  /**
  //   * \return Has port (ever) been linked?
  //   */
  //  @ConstMethod public boolean isLinked() {
  //    return linksTo != null;
  //  }
  //
  //  /**
  //   * Does link name link to port?
  //   *
  //   * \param linkName link name
  //   * \return Answer
  //   */
  //  @ConstMethod public boolean isLinked(@Const @Ref String linkName) {
  //    if (linksTo == null) {
  //      return false;
  //    }
  //    for (@SizeT int i = 0; i < linksTo.size(); i++) {
  //      if (linksTo.get(i).equals(linkName)) {
  //        return true;
  //      }
  //    }
  //    return false;
  //  }

  /*!
   * Update edge statistics
   *
   * \param source Source port
   * \param target Target port
   * \param data Data that was sent
   */
  void UpdateEdgeStatistics(tAbstractPort* source, tAbstractPort* target, tTypedObject* data);

  //  /**
  //   * \return Does port "want" to receive an initial push? Typically it does, unless it has multiple sources or no push strategy itself.
  //   * (Standard implementation for this)
  //   */
  //  @SuppressWarnings("unchecked")
  //  protected boolean wantsInitialPush() {
  //    int sources = 0;
  //    @Ptr ArrayWrapper<AbstractPort> src = edgesDest.getIterable();
  //    for (int i = 0, n = src.size(); i < n; i++) {
  //      @Ptr AbstractPort pb = src.get(i);
  //      if (pb != null && pb.isReady()) {
  //        sources++;
  //      }
  //    }
  //    return isReady() && strategy > 0 && sources <= 1;
  //  }

  template <bool cREVERSE, int8 cCHANGE_CONSTANT>
  /*!
   * Does this port "want" to receive a value via push strategy?
   *
   * \param cReverse direction? (typically we push forward)
   * \param change_constant If this is about an initial push, this should be CHANGED_INITIAL - otherwise CHANGED
   * \return Answer
   *
   * Typically it does, unless it has multiple sources or no push strategy itself.
   * (Standard implementation for this)
   */
  inline bool WantsPush(bool reverse, int8 change_constant)
  {
    // I think and hope that the compiler is intelligent enough to optimize branches away...
    if (cREVERSE)
    {
      if (cCHANGE_CONSTANT == cCHANGED_INITIAL)
      {
        return (this->flags & tPortFlags::cPUSH_STRATEGY_REVERSE) > 0 && edges_src->CountElements() <= 1;
      }
      else
      {
        return (this->flags & tPortFlags::cPUSH_STRATEGY_REVERSE) > 0;
      }
    }
    else if (cCHANGE_CONSTANT == cCHANGED_INITIAL)
    {
      // We don't want initial pushes to ports with multiple inputs
      return strategy > 0 && edges_dest->CountElements() <= 1;
    }
    else
    {
      return strategy > 0;
    }
  }

public:

  /*!
   * \param pci PortCreationInformation
   */
  tAbstractPort(tPortCreationInfo pci);

  //  /**
  //   * Is specified link, the first link to this port?
  //   *
  //   * \param link link name
  //   * \return Answer
  //   */
  //  @ConstMethod public boolean isFirstLink(@Const @Ref String link) {
  //    return linksTo.get(0).equals(link);
  //  }

  //  /**
  //   * Return link name of link number i
  //   *
  //   * \param i
  //   * \param buffer Buffer for result
  //   * \return link name
  //   */
  //  @ConstMethod public /*@Const @Ref*/ void getLink(int i, @Ref StringBuilder buffer) {
  //    getQualifiedLink(buffer, i);
  //  }

  /*!
   * \return Does port accept reverse data?
   */
  inline bool AcceptsReverseData() const
  {
    return GetFlag(tPortFlags::cMAY_ACCEPT_REVERSE_DATA);
  }

  /*!
   * \return Return Netport instance of this port - in case this is a net port - otherwise null
   */
  virtual tNetPort* AsNetPort();

  /*!
   * Connect port to specified source port
   *
   * \param source Source port
   */
  inline void ConnectToSource(tAbstractPort* source)
  {
    source->ConnectToTarget(this);
  }

  /*!
   * Connect port to specified source port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of source port (relative to parent framework element)
   */
  void ConnectToSource(const util::tString& src_link);

  /*!
   * Connect port to specified target port
   *
   * \param target Target port
   */
  void ConnectToTarget(tAbstractPort* target);

  /*!
   * Connect port to specified target port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of target port (relative to parent framework element)
   */
  void ConnectToTarget(const util::tString& dest_link);

  /*!
   * disconnects all edges
   */
  void DisconnectAll();

  void DisconnectFrom(tAbstractPort* target);

  /*!
   * Find network port connected to this port that belongs to specified framework element
   *
   * \param belongs_to Instance (usually TCPServerConnection or RemoteServer) that this port belongs to
   * \return Network port if it could be found - otherwise null
   */
  tNetPort* FindNetPort(util::tObject* belongs_to) const;

  /*!
   * \return Changed "flag" (has two different values for ordinary and initial data)
   */
  inline int8 GetChanged()
  {
    return changed;
  }

  /*!
   * \return Number of connections to this port (incoming and outgoing)
   */
  inline int GetConnectionCount()
  {
    return edges_dest->CountElements() + edges_src->CountElements();
  }

  /*!
   * \return Type of port data
   */
  inline tDataType* GetDataType() const
  {
    return data_type;
  }

  //  /**
  //   * \return Current data auto-locked - Universal & virtual method - call ThreadLocalCache.get().releaseAllLocks to release lock
  //   */
  //  @Virtual public abstract TypedObject universalGetAutoLocked();

  /*!
   * \return Minimum Network Update Interval (only-port specific one; -1 if there's no specific setting for port)
   */
  inline int16 GetMinNetUpdateInterval() const
  {
    return min_net_update_time;
  }

  /*!
   * (Helper function for network functions)
   * Look for minimal port-specific minimal network update interval
   * at all connected ports.
   *
   * \return result - -1 if no port has specific setting
   */
  int16 GetMinNetworkUpdateIntervalForSubscription() const;

  /*!
   * \return Strategy to use, when this port is target
   */
  inline int16 GetStrategy() const
  {
    assert((strategy >= -1));
    return strategy;
  }

  /*!
   * \param dt Data type
   * \return Unused buffer for writing - of specified data type - or null if unsupported by this port
   * (Using this method, typically no new buffers/objects need to be allocated)
   *
   * This method is only supported by a subset of ports that have a MultiTypePortDataBufferPool
   */
  virtual tPortData* GetUnusedBuffer(tDataType* dt)
  {
    throw util::tRuntimeException("Unsupported");
  }

  /*!
   * (relevant for input ports only)
   *
   * \return Has port changed since last reset?
   */
  inline bool HasChanged() const
  {
    return changed > cNO_CHANGE;
  }

  //  /**
  //   * \return Does port have edges to destinations with push strategy?
  //   */
  //  @SuppressWarnings("unchecked") @ConstMethod
  //  public boolean hasActiveEdges() {
  //    @Ptr ArrayWrapper<AbstractPort> it = edgesSrc.getIterable();
  //    for (int i = 0, n = it.size(); i < n; i++) {
  //      @Ptr AbstractPort port = it.get(i);
  //      if (port.getFlag(PortFlags.PUSH_STRATEGY)) {
  //        return true;
  //      }
  //    }
  //    return false;
  //  }

  //  /**
  //   * \return Does port have edges to sources with push strategy?
  //   */
  //  @SuppressWarnings("unchecked") @ConstMethod
  //  public boolean hasActiveEdgesReverse() {
  //    @Ptr ArrayWrapper<AbstractPort> it = edgesDest.getIterable();
  //    for (int i = 0, n = it.size(); i < n; i++) {
  //      @Ptr AbstractPort port = it.get(i);
  //      if (port != null && port.getFlag(PortFlags.PUSH_STRATEGY_REVERSE)) {
  //        return true;
  //      }
  //    }
  //    return false;
  //  }

  /*!
   * \return Does port have incoming edges?
   */
  inline bool HasIncomingEdges() const
  {
    return !edges_dest->IsEmpty();
  }

  /*!
   * \return Does port have outgoing edges?
   */
  inline bool HasOutgoingEdges() const
  {
    return !edges_src->IsEmpty();
  }

  /*!
   * (slightly expensive)
   * \return Is port currently connected?
   */
  inline bool IsConnected() const
  {
    return (!edges_src->IsEmpty()) || (!edges_dest->IsEmpty());
  }

  /*!
   * \return Is port connected to specified other port?
   */
  bool IsConnectedTo(tAbstractPort* target);

  /*!
   * \return Is port connected to output ports that request reverse pushes?
   */
  bool IsConnectedToReversePushSources();

  inline bool IsInputPort() const
  {
    return !IsOutputPort();
  }

  // flag queries

  inline bool IsOutputPort() const
  {
    return GetFlag(tPortFlags::cOUTPUT_PORT);
  }

  /*!
   * Create link to this port
   *
   * \param parent Parent framework element
   * \param link_name name of link
   */
  virtual void Link(tFrameworkElement* parent, const util::tString& link_name)
  {
    ::finroc::core::tFrameworkElement::Link(parent, link_name);
  }

  template <typename T>
  void InitLists(tEdgeList<T>* edges_src_, tEdgeList<T>* edges_dest_)
  {
    edges_src = reinterpret_cast<tEdgeList<>*>(edges_src_);
    edges_dest = reinterpret_cast<tEdgeList<>*>(edges_dest_);
  }

  /*!
   * Can this port be connected to specified target port? (additional non-standard checks)
   *
   * \param target Target port?
   * \return Answer
   */
  bool MayConnectTo(tAbstractPort* target);

  /*!
   * Notify port of (network) disconnect
   */
  virtual void NotifyDisconnect() = 0;

  /*!
   * Is data to this port pushed or pulled?
   *
   * \return Answer
   */
  inline bool PushStrategy() const
  {
    return GetStrategy() > 0;
  }

  /*!
   * (relevant for input ports only)
   *
   * Reset changed flag.
   */
  inline void ResetChanged()
  {
    changed = cNO_CHANGE;
  }

  /*!
   * Is data to this port pushed or pulled (in reverse direction)?
   *
   * \return Answer
   */
  inline bool ReversePushStrategy() const
  {
    return (this->flags & tPortFlags::cPUSH_STRATEGY_REVERSE) > 0;
  }

  /*!
   * (relevant for input ports only)
   *
   * Sets change flag
   */
  inline void SetChanged()
  {
    changed = cCHANGED;
    /*if (parent instanceof PortSet) {
      ((PortSet)parent).childChanged();
    }*/
  }

  /*!
   * Sets change flag
   *
   * \param change_constant Constant to set changed flag to
   */
  inline void SetChanged(int8 change_constant)
  {
    changed = change_constant;
    /*if (parent instanceof PortSet) {
      ((PortSet)parent).childChanged();
    }*/
  }

  /*!
   * Set maximum queue length
   *
   * \param length Maximum queue length (values <= 1 mean that there is no queue)
   */
  virtual void SetMaxQueueLength(int queue_length);

  /*!
   * \param interval2 Minimum Network Update Interval
   */
  void SetMinNetUpdateInterval(int interval2);

  /*!
   * Set whether data should be pushed or pulled
   *
   * \param push Push data?
   */
  inline void SetPushStrategy(bool push)
  {
    util::tLock lock2(obj_synch);
    SetFlag(tPortFlags::cPUSH_STRATEGY, push);
    PropagateStrategy(NULL, NULL);
  }

  /*!
   * Set whether data should be pushed or pulled in reverse direction
   *
   * \param push Push data?
   */
  void SetReversePushStrategy(bool push);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__TABSTRACTPORT_H
