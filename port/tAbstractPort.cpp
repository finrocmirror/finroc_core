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
/*!\file    core/port/tAbstractPort.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/port/tAbstractPort.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/string.h"
#include "rrlib/rtti_conversion/tStaticCastOperation.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tLockOrderLevel.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPortConnectionConstraint.h"
#include "core/internal/tLocalUriConnector.h"
#include "core/internal/tGarbageDeleter.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

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

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tAbstractPort::tAbstractPort(const tAbstractPortCreationInfo& info) :
  tOwner(info.parent, info.name, info.flags | tFlag::PORT),
  outgoing_connections(),
  incoming_connections(),
  wrapper_data_type(),
  data_type(info.data_type)
{
}

tAbstractPort::~tAbstractPort()
{
}

void tAbstractPort::Connect(const tPath& port1_path, const tPath& port2_path, const tConnectOptions& connect_options)
{
  if (port1_path.Size() == 0 || port2_path.Size() == 0)
  {
    FINROC_LOG_PRINT_STATIC_TO(edges, ERROR, "Empty port link provided. Not connecting anything.");
    return;
  }

  rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  internal::tLocalUriConnector::Create(port1_path, port2_path, connect_options, tRuntimeEnvironment::GetInstance());
}

tConnector& tAbstractPort::ConnectImplementation(tAbstractPort& destination, const tConnectOptions& connect_options)
{
  tEdgeAggregator::EdgeAdded(*this, destination);

  tConnector* connector = CreateConnector(destination, connect_options);
  if (!connector)
  {
    throw std::logic_error("Failed to create connection");
  }

  this->outgoing_connections.Add(connector);
  destination.incoming_connections.Add(connector);

  auto& runtime = GetRuntime();
  for (auto it = runtime.runtime_listeners.Begin(); it != runtime.runtime_listeners.End(); ++it)
  {
    (*it)->OnConnectorChange(tRuntimeListener::tEvent::ADD, *connector);
  }

  return *connector;
}

tConnector* tAbstractPort::ConnectTo(tAbstractPort& to, const tConnectOptions& connect_options)
{
  rrlib::thread::tLock lock(GetStructureMutex());

  // Basic checks
  internal::CheckConnectionFlags(connect_options.flags);
  if (IsDeleted() || to.IsDeleted())
  {
    FINROC_LOG_PRINT(WARNING, "Port already deleted!");
    return nullptr;
  }
  if (&to == this)
  {
    FINROC_LOG_PRINT(WARNING, "Cannot connect port to itself.");
    return nullptr;
  }
  if (IsConnectedTo(to)) // already connected?
  {
    return nullptr;
  }

  // determine connect direction
  tConnectDirection connect_direction = tConnectDirection::TO_DESTINATION;
  tConnectionFlags connect_direction_flags = connect_options.flags & (tConnectionFlag::DIRECTION_TO_DESTINATION | tConnectionFlag::DIRECTION_TO_SOURCE);
  if (connect_direction_flags.Raw() == 0) // auto mode?
  {
    bool to_target_possible = MayConnectTo(to, connect_options);
    bool to_source_possible = to.MayConnectTo(*this, connect_options);
    if (to_target_possible && to_source_possible)
    {
      connect_direction = InferConnectDirection(to);
    }
    else if (to_target_possible || to_source_possible)
    {
      connect_direction = to_target_possible ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
    }
    else
    {
      std::stringstream reason_stream;
      reason_stream << "Could not connect ports '" << (*this) << "' and '" << to << "' for the following reason: ";
      MayConnectTo(to, connect_options, &reason_stream);
      reason_stream << "\nConnecting in the reverse direction did not work either: ";
      to.MayConnectTo(*this, connect_options, &reason_stream);
      FINROC_LOG_PRINT(WARNING, reason_stream.str());
      return nullptr;
    }
  }
  else
  {
    connect_direction = connect_options.flags.Get(tConnectionFlag::DIRECTION_TO_DESTINATION) ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
  }

  // connect
  tAbstractPort& source = (connect_direction == tConnectDirection::TO_DESTINATION) ? *this : to;
  tAbstractPort& destination = (connect_direction == tConnectDirection::TO_DESTINATION) ? to : *this;
  std::stringstream reason_stream;
  if (source.MayConnectTo(destination, connect_options, &reason_stream))
  {
    FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Creating Edge from ", source, " to ", destination);
    bool connection_to_reconnect = (!connect_options.flags.Get(tConnectionFlag::NON_PRIMARY_CONNECTOR)) &&
                                   (connect_options.flags.Get(tConnectionFlag::RECONNECT) || (source.GetFlag(tFlag::VOLATILE) || destination.GetFlag(tFlag::VOLATILE)));
    tConnectOptions non_primary_options = connect_options;
    non_primary_options.flags |= tConnectionFlag::NON_PRIMARY_CONNECTOR;
    tConnector& connector = source.ConnectImplementation(destination, connection_to_reconnect ? non_primary_options : connect_options);
    source.OnConnect(destination, true);
    destination.OnConnect(source, false);

    // Connection to reconnect?
    if (connection_to_reconnect)
    {
      tConnectOptions options = connect_options;
      options.flags |= tConnectionFlag::RECONNECT;
      if (source.GetFlag(tFlag::VOLATILE) && destination.GetFlag(tFlag::VOLATILE))
      {
        options.flags |= tConnectionFlag::DIRECTION_TO_DESTINATION;
        internal::tLocalUriConnector::Create(source.GetPath(), destination.GetPath(), options, tRuntimeEnvironment::GetInstance(), &connector);
      }
      else if (source.GetFlag(tFlag::VOLATILE))
      {
        options.flags |= tConnectionFlag::DIRECTION_TO_SOURCE;
        internal::tLocalUriConnector::Create(destination, source.GetPath(), options, destination, &connector);
      }
      else if (destination.GetFlag(tFlag::VOLATILE))
      {
        options.flags |= tConnectionFlag::DIRECTION_TO_DESTINATION;
        internal::tLocalUriConnector::Create(source, destination.GetPath(), options, source, &connector);
      }
      else
      {
        options.flags |= tConnectionFlag::DIRECTION_TO_DESTINATION;
        internal::tLocalUriConnector::Create(source.GetPath(), destination.GetPath(), options, tRuntimeEnvironment::GetInstance(), &connector);
      }
    }
    return &connector;
  }
  else
  {
    FINROC_LOG_PRINT(WARNING, "Could not connect ports '", (*this), "' and '", to, "' for the following reason:\n- ", reason_stream.str());
  }
  return nullptr;
}

void tAbstractPort::ConnectTo(const tURI& uri, const tUriConnectOptions& connect_options)
{
  if (uri.ToString().length() == 0)
  {
    FINROC_LOG_PRINT_TO(edges, ERROR, "No URI specified for partner port. Not connecting anything.");
    return;
  }

  rrlib::thread::tLock lock2(GetStructureMutex());
  if (IsDeleted())
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Port already deleted!");
    return;
  }
  tUriConnector::Create(*this, uri, connect_options);
}

void tAbstractPort::ConnectTo(tFrameworkElement& partner_port_parent, const tPath& partner_port_path, bool warn_if_not_available, const tConnectOptions& connect_options)
{
  rrlib::thread::tLock lock2(GetStructureMutex());
  tFrameworkElement* p = partner_port_parent.GetChild(partner_port_path);
  if (p && p->IsPort())
  {
    ConnectTo(*static_cast<tAbstractPort*>(p), connect_options);
    return;
  }
  else if (warn_if_not_available)
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot find port '", partner_port_path, "' in ", partner_port_parent, ". Creating connection if it becomes available later.");
  }
  tPath path;
  partner_port_parent.GetPath(path);
  ConnectTo(tURI(path.Append(partner_port_path)), connect_options);
}

size_t tAbstractPort::CountIncomingConnections() const
{
  size_t result = 0;
  for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
  {
    result++;
  }
  return result;
}

size_t tAbstractPort::CountOutgoingConnections() const
{
  size_t result = 0;
  for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
  {
    result++;
  }
  return result;
}

tConnector* tAbstractPort::CreateConnector(tAbstractPort& destination, const tConnectOptions& connect_options)
{
  return new tConnector(*this, destination, connect_options);
}

bool tAbstractPort::Disconnect(const tPath& port1_path, const tPath& port2_path)
{
  rrlib::thread::tLock lock2(GetRuntime().GetStructureMutex());

  tRuntimeEnvironment& runtime = GetRuntime();
  core::tAbstractPort* port1 = runtime.GetPort(port1_path);
  if (port1 && port1->DisconnectFrom(port2_path))
  {
    return true;
  }
  core::tAbstractPort* port2 = runtime.GetPort(port2_path);
  if (port2 && port2->DisconnectFrom(port1_path))
  {
    return true;
  }

  auto& connectors_map = runtime.registered_connectors;
  auto connector_list = connectors_map.find(port1_path);
  if (connector_list != connectors_map.end())
  {
    for (internal::tLocalUriConnector * connector : connector_list->second)
    {
      if ((connector->GetPortReferences()[0].path == port1_path && connector->GetPortReferences()[1].path == port2_path) || (connector->GetPortReferences()[1].path == port1_path && connector->GetPortReferences()[0].path == port2_path))
      {
        connector->Disconnect();
        return true;
      }
    }
  }
  return false;
}

void tAbstractPort::DisconnectAll(bool incoming, bool outgoing)
{
  if ((!incoming) && (!outgoing))
  {
    return;
  }

  rrlib::thread::tLock lock(GetStructureMutex());

  if (outgoing)
  {
    for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
    {
      if (!it->Destination().GetFlag(tFlag::TOOL_PORT))
      {
        DisconnectImplementation(*it, true);
      }
    }
  }

  if (incoming)
  {
    for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
    {
      if (!it->Source().GetFlag(tFlag::TOOL_PORT))
      {
        DisconnectImplementation(*it, true);
      }
    }
  }

  // remove high-level connectors managed by port
  if (UriConnectors().size())
  {
    if (incoming && outgoing)
    {
      ClearUriConnectors();
    }
    else
    {
      size_t size = UriConnectors().size();
      tUriConnector* list_copy[size]; // important: a copy is made here, since vector may be changed by tHighLevelConnector::Disconnect()
      for (size_t i = 0; i < size; i++)
      {
        list_copy[i] = UriConnectors()[i].get();
      }
      for (size_t i = 0; i < size; i++)
      {
        if (list_copy[i])
        {
          bool outgoing_connector = list_copy[i]->IsOutgoingConnector(*this);
          if ((outgoing_connector && outgoing) || ((!outgoing_connector) && incoming))
          {
            list_copy[i]->Disconnect();
          }
        }
      }
    }
  }

  assert(((!incoming) || (!outgoing) || (UriConnectors().size() == 0)));
}

bool tAbstractPort::DisconnectFrom(tAbstractPort& port)
{
  rrlib::thread::tLock lock(GetStructureMutex());
  for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
  {
    if (&(it->Destination()) == &port)
    {
      DisconnectImplementation(*it, true);
      return true;
    }
  }

  for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
  {
    if (&(it->Source()) == &port)
    {
      DisconnectImplementation(*it, true);
      return true;
    }
  }
  return false;
}

bool tAbstractPort::DisconnectFrom(const tURI& uri)
{
  rrlib::thread::tLock lock(GetStructureMutex());

  for (auto & uri_connector : UriConnectors())
  {
    if (uri_connector && uri_connector->Uri() == uri)
    {
      uri_connector->Disconnect();
      return true;
    }
  }
  return false;
}

void tAbstractPort::DisconnectImplementation(tConnector& connector, bool stop_any_reconnecting)
{
  tEdgeAggregator::EdgeRemoved(connector.Source(), connector.Destination());

  connector.Destination().incoming_connections.Remove(&connector);
  connector.Source().outgoing_connections.Remove(&connector);

  connector.Destination().OnDisconnect(connector.Source(), false);
  connector.Source().OnDisconnect(connector.Destination(), true);

  auto& runtime = GetRuntime();
  for (auto it = runtime.runtime_listeners.Begin(); it != runtime.runtime_listeners.End(); ++it)
  {
    (*it)->OnConnectorChange(tRuntimeListener::tEvent::REMOVE, connector);
  }

  connector.OnDisconnect(stop_any_reconnecting);
  internal::tGarbageDeleter::DeleteDeferred(&connector);
}

std::vector<tPortConnectionConstraint*>& tAbstractPort::GetConnectionConstraintList()
{
  typedef rrlib::design_patterns::tSingletonHolder<std::vector<tPortConnectionConstraint*>> tConstraintListSingleton;
  return tConstraintListSingleton::Instance();
}

tAbstractPort::tConnectDirection tAbstractPort::InferConnectDirection(const tAbstractPort& other) const
{
  // If one port is no proxy port (only emits or accepts data), direction is clear
  if (!(GetFlag(tFlag::EMITS_DATA) && GetFlag(tFlag::ACCEPTS_DATA))) // not a proxy port?
  {
    return GetFlag(tFlag::EMITS_DATA) ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
  }
  if (!(other.GetFlag(tFlag::EMITS_DATA) && other.GetFlag(tFlag::ACCEPTS_DATA))) // not a proxy port?
  {
    return other.GetFlag(tFlag::ACCEPTS_DATA) ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
  }

  // Temporary variable to store result: Return tConnectDirection::TO_TARGET?
  bool to_destination = true;

  // Do we have input or output proxy ports?
  bool this_output_proxy = this->IsOutputPort();
  bool other_output_proxy = other.IsOutputPort();

  // Do ports belong to the same module or group?
  tEdgeAggregator* this_aggregator = tEdgeAggregator::GetAggregator(*this);
  tEdgeAggregator* other_aggregator = tEdgeAggregator::GetAggregator(other);
  bool ports_have_same_parent = this_aggregator && other_aggregator &&
                                ((this_aggregator == other_aggregator) || (this_aggregator->GetParent() == other_aggregator->GetParent() && this_aggregator->GetFlag(tFlag::INTERFACE) && other_aggregator->GetFlag(tFlag::INTERFACE)));

  // Ports of network interfaces typically determine connection direction
  if (this->GetFlag(tFlag::NETWORK_ELEMENT))
  {
    to_destination = this_output_proxy;
  }
  else if (other.GetFlag(tFlag::NETWORK_ELEMENT))
  {
    to_destination = !other_output_proxy;
  }
  else if (this_output_proxy != other_output_proxy)
  {
    // If we have an output and an input port, typically, the output port is connected to the input port
    to_destination = this_output_proxy;

    // If ports are in interfaces of the same group/module, connect in the reverse of the typical direction
    if (ports_have_same_parent)
    {
      to_destination = !to_destination;
    }
  }
  else
  {
    // count parents
    int this_parent_node_count = 1;
    tFrameworkElement* parent = this->GetParent();
    while ((parent = parent->GetParent()))
    {
      this_parent_node_count++;
    }

    int other_parent_node_count = 1;
    parent = other.GetParent();
    while ((parent = parent->GetParent()))
    {
      other_parent_node_count++;
    }

    // Are ports forwarded to outer interfaces?
    if (this_parent_node_count != other_parent_node_count)
    {
      to_destination = (this_output_proxy && other_parent_node_count < this_parent_node_count) ||
                       ((!this_output_proxy) && this_parent_node_count < other_parent_node_count);
    }
    else
    {
      FINROC_LOG_PRINT(WARNING, "Two proxy ports ('", (*this), "' and '", other, "') in the same direction and on the same level are to be connected. Cannot infer direction. Guessing TO_DESTINATION.");
    }
  }

  return to_destination ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
}

tAbstractPort::tConnectDirection tAbstractPort::InferConnectDirection(const tPath& path) const
{
  if (!(GetFlag(tFlag::EMITS_DATA) && GetFlag(tFlag::ACCEPTS_DATA))) // not a proxy port?
  {
    return GetFlag(tFlag::EMITS_DATA) ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
  }

  bool to_destination = true;

  // Ports of network interfaces typically determine connection direction
  if (this->GetFlag(tFlag::NETWORK_ELEMENT))
  {
    to_destination = this->IsOutputPort();
  }
  else
  {
    tFrameworkElement* component = (GetParent() && GetParent()->GetFlag(tFlag::INTERFACE)) ? GetParent()->GetParent() : nullptr;
    if (component)
    {
      rrlib::uri::tPath component_path;
      component->GetPath(component_path);
      bool inner_connection = path.CountCommonElements(component_path) == component_path.Size();
      to_destination = (inner_connection && this->IsInputPort()) || ((!inner_connection) && this->IsOutputPort());
    }
    else
    {
      FINROC_LOG_PRINT(WARNING, "This proxy port is not part of a interface. So far, no heuristics for this case have been implemented. Guessing connect direction to be its primary connect direction.");
      to_destination = this->IsOutputPort();
    }
  }
  return to_destination ? tConnectDirection::TO_DESTINATION : tConnectDirection::TO_SOURCE;
}

bool tAbstractPort::IsConnectedTo(tAbstractPort& port) const
{
  for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
  {
    if (&(it->Destination()) == &port)
    {
      return true;
    }
  }
  for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
  {
    if (&(it->Source()) == &port)
    {
      return true;
    }
  }
  return false;
}

bool tAbstractPort::MayConnectTo(tAbstractPort& destination, const tConnectOptions& connect_options, std::stringstream* reason_stream) const
{
  if (connect_options.conversion_operations.Size() == 0 && (!rrlib::rtti::conversion::tStaticCastOperation::IsImplicitlyConvertibleTo(data_type, destination.data_type)))
  {
    if (reason_stream)
    {
      (*reason_stream) << "Data types require explicit conversion (source: '" << GetDataType().GetName() << "' and target: '" << destination.GetDataType().GetName() << "').";
    }
    return false;
  }
  // TODO: should conversion operation be checked here already? (con: simpler if not and it will be checked anyway when creating the connector)

  if (GetFlag(tFlag::TOOL_PORT) || destination.GetFlag(tFlag::TOOL_PORT))
  {
    return true; // ignore flags and constraints for ports from tools
  }

  if (!GetFlag(tFlag::EMITS_DATA))
  {
    if (reason_stream)
    {
      (*reason_stream) << "Port '" << (*this) << "' does not emit data.";
    }
    return false;
  }

  if (!destination.GetFlag(tFlag::ACCEPTS_DATA))
  {
    if (reason_stream)
    {
      (*reason_stream) << "Port '" << destination << "' does not accept data.";
    }
    return false;
  }

  auto& constraints = GetConnectionConstraintList();
  for (auto it = constraints.begin(); it != constraints.end(); ++it)
  {
    if (!(*it)->AllowPortConnection(*this, destination))
    {
      if (reason_stream)
      {
        (*reason_stream) << "The following constraint disallows connection: '" << (*it)->Description() << "'";
      }
      return false;
    }
  }

  return true;
}

void tAbstractPort::OnManagedDelete()
{
  rrlib::thread::tLock lock1(GetStructureMutex());

  // Disconnect without stopping reconnecting with any newly created port
  for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
  {
    DisconnectImplementation(*it, false);
  }
  for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
  {
    DisconnectImplementation(*it, false);
  }
  ClearUriConnectors();
}

void tAbstractPort::SetWrapperDataType(const rrlib::rtti::tType& wrapper_data_type)
{
  if (this->wrapper_data_type && wrapper_data_type != this->wrapper_data_type)
  {
    FINROC_LOG_PRINT(ERROR, "Wrapper data type should not be set twice. Ignoring");
    return;
  }
  this->wrapper_data_type = wrapper_data_type;
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
