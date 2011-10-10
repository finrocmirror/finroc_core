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

#ifndef core__port__tAbstractPort_h__
#define core__port__tAbstractPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/tRuntimeSettings.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/port/tPortFlags.h"
#include "core/port/tPortCreationInfoBase.h"
#include "core/tFrameworkElement.h"
#include "rrlib/serialization/tFactory.h"

namespace rrlib
{
namespace serialization
{
class tGenericObject;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tLinkEdge;
class tNetPort;
class tPortDataManager;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for all ports (and port sets)
 *
 * Convention: Protected Methods do not perform any necessary synchronization
 * concerning calling threads (that they are called only once at the same time)
 * This has to be done by all public methods.
 *
 * Methods are all thread-safe. Most setting methods are synchronized on runtime.
 * Constant methods may return outdated results when element is concurrently changed.
 * In many cases this (non-blocking) behaviour is intended.
 * However, to avoid that, synchronize to runtime before calling.
 */
class tAbstractPort : public tFrameworkElement, public rrlib::serialization::tFactory
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
  static const uint cBULK_N_EXPRESS = tPortFlags::cIS_BULK_PORT | tPortFlags::cIS_EXPRESS_PORT;

  /*! Bitvector indicating which of the first 16 outgoing edges was finstructed. Further info is stored in annotation if (ever) needed. */
  uint16_t outgoing_edges_finstructed;

protected:

  /*! Type of port data */
  const rrlib::serialization::tDataTypeBase data_type;

  /*! Minimum network update interval. Value < 0 means default for this type */
  int16 min_net_update_time;

public:

  /*! Timeout for pull operations */
  static const int cPULL_TIMEOUT = 1000;

  /*! constants for port change status */
  static const int8 cNO_CHANGE = 0, cCHANGED = 1, cCHANGED_INITIAL = 2;

  /*! Log domain for initial pushing */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(initial_push_log, "initial_pushes");

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "ports");

private:

  /*!
   * Should be called in situations where there might need to be an initial push
   * (e.g. connecting or strategy change)
   *
   * \param target Potential Target port
   */
  void ConsiderInitialReversePush(tAbstractPort* target);

  /*!
   * Forward current strategy to source ports (helper for above - and possibly variations of above)
   *
   * \param strategy2 New Strategy of this port
   * \param push_wanter Port that "wants" an initial push and from whom this call originates - null if there's no port that wants as push
   */
  void ForwardStrategy(int16 strategy2, tAbstractPort* push_wanter);

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
  static uint ProcessFlags(const tPortCreationInfoBase& pci);

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
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   */
  virtual void ConnectionRemoved(tAbstractPort* partner)
  {
  }

  /*!
   * \return Maximum queue length
   */
  virtual int GetMaxQueueLengthImpl() const = 0;

  /*!
   * \return Returns minimum strategy requirement (for this port in isolation) - typically 0 for non-input-ports
   * (Called in runtime-registry synchronized context only)
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
   * Called whenever a new connection to this port was established
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
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
   * \param finstructed Was edge created using finstruct? (Should never be called with true by application developer)
   */
  virtual void RawConnectToTarget(tAbstractPort* target, bool finstructed);

  /*!
   * Sets special change flag for initial push data
   */
  inline void SetChangedInitial()
  {
    changed = cCHANGED_INITIAL;
  }

  /*!
   * Set maximum queue length
   * (only implementation - does not set flags or propagate strategy)
   * (Called in runtime-registry synchronized context only)
   *
   * \param length Maximum queue length (values <= 1 mean that there is no queue)
   */
  virtual void SetMaxQueueLengthImpl(int length) = 0;

  /*!
   * Update edge statistics
   *
   * \param source Source port
   * \param target Target port
   * \param data Data that was sent
   */
  void UpdateEdgeStatistics(tAbstractPort* source, tAbstractPort* target, rrlib::serialization::tGenericObject* data);

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
  inline bool WantsPush(bool reverse, int8 change_constant) const
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
  tAbstractPort(tPortCreationInfoBase pci);

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
   * \param finstructed Was edge created using finstruct? (Should never be called with true by application developer)
   */
  void ConnectToSource(const util::tString& src_link, bool finstructed = false);

  /*!
   * Connect port to specified source port
   *
   * \param src_port_parent Parent of source port
   * \param src_port_name Name of source port
   * \param warn_if_not_available Print warning message if connection cannot be established
   */
  void ConnectToSource(tFrameworkElement* src_port_parent, util::tString& src_port_name, bool warn_if_not_available = true);

  /*!
   * Connect port to specified target port
   *
   * \param target Target port
   * \param finstructed Was edge created using finstruct? (Should never be called with true by application developer)
   */
  void ConnectToTarget(tAbstractPort* target, bool finstructed = false);

  /*!
   * Connect port to specified target port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of target port (relative to parent framework element)
   * \param finstructed Was edge created using finstruct? (Should never be called with true by application developer)
   */
  void ConnectToTarget(const util::tString& dest_link, bool finstructed = false);

  /*!
   * Connect port to specified destination port
   *
   * \param dest_port_parent Parent of destination port
   * \param dest_port_name Name of destination port
   * \param warn_if_not_available Print warning message if connection cannot be established
   */
  void ConnectToTarget(tFrameworkElement* dest_port_parent, util::tString& dest_port_name, bool warn_if_not_available = true);

  virtual std::shared_ptr<void> CreateBuffer(rrlib::serialization::tDataTypeBase dt)
  {
    return std::shared_ptr<void>(dt.CreateInstance());
  }

  virtual rrlib::serialization::tGenericObject* CreateGenericObject(rrlib::serialization::tDataTypeBase dt, void* factory_parameter);

  /*!
   * disconnects all edges
   */
  inline void DisconnectAll()
  {
    DisconnectAll(true, true);
  }

  /*!
   * disconnects all edges
   *
   * \param incoming disconnect incoming edges?
   * \param outgoing disconnect outgoing edges?
   */
  void DisconnectAll(bool incoming, bool outgoing);

  void DisconnectFrom(tAbstractPort* target);

  /*!
   * Disconnect from port with specified link (removes link edges
   *
   * \param link Qualified link of connection partner
   */
  void DisconnectFrom(const util::tString& link);

  /*!
   * Find network port connected to this port that belongs to specified framework element
   *
   * \param belongs_to Instance (usually TCPServerConnection or RemoteServer) that this port belongs to
   * \return Network port if it could be found - otherwise null
   */
  tNetPort* FindNetPort(util::tObject* belongs_to) const;

  /*!
   * Publish current data in the specified other port
   * (in a safe way)
   *
   * \param destination other port
   */
  virtual void ForwardData(tAbstractPort* other) = 0;

  /*!
   * \return Changed "flag" (has two different values for ordinary and initial data)
   */
  inline int8 GetChanged() const
  {
    return changed;
  }

  /*!
   * \return Number of connections to this port (incoming and outgoing)
   */
  inline int GetConnectionCount() const
  {
    return edges_dest->CountElements() + edges_src->CountElements();
  }

  /*!
   * Get all ports that this port is connected to
   *
   * \param result List to write results to
   * \param outgoing_edges Consider outgoing edges
   * \param incoming_edges Consider incoming edges
   * \param finstructed_edges_only Consider only outgoing finstructed edges?
   */
  void GetConnectionPartners(util::tSimpleList<tAbstractPort*>& result, bool outgoing_edges, bool incoming_edges, bool finstructed_edges_only = false);

  /*!
   * \return Type of port data
   */
  inline const rrlib::serialization::tDataTypeBase GetDataType() const
  {
    return data_type;
  }

  /*!
   * \return Number of connections to this port (incoming and outgoing)
   */
  inline int GetIncomingConnectionCount() const
  {
    return edges_dest->CountElements();
  }

  /*!
   * \return List with all link edges (must not be modified)
   */
  inline util::tSimpleList<tLinkEdge*>* GetLinkEdges()
  {
    return link_edges;
  }

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
   * \return Number of connections to this port (incoming and outgoing)
   */
  inline int GetOutgoingConnectionCount() const
  {
    return edges_src->CountElements();
  }

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
  virtual tPortDataManager* GetUnusedBufferRaw(rrlib::serialization::tDataTypeBase dt)
  {
    throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
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

  /*!
   * \return Does port have incoming edges?
   */
  inline bool HasIncomingEdges() const
  {
    return !edges_dest->IsEmpty();
  }

  /*!
   * \return Does port have any link edges?
   */
  inline bool HasLinkEdges()
  {
    return link_edges != NULL && link_edges->Size() > 0;
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
  bool IsConnectedToReversePushSources() const;

  /*!
   * \param target Index of target port
   * \return Is edge to specified target port finstructed?
   */
  bool IsEdgeFinstructed(int idx);

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
   * \return Is this port volatile (meaning that it's not always there and connections to it should preferably be links)?
   */
  inline bool IsVolatile()
  {
    return GetFlag(tPortFlags::cIS_VOLATILE);
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
   * (may be overridden by subclass - should usually call superclass method, too)
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
   * Serializes target handles of all outgoing connection destinations to stream
   * [byte: number of connections][int handle 1][bool finstructed 1]...[int handle n][bool finstructed n]
   *
   * \param co Output Stream
   */
  void SerializeOutgoingConnections(rrlib::serialization::tOutputStream& co);

  /*!
   * (relevant for input ports only)
   *
   * Sets change flag
   */
  inline void SetChanged()
  {
    changed = cCHANGED;
  }

  /*!
   * Sets change flag
   *
   * \param change_constant Constant to set changed flag to
   */
  inline void SetChanged(int8 change_constant)
  {
    changed = change_constant;
  }

  /*!
   * Mark edge with specified index as finstructed
   *
   * \param idx Index of edge
   * \param value True if edge was finstructed, false if edge was not finstructed (or when it is possibly deleted)
   */
  void SetEdgeFinstructed(int idx, bool value);

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
  void SetPushStrategy(bool push);

  /*!
   * Set whether data should be pushed or pulled in reverse direction
   *
   * \param push Push data?
   */
  void SetReversePushStrategy(bool push);
};

} // namespace finroc
} // namespace core

#endif // core__port__tAbstractPort_h__
