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
  tFrameworkElement(info.parent, info.name, info.flags | tFlag::PORT, info.lock_order < 0 ? static_cast<int>(tLockOrderLevel::PORT) : info.lock_order),
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
  tLock lock(GetStructureMutex());
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
    bool to_target_possible = MayConnectTo(to, false);
    bool to_source_possible = to.MayConnectTo(*this, false);
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
      FINROC_LOG_PRINT(WARNING, "Could not connect ports '", GetQualifiedName(), "' and '", to.GetQualifiedName(), "' for the following reasons:");
      MayConnectTo(to, true);
      to.MayConnectTo(*this, true);
    }
  }

  // connect
  tAbstractPort& source = (connect_direction == tConnectDirection::TO_TARGET) ? *this : to;
  tAbstractPort& target = (connect_direction == tConnectDirection::TO_TARGET) ? to : *this;
  if (source.MayConnectTo(target, true))
  {
    FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Creating Edge from ", source.GetQualifiedName(), " to ", target.GetQualifiedName());
    source.ConnectImplementation(target, finstructed);
    source.ConnectionAdded(target, true);
    target.ConnectionAdded(source, false);
  }
}

void tAbstractPort::ConnectTo(const tString& link_name, tConnectDirection connect_direction, bool finstructed)
{
  if (link_name.length() == 0)
  {
    FINROC_LOG_PRINT_TO(edges, ERROR, "No link name specified for partner port. Not connecting anything.");
    return;
  }

  tLock lock2(GetStructureMutex());
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
    if (boost::equals((*it)->GetTargetLink(), link_name) || boost::equals((*it)->GetSourceLink(), link_name))
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

void tAbstractPort::DisconnectAll(bool incoming, bool outgoing)
{
  tLock lock(GetStructureMutex());

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
    tLock lock(GetStructureMutex());
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
  tLock lock(GetStructureMutex());
  for (auto it = link_edges->begin(); it != link_edges->end(); ++it)
  {
    if (boost::equals((*it)->GetSourceLink(), link) || boost::equals((*it)->GetTargetLink(), link))
    {
      delete &(*it);
      it = link_edges->erase(it);
    }
  }

  tAbstractPort* ap = GetRuntime().GetPort(link);
  if (ap)
  {
    DisconnectFrom(*this);
  }
}

void tAbstractPort::DisconnectImplementation(tAbstractPort& source, tAbstractPort& destination)
{
  tEdgeAggregator::EdgeRemoved(source, destination);

  destination.incoming_connections.Remove(&source);
  source.outgoing_connections.Remove(&destination);

  destination.ConnectionRemoved(source, false);
  source.ConnectionRemoved(destination, true);

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

tString tAbstractPort::MakeAbsoluteLink(const tString& rel_link) const
{
  if (rel_link.length() > 0 && rel_link[0] == '/')
  {
    return rel_link;
  }
  tFrameworkElement* relative_to = GetParent();
  tString rel_link2 = rel_link;
  while (boost::starts_with(rel_link2, "../"))
  {
    rel_link2 = rel_link2.substr(3);
    relative_to = relative_to->GetParent();
  }
  return relative_to->GetQualifiedLink() + "/" + rel_link2;
}

bool tAbstractPort::MayConnectTo(tAbstractPort& target, bool warn_if_impossible) const
{
  if (!GetFlag(tFlag::EMITS_DATA))
  {
    if (warn_if_impossible)
    {
      FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect to target port '", target.GetQualifiedName(), "', because this (source) port does not emit data.");
    }
    return false;
  }

  if (!target.GetFlag(tFlag::ACCEPTS_DATA))
  {
    if (warn_if_impossible)
    {
      FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect to target port '", target.GetQualifiedName(), "', because it does not accept data.");
    }
    return false;
  }

  if (!data_type.IsConvertibleTo(target.data_type))
  {
    if (warn_if_impossible)
    {
      FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect to target port '", target.GetQualifiedName(), "', because data types are incompatible ('", GetDataType().GetName(), "' and '", target.GetDataType().GetName(), "').");
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
      if (warn_if_impossible)
      {
        FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect to target port '", target.GetQualifiedName(), "', because the following constraint disallows this: '", (*it)->Description(), "'");
      }
      return false;
    }
  }

  return true;
}

void tAbstractPort::PrepareDelete()
{
  tLock lock1(*this);

  // disconnect all edges
  DisconnectAll();
}

void tAbstractPort::SetWrapperDataType(const rrlib::rtti::tDataTypeBase& wrapper_data_type)
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
