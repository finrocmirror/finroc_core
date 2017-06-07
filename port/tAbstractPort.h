//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
#include "core/port/tConnector.h"
#include "core/internal/tLocalUriConnector.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
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
class tAbstractPort : public tUriConnector::tOwner
{
  // connection list types
  typedef rrlib::concurrent_containers::tSet < tConnector*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<2, 9, definitions::cSINGLE_THREADED>, true > tOutgoingConnectionSet;
  typedef rrlib::concurrent_containers::tSet < tConnector*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<1, 9, definitions::cSINGLE_THREADED>, true > tIncomingConnectionSet;

  // iterator type
  typedef tOutgoingConnectionSet::tConstIterator tOutgoingConnectionIterator;
  typedef tIncomingConnectionSet::tConstIterator tIncomingConnectionIterator;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tAbstractPort(const tAbstractPortCreationInfo& creation_info);

  /*!
   * Connects ports with the two specified paths.
   * The connection is established when ports with these paths are available.
   *
   * \param port1_path Path to first port to connect
   * \param port2_path Path to second port to connect
   * \param connect_options Any connect options to apply
   * \throw Throws std::invalid_argument on invalid connect options
   */
  static void Connect(const tPath& port1_path, const tPath& port2_path, const tConnectOptions& connect_options = tConnectOptions());

  /*!
   * Connect port to specified partner port
   *
   * \param to Port to connect this port to
   * \param connect_options Any connect options to apply
   * \return Pointer to connector object if connecting succeeded. nullptr otherwise.
   * \throw Throws std::invalid_argument on invalid connect options
   */
  tConnector* ConnectTo(tAbstractPort& to, const tConnectOptions& connect_options = tConnectOptions());

  /*!
   * Connect port to partner port with specified URI.
   * The connection is established when a port with the URI is available.
   *
   * \param uri Absolute URI of partner port
   * \param connect_options Any connect options to apply
   * \throw Throws std::invalid_argument on invalid connect options
   */
  void ConnectTo(const tURI& uri, const tUriConnectOptions& connect_options = tUriConnectOptions());

  /*!
   * Connect port to specified partner port.
   * The connection is established when a port with the specified name/link is available.
   *
   * \param partner_port_parent Parent of port to connect to
   * \param partner_port_path Path to port to connect to (relative to partner_port_parent)
   * \param warn_if_not_available Print warning message if connection cannot be established
   * \param connect_options Any connect options to apply
   * \throw Throws std::invalid_argument on invalid connect options
   */
  void ConnectTo(tFrameworkElement& partner_port_parent, const tPath& partner_port_path, bool warn_if_not_available = true, const tConnectOptions& connect_options = tConnectOptions());

  /*!
   * \return Number of incoming connections (slightly expensive operation - O(n))
   */
  size_t CountIncomingConnections() const;

  /*!
   * \return Number of incoming connections (slightly expensive operation - O(n))
   */
  size_t CountOutgoingConnections() const;

  /*!
   * Disconnects ports with the two specified paths and no longer tries to connect them
   * (reverse operation to Connect())
   *
   * \param port1_path Path of first port to disconnect
   * \param port2_path Path of second port to disconnect
   * \return True if ports were disconnected or pending reconnect was stopped.
   */
  static bool Disconnect(const tPath& port1_path, const tPath& port2_path);

  /*!
   * Disconnects all connections (except of connections from tools).
   * Also stops any reconnecting to this port.
   *
   * \param incoming Disconnect incoming connections?
   * \param outgoing Disconnect outgoing connections?
   */
  void DisconnectAll(bool incoming = true, bool outgoing = true);

  /*!
   * Disconnect from specified port
   *
   * \param port Port to disconnect from
   * \return True if ports were disconnected or pending reconnect was stopped.
   */
  bool DisconnectFrom(tAbstractPort& port);

  /*!
   * Disconnect from port with specified URI
   *
   * \param uri Absolute URI of connection partner
   * \return True if URI connector was removed
   */
  bool DisconnectFrom(const tURI& uri);

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
   * \return An iterator to iterate over all incoming connections
   *
   * Typically used in this way (port is a tAbstractPort reference):
   *
   *   for (auto it = port.IncomingConnectionsBegin(); it != port.IncomingConnectionsEnd(); ++it)
   *   {
   *     if (it->Source().IsReady())
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
   * \param port Other port
   * \return Is port connected to specified other port?
   */
  bool IsConnectedTo(tAbstractPort& port) const;

  /*!
   * Check whether this (output) port has a data sink
   *
   * \return Is \a this port currently connected to an input port?
   */
  inline bool IsConnectedToInputPort() const
  {
    for (auto it = this->OutgoingConnectionsBegin(); it != this->OutgoingConnectionsEnd(); ++it)
    {
      if (it->Destination().IsInputPort() || it->Destination().IsConnectedToInputPort())
      {
        return true;
      }
    }
    return false;
  }

  /*!
   * Check whether this (input) port has a data sink
   *
   * \return Is \a this port currently connected to an output port?
   */
  inline bool IsConnectedToOutputPort() const
  {
    for (auto it = this->IncomingConnectionsBegin(); it != this->IncomingConnectionsEnd(); ++it)
    {
      if (it->Source().IsOutputPort() || it->Source().IsConnectedToOutputPort())
      {
        return true;
      }
    }
    return false;
  }

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
   * Checks whether this port can be connected to specified destination port.
   *
   * \param destination Destination port to check connectability to.
   * \param connect_options Any connect options to apply
   * \param reason_string If connecting is not possible, appends reason to this stream (unless it is nullptr)
   * \return Answer
   */
  bool MayConnectTo(tAbstractPort& destination, const tConnectOptions& connect_options = tConnectOptions(), std::stringstream* reason_stream = nullptr) const;

  /*!
   * Notify port of possibly temporary network connection loss.
   * If port's DEFAULT_ON_DISCONNECT flag is set, the port value is set to its default (typically a safe state).
   * This method is typically called by network transport plugins (only).
   */
  void NotifyOfNetworkConnectionLoss()
  {
    OnNetworkConnectionLoss();
  }

  /*!
   * \return An iterator to iterate over outgoing connections
   *
   * Typically used in this way (port is a tAbstractPort reference):
   *
   *   for (auto it = port.OutgoingConnectionsBegin(); it != port.OutgoingConnectionsEnd(); ++it)
   *   {
   *     if (it->Destination().IsReady())
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

  /*! Result of InferConnectDirection */
  enum tConnectDirection
  {
    TO_DESTINATION,
    TO_SOURCE
  };

  /*!
   * Create connector for specified connection
   * (may be overridden to create specialized connectors)
   *
   * \param destination Destination port to connect to
   * \param connect_options Connect options to apply
   * \return Reference to created connector
   */
  virtual tConnector* CreateConnector(tAbstractPort& destination, const tConnectOptions& connect_options);

  /*!
   * Infers connect direction to specified partner port
   * (must be called with structure mutex lock)
   *
   * \param other Port to determine connect direction to.
   * \return Either TO_DESTINATION or TO_SOURCE depending on whether 'other' should be destination or source of a connection with this port.
   */
  virtual tConnectDirection InferConnectDirection(const tAbstractPort& other) const;

  /*!
   * Infers connect direction to partner port with specified path.
   * Partner port does not need to exist (if it does, the other overload should be called, as it needs less heuristics).
   * (must be called with structure mutex lock)
   *
   * \param path Path of port to determine connect direction to.
   * \return Either TO_DESTINATION or TO_SOURCE depending on whether 'other' should be destination or source of a connection with this port.
   */
  virtual tConnectDirection InferConnectDirection(const tPath& path) const;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tPortConnectionConstraint;
  friend class tRuntimeEnvironment;
  friend class tConnector;
  friend class tUriConnector;
  friend class internal::tLocalUriConnector;

  /*! Edges emerging from this port */
  tOutgoingConnectionSet outgoing_connections;

  /*! Edges ending at this port */
  tIncomingConnectionSet incoming_connections;

  /*!
   * Data type of class wrapping this port (differs from data_type e.g. with numeric types)
   * If port was not created using a wrapper class, this is empty.
   */
  rrlib::rtti::tType wrapper_data_type;

  /*! Data type of port */
  const rrlib::rtti::tType data_type;


  /*!
   * Connect port to specified destination port - called after all tests succeeded
   * (updates internal variables etc.)
   *
   * \param destination Destination port to connect to
   * \param connect_options Connect options to apply
   * \return Reference to created connector
   */
  tConnector& ConnectImplementation(tAbstractPort& destination, const tConnectOptions& connect_options);

  /*!
   * Implementation of actual removement of connector (updates internal variables etc.)
   *
   * \param connector Connector to disconnect and remove
   * \param stop_any_reconnecting Stop any reconnecting of connection? (Usually done if disconnect is explicitly called - usually not done when port is deleted)
   */
  static void DisconnectImplementation(tConnector& connector, bool stop_any_reconnecting);

  /*!
   * \return Global list of constraints regarding connections among ports
   * \throws May throw an exception during static destruction phase
   */
  static std::vector<tPortConnectionConstraint*>& GetConnectionConstraintList();

  /*!
   * Called whenever a new connection to or from this port is established.
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   * \param partner_is_destination Is partner port destination port? (otherwise it's the source port)
   */
  virtual void OnConnect(tAbstractPort& partner, bool partner_is_destination)
  {
  }

  /*!
   * Called whenever a connection to or from this port is removed or lost.
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   * \param partner_is_destination Is partner port destination port? (otherwise it's the source port)
   */
  virtual void OnDisconnect(tAbstractPort& partner, bool partner_is_destination)
  {
  }

  /*!
   * Called whenever port is notified of possibly temporary network connection loss.
   * (meant to be overridden by subclasses)
   * (see NotifyOfNetworkConnectionLoss())
   */
  virtual void OnNetworkConnectionLoss()
  {
  }

  virtual void OnManagedDelete() override;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
