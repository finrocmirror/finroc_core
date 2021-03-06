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

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tLockOrderLevel.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPortConnectionConstraint.h"
#include "core/internal/tLinkEdge.h"

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
namespace internal
{
/*!
 * Info on which edges were created using finstruct/XML
 */
class tFinstructedEdgeInfo : public tAnnotation
{
public:

  /*! Contains all outgoing edges that were finstructed */
  std::vector<tAbstractPort*> outgoing_edges_finstructed;

  tFinstructedEdgeInfo() :
    outgoing_edges_finstructed()
  {}
};

} // namespace internal

tAbstractPort::tAbstractPort(const tAbstractPortCreationInfo& info) :
  tFrameworkElement(info.parent, info.name, info.flags | tFlag::PORT),
  outgoing_connections(),
  incoming_connections(),
  link_edges(),
  wrapper_data_type(),
  data_type(info.data_type)
{
}

tAbstractPort::~tAbstractPort()
{
}

void tAbstractPort::Connect(const std::string& port1_link, const std::string& port2_link, tConnectDirection connect_direction)
{
  if (port1_link.length() == 0 || port2_link.length() == 0)
  {
    FINROC_LOG_PRINT_STATIC_TO(edges, ERROR, "No link name specified for partner port. Not connecting anything.");
    return;
  }

  rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  std::vector<std::unique_ptr<internal::tLinkEdge>>& global_link_edges = tRuntimeEnvironment::GetInstance().global_link_edges;

  for (auto it = global_link_edges.begin(); it != global_link_edges.end(); ++it)
  {
    if (((*it)->GetTargetLink().compare(port1_link) == 0 && (*it)->GetSourceLink().compare(port2_link) == 0) ||
        ((*it)->GetTargetLink().compare(port2_link) == 0 && (*it)->GetSourceLink().compare(port1_link) == 0))
    {
      return;
    }
  }
  switch (connect_direction)
  {
  case tConnectDirection::AUTO:
  case tConnectDirection::TO_TARGET:
    global_link_edges.emplace_back(new internal::tLinkEdge(port1_link, port2_link, connect_direction == tConnectDirection::AUTO));
    break;
  case tConnectDirection::TO_SOURCE:
    global_link_edges.emplace_back(new internal::tLinkEdge(port2_link, port1_link, false));
    break;
  }
}

void tAbstractPort::ConnectImplementation(tAbstractPort& target, bool finstructed)
{
  tEdgeAggregator::EdgeAdded(*this, target);

  this->outgoing_connections.Add(&target);
  target.incoming_connections.Add(this);
  if (finstructed)
  {
    internal::tFinstructedEdgeInfo* info = GetAnnotation<internal::tFinstructedEdgeInfo>();
    if (!info)
    {
      info = new internal::tFinstructedEdgeInfo();
      AddAnnotation<internal::tFinstructedEdgeInfo>(*info);
    }
    info->outgoing_edges_finstructed.push_back(&target);
  }

  PublishUpdatedEdgeInfo(tRuntimeListener::tEvent::ADD, target);
}

void tAbstractPort::ConnectTo(tAbstractPort& to, tConnectDirection connect_direction, bool finstructed)
{
  rrlib::thread::tLock lock(GetStructureMutex());
  if (IsDeleted() || to.IsDeleted())
  {
    FINROC_LOG_PRINT(WARNING, "Port already deleted!");
    return;
  }
  if (&to == this)
  {
    FINROC_LOG_PRINT(WARNING, "Cannot connect port to itself.");
    return;
  }
  if (IsConnectedTo(to)) // already connected?
  {
    return;
  }

  // determine connect direction
  if (connect_direction == tConnectDirection::AUTO)
  {
    bool to_target_possible = MayConnectTo(to);
    bool to_source_possible = to.MayConnectTo(*this);
    if (to_target_possible && to_source_possible)
    {
      connect_direction = InferConnectDirection(to);
    }
    else if (to_target_possible || to_source_possible)
    {
      connect_direction = to_target_possible ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
    }
    else
    {
      std::string reason_string = "Could not connect ports '" + GetQualifiedName() + "' and '" + to.GetQualifiedName() + "' for the following reason: ";
      MayConnectTo(to, &reason_string);
      reason_string += "\nConnecting in the reverse direction did not work either: ";
      to.MayConnectTo(*this, &reason_string);
      FINROC_LOG_PRINT(WARNING, reason_string);
      return;
    }
  }

  // connect
  tAbstractPort& source = (connect_direction == tConnectDirection::TO_TARGET) ? *this : to;
  tAbstractPort& target = (connect_direction == tConnectDirection::TO_TARGET) ? to : *this;
  std::string reason_string;
  if (source.MayConnectTo(target, &reason_string))
  {
    FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Creating Edge from ", source.GetQualifiedName(), " to ", target.GetQualifiedName());
    source.ConnectImplementation(target, finstructed);
    source.OnConnect(target, true);
    target.OnConnect(source, false);
  }
  else
  {
    FINROC_LOG_PRINT(WARNING, "Could not connect ports '" + GetQualifiedName() + "' and '" + to.GetQualifiedName() + "' for the following reason:\n- ", reason_string);
  }
}

void tAbstractPort::ConnectTo(const tString& link_name, tConnectDirection connect_direction, bool finstructed)
{
  if (link_name.length() == 0)
  {
    FINROC_LOG_PRINT_TO(edges, ERROR, "No link name specified for partner port. Not connecting anything.");
    return;
  }

  rrlib::thread::tLock lock2(GetStructureMutex());
  if (IsDeleted())
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Port already deleted!");
    return;
  }
  if (!link_edges)    // lazy initialization
  {
    link_edges.reset(new std::vector<internal::tLinkEdge*>());
  }
  for (auto it = link_edges->begin(); it != link_edges->end(); ++it)
  {
    if ((*it)->GetTargetLink() == link_name || (*it)->GetSourceLink() == link_name)
    {
      return;
    }
  }
  switch (connect_direction)
  {
  case tConnectDirection::AUTO:
  case tConnectDirection::TO_TARGET:
    link_edges->push_back(new internal::tLinkEdge(*this, MakeAbsoluteLink(link_name), connect_direction == tConnectDirection::AUTO, finstructed));
    break;
  case tConnectDirection::TO_SOURCE:
    link_edges->push_back(new internal::tLinkEdge(MakeAbsoluteLink(link_name), *this, false, finstructed));
    break;
  }
}

void tAbstractPort::ConnectTo(tFrameworkElement& partner_port_parent, const tString& port_name, bool warn_if_not_available, tConnectDirection connect_direction)
{
  tFrameworkElement* p = partner_port_parent.GetChildElement(port_name, false);
  if (p && p->IsPort())
  {
    ConnectTo(*static_cast<tAbstractPort*>(p), connect_direction);
  }
  else if (warn_if_not_available)
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot find port '", port_name, "' in ", partner_port_parent.GetQualifiedName(), ".");
  }
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

void tAbstractPort::DisconnectAll(bool incoming, bool outgoing)
{
  rrlib::thread::tLock lock(GetStructureMutex());

  // remove link edges
  if (link_edges != NULL)
  {
    for (size_t i = 0u; i < link_edges->size(); i++)
    {
      internal::tLinkEdge* le = (*link_edges)[i];
      if ((incoming && le->GetSourceLink().length() > 0) || (outgoing && le->GetTargetLink().length() > 0))
      {
        link_edges->erase(link_edges->begin() + i);
        delete le;
        i--;
      }
    }
  }
  assert(((!incoming) || (!outgoing) || (link_edges == NULL) || (link_edges->size() == 0)));

  if (outgoing)
  {
    for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
    {
      DisconnectImplementation(*this, *it);
    }
  }

  if (incoming)
  {
    for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
    {
      DisconnectImplementation(*it, *this);
    }
  }
}

void tAbstractPort::DisconnectFrom(tAbstractPort& target)
{
  bool found = false;
  {
    rrlib::thread::tLock lock(GetStructureMutex());
    for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
    {
      if (&(*it) == &target)
      {
        DisconnectImplementation(*this, target);
        found = true;
      }
    }

    for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
    {
      if (&(*it) == &target)
      {
        DisconnectImplementation(target, *this);
        found = true;
      }
    }
  }
  if (!found)
  {
    FINROC_LOG_PRINT(DEBUG_WARNING, "Edge to remove not found");
  }
  // not found: throw error message?
}

void tAbstractPort::DisconnectFrom(const tString& link)
{
  rrlib::thread::tLock lock(GetStructureMutex());
  if (link_edges)
  {
    for (size_t i = 0; i < link_edges->size(); i++)
    {
      internal::tLinkEdge& link_edge = *(*link_edges)[i];
      if (link_edge.GetSourceLink() == link || link_edge.GetTargetLink() == link)
      {
        delete &link_edge;
        link_edges->erase(link_edges->begin() + i);
        i--;
      }
    }
  }

  tAbstractPort* ap = GetRuntime().GetPort(link);
  if (ap)
  {
    DisconnectFrom(*ap);
  }
}

void tAbstractPort::DisconnectImplementation(tAbstractPort& source, tAbstractPort& destination)
{
  tEdgeAggregator::EdgeRemoved(source, destination);

  destination.incoming_connections.Remove(&source);
  source.outgoing_connections.Remove(&destination);

  internal::tFinstructedEdgeInfo* info = source.GetAnnotation<internal::tFinstructedEdgeInfo>();
  if (info)
  {
    auto& vec = info->outgoing_edges_finstructed;
    vec.erase(std::remove(vec.begin(), vec.end(), &destination), vec.end());
  }

  destination.OnDisconnect(source, false);
  source.OnDisconnect(destination, true);

  source.PublishUpdatedEdgeInfo(tRuntimeListener::tEvent::REMOVE, destination);
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
    return GetFlag(tFlag::EMITS_DATA) ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
  }
  if (!(other.GetFlag(tFlag::EMITS_DATA) && other.GetFlag(tFlag::ACCEPTS_DATA))) // not a proxy port?
  {
    return other.GetFlag(tFlag::ACCEPTS_DATA) ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
  }

  // Temporary variable to store result: Return tConnectDirection::TO_TARGET?
  bool return_to_target = true;

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
    return_to_target = this_output_proxy;
  }
  else if (other.GetFlag(tFlag::NETWORK_ELEMENT))
  {
    return_to_target = !other_output_proxy;
  }
  else if (this_output_proxy != other_output_proxy)
  {
    // If we have an output and an input port, typically, the output port is connected to the input port
    return_to_target = this_output_proxy;

    // If ports are in interfaces of the same group/module, connect in the reverse of the typical direction
    if (ports_have_same_parent)
    {
      return_to_target = !return_to_target;
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
      return_to_target = (this_output_proxy && other_parent_node_count < this_parent_node_count) ||
                         ((!this_output_proxy) && this_parent_node_count < other_parent_node_count);
    }
    else
    {
      FINROC_LOG_PRINTF(WARNING, "Two proxy ports ('%s' and '%s') in the same direction and on the same level are to be connected. Cannot infer direction. Guessing TO_TARGET.",
                        this->GetQualifiedName().c_str(), other.GetQualifiedName().c_str());
    }
  }

  return return_to_target ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
}

bool tAbstractPort::IsConnectedTo(tAbstractPort& target) const
{
  for (auto it = OutgoingConnectionsBegin(); it != OutgoingConnectionsEnd(); ++it)
  {
    if (&(*it) == &target)
    {
      return true;
    }
  }
  for (auto it = IncomingConnectionsBegin(); it != IncomingConnectionsEnd(); ++it)
  {
    if (&(*it) == &target)
    {
      return true;
    }
  }
  return false;
}

bool tAbstractPort::IsEdgeFinstructed(tAbstractPort& destination) const
{
  internal::tFinstructedEdgeInfo* info = this->GetAnnotation<internal::tFinstructedEdgeInfo>();
  if (info)
  {
    auto& vec = info->outgoing_edges_finstructed;
    return std::find(vec.begin(), vec.end(), &destination) != vec.end();
  }
  return false;
}

tString tAbstractPort::MakeAbsoluteLink(const tString& rel_link) const
{
  if (rel_link.length() > 0 && rel_link[0] == '/')
  {
    return rel_link;
  }
  tFrameworkElement* relative_to = GetParent();
  tString rel_link2 = rel_link;
  while (rel_link2.compare(0, 3, "../") == 0) // starts with '../'?
  {
    rel_link2 = rel_link2.substr(3);
    relative_to = relative_to->GetParent();
  }
  return relative_to->GetQualifiedLink() + "/" + rel_link2;
}

bool tAbstractPort::MayConnectTo(tAbstractPort& target, std::string* reason_string) const
{
  if (!data_type.IsConvertibleTo(target.data_type))
  {
    if (reason_string)
    {
      (*reason_string) += "Data types are incompatible (source: '" + GetDataType().GetName() + "' and target: '" + target.GetDataType().GetName() + "').";
    }
    return false;
  }

  if (!GetFlag(tFlag::EMITS_DATA))
  {
    if (reason_string)
    {
      (*reason_string) += "Port '" + this->GetQualifiedName() + "' does not emit data.";
    }
    return false;
  }

  if (!target.GetFlag(tFlag::ACCEPTS_DATA))
  {
    if (reason_string)
    {
      (*reason_string) += "Port '" + target.GetQualifiedName() + "' does not accept data.";
    }
    return false;
  }

  if (GetFlag(tFlag::TOOL_PORT) || target.GetFlag(tFlag::TOOL_PORT))
  {
    return true; // ignore constraints for ports from tools
  }

  auto& constraints = GetConnectionConstraintList();
  for (auto it = constraints.begin(); it != constraints.end(); ++it)
  {
    if (!(*it)->AllowPortConnection(*this, target))
    {
      if (reason_string)
      {
        (*reason_string) += std::string("The following constraint disallows connection: '") + (*it)->Description() + "'";
      }
      return false;
    }
  }

  return true;
}

void tAbstractPort::PrepareDelete()
{
  rrlib::thread::tLock lock1(GetStructureMutex());

  // disconnect all edges
  DisconnectAll();
}

void tAbstractPort::SetWrapperDataType(const rrlib::rtti::tType& wrapper_data_type)
{
  if (this->wrapper_data_type != NULL && wrapper_data_type != this->wrapper_data_type)
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
