//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    core/port/tAbstractPort.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tAbstractPort
 *
 * \b tAbstractPort
 *
 * This is the abstract base class for all ports.
 * Edges can be used to connect two ports.
 * Ports can be connected n:m (however, typically only 1:n makes sense inside robotic applications)
 * Additional constraints on port connections can be imposed by instantiating customized
 * tPortConnectionConstraint objects.
 */
//----------------------------------------------------------------------
#ifndef __core__port__tAbstractPort_h__
#define __core__port__tAbstractPort_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"
#include "core/port/tAbstractPortCreationInfo.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace runtime_construction
{
class tFinstructableGroup;
}

namespace core
{
namespace internal
{
class tLinkEdge;
}

class tPortConnectionConstraint;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Base class of all Finroc ports
/*!
 * This is the abstract base class for all ports.
 * Edges can be used to connect two ports.
 * Ports can be connected n:m (however, typically only 1:n makes sense inside robotic applications)
 * Additional constraints on port connections can be imposed by instantiating customized
 * tPortConnectionConstraint objects.
 *
 * Convention: Private and protected methods do not perform any necessary synchronization
 * regarding concurrent calls.
 * Public methods are all thread-safe.
 * Constant methods may return outdated results when element is concurrently changed.
 * In many cases this (non-blocking) behaviour is intended.
 * However, to avoid that, acquire runtime structure lock before calling.
 */
class tAbstractPort : public tFrameworkElement
{
  // connection list types
  typedef rrlib::concurrent_containers::tSet < tAbstractPort*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<2, 9, definitions::cSINGLE_THREADED>, true > tOutgoingConnectionSet;
  typedef rrlib::concurrent_containers::tSet < tAbstractPort*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<1, 9, definitions::cSINGLE_THREADED>, true > tIncomingConnectionSet;

  // iterator type
  typedef tOutgoingConnectionSet::tConstIterator tOutgoingConnectionIterator;
  typedef tIncomingConnectionSet::tConstIterator tIncomingConnectionIterator;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Connection direction
   */
  enum class tConnectDirection
  {
    AUTO,      //!< Automatically determine connection direction. Usually a good choice
    TO_TARGET, //!< Specified port is target port
    TO_SOURCE  //!< Specified port is source port
  };

  tAbstractPort(const tAbstractPortCreationInfo& creation_info);

  /*!
   * Connects ports with the two specified links.
   * The connection is (re)established whenever ports with these links are available.
   *
   * \param port1_link Link name of first port to connect
   * \param port2_link Link name of second port to connect
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   *                          (TO_TARGET means that the second port is the target port)
   */
  static void Connect(const std::string& port1_link, const std::string& port2_link, tConnectDirection connect_direction = tConnectDirection::AUTO);

  /*!
   * Connect port to specified partner port
   *
   * \param to Port to connect this port to
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   * \param finstructed Was edge created using finstruct (or loaded from XML file)? (Should never be called with true by application developer)
   */
  void ConnectTo(tAbstractPort& to, tConnectDirection connect_direction = tConnectDirection::AUTO, bool finstructed = false);

  /*!
   * Connect port to specified partner port
   * The connection is (re)established whenever a port with this link is available.
   *
   * \param link_name Link name of partner port (relative to parent framework element)
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   * \param finstructed Was edge created using finstruct (or loaded from XML file)? (Should never be called with true by application developer)
   */
  void ConnectTo(const tString& link_name, tConnectDirection connect_direction = tConnectDirection::AUTO, bool finstructed = false);

  /*!
   * Connect port to specified partner port
   *
   * \param partner_port_parent Parent of port to connect to
   * \param partner_port_name Name of port to connect to
   * \param warn_if_not_available Print warning message if connection cannot be established
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  void ConnectTo(tFrameworkElement& partner_port_parent, const tString& port_name, bool warn_if_not_available = true, tConnectDirection connect_direction = tConnectDirection::AUTO);

  /*!
   * \return Number of incoming connections (slightly expensive operation - O(n))
   */
  size_t CountIncomingConnections() const;

  /*!
   * \return Number of incoming connections (slightly expensive operation - O(n))
   */
  size_t CountOutgoingConnections() const;

  /*!
   * Disconnects all edges
   *
   * \param incoming disconnect incoming edges?
   * \param outgoing disconnect outgoing edges?
   */
  void DisconnectAll(bool incoming = true, bool outgoing = true);

  /*!
   * Disconnect from specified port
   *
   * \param target Port to disconnect from
   */
  void DisconnectFrom(tAbstractPort& target);

  /*!
   * Disconnect from port with specified link (removes link edges
   *
   * \param link Qualified link of connection partner
   */
  void DisconnectFrom(const tString& link);

  /*!
   * \return Data type of port
   */
  inline const rrlib::rtti::tType GetDataType() const
  {
    return data_type;
  }

  /*!
   * \return Data type of class wrapping this port (differs from data_type e.g. with numeric types)
   * If port was not created using a wrapper class, this is empty.
   */
  rrlib::rtti::tType GetWrapperDataType() const
  {
    return wrapper_data_type;
  }

  /*!
   * \return Was edge from this port to specified destination created using finstruct?
   */
  bool IsEdgeFinstructed(tAbstractPort& destination) const;

  /*!
   * \return Does port have any incoming connections?
   */
  bool HasIncomingConnections()
  {
    return !incoming_connections.Empty();
  }

  /*!
   * \return Does port have any outgoing connections?
   */
  bool HasOutgoingConnections()
  {
    return !outgoing_connections.Empty();
  }

  /*!
   * \return An iterator to iterate over all ports that this port has incoming connections from
   *
   * Typically used in this way (port is a tAbstractPort reference):
   *
   *   for (auto it = port.OutgoingConnectionsBegin(); it != port.OutgoingConnectionsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  tIncomingConnectionIterator IncomingConnectionsBegin() const
  {
    return incoming_connections.Begin();
  }

  /*!
   * \return An iterator to iterate over all ports that this port has incoming connections from - pointing to the past-the-end element.
   */
  tIncomingConnectionIterator IncomingConnectionsEnd() const
  {
    return incoming_connections.End();
  }

  /*!
   * (slightly expensive)
   * \return Is port currently connected?
   */
  inline bool IsConnected() const
  {
    return (!outgoing_connections.Empty()) || (!incoming_connections.Empty());
  }

  /*!
   * \return Is port connected to specified other port?
   */
  bool IsConnectedTo(tAbstractPort& target) const;

  /*!
   * \return Is this an input port?
   */
  inline bool IsInputPort() const
  {
    return !IsOutputPort();
  }

  /*!
   * \return Is this an output port?
   */
  inline bool IsOutputPort() const
  {
    return GetFlag(tFlag::OUTPUT_PORT);
  }

  void Link(tFrameworkElement& parent, const tString& link_name)
  {
    tFrameworkElement::Link(parent, link_name); // TODO: Remove this from tFrameworkElement?
  }

  /*!
   * Can this port be connected to specified target port? (additional non-standard checks)
   * (may be overridden by subclass - should usually call superclass method, too)
   *
   * \param target Target port?
   * \param reason_string If connecting is not possible, appends reason to this string (if not NULL)
   * \return Answer
   */
  bool MayConnectTo(tAbstractPort& target, std::string* reason_string = NULL) const;

  /*!
   * \return An iterator to iterate over all ports that this port has outgoing connections to
   *
   * Typically used in this way (port is a tAbstractPort reference):
   *
   *   for (auto it = port.OutgoingConnectionsBegin(); it != port.OutgoingConnectionsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  tOutgoingConnectionIterator OutgoingConnectionsBegin() const
  {
    return outgoing_connections.Begin();
  }

  /*!
   * \return An iterator to iterate over all ports that this port has outgoing connections to pointing to the past-the-end element.
   */
  tOutgoingConnectionIterator OutgoingConnectionsEnd() const
  {
    return outgoing_connections.End();
  }

  /*!
   * Only intended to be called by classes wrapping port classes derived from tAbstractPort.
   *
   * \param Data type of class wrapping this port (differs from data_type e.g. with numeric types)
   */
  void SetWrapperDataType(const rrlib::rtti::tType& wrapper_data_type);

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual ~tAbstractPort();

  /*!
   * Infers connect direction to specified partner port
   *
   * \param other Port to determine connect direction to.
   * \return Either TO_TARGET or TO_SOURCE depending on whether 'other' should be target or source of a connection with this port.
   */
  virtual tConnectDirection InferConnectDirection(const tAbstractPort& other) const;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tPortConnectionConstraint;
  friend class tRuntimeEnvironment;
  friend class runtime_construction::tFinstructableGroup; // for link edge access

  /*! Edges emerging from this port */
  tOutgoingConnectionSet outgoing_connections;

  /*! Edges ending at this port */
  tIncomingConnectionSet incoming_connections;

  /*! Contains any link edges created by this port */
  std::unique_ptr<std::vector<internal::tLinkEdge*>> link_edges;

  /*!
   * Data type of class wrapping this port (differs from data_type e.g. with numeric types)
   * If port was not created using a wrapper class, this is empty.
   */
  rrlib::rtti::tType wrapper_data_type;

  /*! Data type of port */
  const rrlib::rtti::tType data_type;


  /*!
   * Connect port to specified target port - called after all tests succeeded
   * (updates internal variables etc.)
   *
   * \param target Target to connect to
   * \param finstructed Was edge created using finstruct?
   */
  void ConnectImplementation(tAbstractPort& target, bool finstructed);

  /*!
   * Called whenever a new connection to or from this port is established
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   * \param partner_is_destination Is partner port destination port? (otherwise it's the source port)
   */
  virtual void ConnectionAdded(tAbstractPort& partner, bool partner_is_destination)
  {
  }

  /*!
   * Called whenever a new connection to or from this port is removed
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   * \param partner_is_destination Is partner port destination port? (otherwise it's the source port)
   */
  virtual void ConnectionRemoved(tAbstractPort& partner, bool partner_is_destination)
  {
  }

  /*!
   * Implementation of actual removement of edge (updates internal variables etc.)
   *
   * \param source Source Port
   * \param destination Destination Port
   */
  void DisconnectImplementation(tAbstractPort& source, tAbstractPort& destination);

  /*!
   * (may throw an exception during static destruction)
   * \return Global list of constraints regarding connections among ports
   */
  static std::vector<tPortConnectionConstraint*>& GetConnectionConstraintList();

  /*!
   * Transforms (possibly relative link) to absolute link
   *
   * \param rel_link possibly relative link (absolute if it starts with '/')
   * \return absolute link
   */
  tString MakeAbsoluteLink(const tString& rel_link) const;

  virtual void PrepareDelete(); // TODO: add "override" when we use gcc 4.7

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
