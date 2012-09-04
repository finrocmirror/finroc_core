/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/serialization/serialization.h"

#include "core/port/tAbstractPort.h"
#include "core/tLockOrderLevels.h"
#include "core/tRuntimeListener.h"
#include "core/tLinkEdge.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "core/port/tEdgeAggregator.h"
#include "core/tFinrocAnnotation.h"
#include "core/port/tPortConnectionConstraint.h"

namespace finroc
{
namespace core
{

class tNetPort;

namespace internal
{

/*!
 * Finstructed edge info if port has more than 16 outgoing connections
 */
class tFinstructedEdgeInfo : public tFinrocAnnotation
{
public:

  /*! tAbstractPort::outgoing_edges_finstructed continued (starting at 17th port) */
  std::vector<bool> outgoing_edges_finstructed;

  tFinstructedEdgeInfo() :
    outgoing_edges_finstructed()
  {}
};

}

tAbstractPort::~tAbstractPort()
{

  //delete linksTo;
  delete link_edges;
}

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
constexpr rrlib::time::tDuration tAbstractPort::cPULL_TIMEOUT;
#else
rrlib::time::tDuration tAbstractPort::cPULL_TIMEOUT = std::chrono::seconds(1);
#endif
const int8 tAbstractPort::cNO_CHANGE, tAbstractPort::cCHANGED, tAbstractPort::cCHANGED_INITIAL;
const uint tAbstractPort::cBULK_N_EXPRESS;

tAbstractPort::tAbstractPort(tPortCreationInfoBase pci) :
  tFrameworkElement(pci.parent, pci.name, ProcessFlags(pci), pci.lock_order < 0 ? tLockOrderLevels::cPORT : pci.lock_order),
  changed(0),
  custom_changed_flag(0),
  edges_src(NULL),
  edges_dest(NULL),
  link_edges(NULL),
  strategy(-1),
  outgoing_edges_finstructed(0),
  data_type(pci.data_type),
  min_net_update_time(pci.min_net_update_interval)
{
}

tNetPort* tAbstractPort::AsNetPort()
{
  return NULL;
}

void tAbstractPort::CommitUpdateTimeChange()
{
  PublishUpdatedInfo(tRuntimeListener::cCHANGE);
  /*if (isShared() && (portSpecific || minNetUpdateTime <= 0)) {
      RuntimeEnvironment.getInstance().getSettings().getSharedPorts().commitUpdateTimeChange(getIndex(), getMinNetUpdateInterval());
  }*/
}

void tAbstractPort::ConnectTo(tAbstractPort& to, tConnectDirection connect_direction, bool finstructed)
{
  tLock lock2(GetRegistryLock());
  if (IsDeleted() || to.IsDeleted())
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Port already deleted!");
    return;
  }
  if (&to == this)
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect port to itself.");
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
    source.RawConnectToTarget(target, finstructed);
    target.PropagateStrategy(NULL, &source);
    source.NewConnection(target);
    target.NewConnection(source);
    FINROC_LOG_PRINT_TO(edges, DEBUG_VERBOSE_1, "Creating Edge from ", source.GetQualifiedName(), " to ", target.GetQualifiedName());

    // check whether we need an initial reverse push
    source.ConsiderInitialReversePush(target);
  }
}

void tAbstractPort::ConnectTo(const util::tString& link_name, tConnectDirection connect_direction, bool finstructed)
{
  if (link_name.length() == 0)
  {
    FINROC_LOG_PRINT_TO(edges, ERROR, "No link name specified for partner port. Not connecting anything.");
    return;
  }

  tLock lock2(GetRegistryLock());
  if (IsDeleted())
  {
    FINROC_LOG_PRINT_TO(edges, WARNING, "Port already deleted!");
    return;
  }
  if (link_edges == NULL)    // lazy initialization
  {
    link_edges = new util::tSimpleList<tLinkEdge*>();
  }
  for (size_t i = 0u; i < link_edges->Size(); i++)
  {
    if (boost::equals(link_edges->Get(i)->GetTargetLink(), link_name) || boost::equals(link_edges->Get(i)->GetSourceLink(), link_name))
    {
      return;
    }
  }
  switch (connect_direction)
  {
  case tConnectDirection::AUTO:
  case tConnectDirection::TO_TARGET:
    link_edges->Add(new tLinkEdge(*this, MakeAbsoluteLink(link_name), connect_direction == tConnectDirection::AUTO, finstructed));
    break;
  case tConnectDirection::TO_SOURCE:
    link_edges->Add(new tLinkEdge(MakeAbsoluteLink(link_name), *this, false, finstructed));
    break;
  }
}

void tAbstractPort::ConnectTo(tFrameworkElement& partner_port_parent, const util::tString& port_name, bool warn_if_not_available, tConnectDirection connect_direction)
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

void tAbstractPort::ConsiderInitialReversePush(tAbstractPort& target)
{
  if (IsReady() && target.IsReady())
  {
    if (ReversePushStrategy() && edges_src->CountElements() == 1)
    {
      FINROC_LOG_PRINT_TO(initial_pushes, DEBUG_VERBOSE_1, "Performing initial reverse push from ", target.GetQualifiedName(), " to ", GetQualifiedName());
      target.InitialPushTo(*this, true);
    }
  }
}

void tAbstractPort::DisconnectAll(bool incoming, bool outgoing)
{
  tLock lock2(GetRegistryLock());

  // remove link edges
  if (link_edges != NULL)
  {
    for (size_t i = 0u; i < link_edges->Size(); i++)
    {
      tLinkEdge* le = link_edges->Get(i);
      if ((incoming && le->GetSourceLink().length() > 0) || (outgoing && le->GetTargetLink().length() > 0))
      {
        link_edges->Remove(i);
        delete le;
        i--;
      }
    }
  }
  assert(((!incoming) || (!outgoing) || (link_edges == NULL) || (link_edges->Size() == 0)));

  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  if (outgoing)
  {
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      tAbstractPort* target = it->Get(i);
      if (target)
      {
        RemoveInternal(*this, *target);
      }
    }
  }

  if (incoming)
  {
    it = edges_dest->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      tAbstractPort* target = it->Get(i);
      if (target)
      {
        RemoveInternal(*target, *this);
      }
    }
  }
}

void tAbstractPort::DisconnectFrom(tAbstractPort& target)
{
  bool found = false;
  {
    tLock lock2(GetRegistryLock());
    util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      if (it->Get(i) == &target)
      {
        RemoveInternal(*this, target);
        found = true;
      }
    }

    it = edges_dest->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      if (it->Get(i) == &target)
      {
        RemoveInternal(target, *this);
        found = true;
      }
    }
  }
  if (!found)
  {
    FINROC_LOG_PRINT_TO(edges, DEBUG_WARNING, "edge not found in AbstractPort::disconnectFrom()");
  }
  // not found: throw error message?
}

void tAbstractPort::DisconnectFrom(const util::tString& link)
{
  tLock lock2(GetRegistryLock());
  for (size_t i = 0u; i < link_edges->Size(); i++)
  {
    tLinkEdge* le = link_edges->Get(i);
    if (boost::equals(le->GetSourceLink(), link) || boost::equals(le->GetTargetLink(), link))
    {
      delete le;
    }
  }

  tAbstractPort* ap = GetRuntime().GetPort(link);
  if (ap != NULL)
  {
    DisconnectFrom(*this);
  }
}

void tAbstractPort::ForwardStrategy(int16 strategy2, tAbstractPort* push_wanter)
{
  util::tArrayWrapper<tAbstractPort*>* it = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL && (push_wanter != NULL || port->GetStrategy() != strategy2))
    {
      port->PropagateStrategy(push_wanter, NULL);
    }
  }
}

std::vector<tPortConnectionConstraint*>& tAbstractPort::GetConnectionConstraintList()
{
  typedef rrlib::design_patterns::tSingletonHolder<std::vector<tPortConnectionConstraint*>> tConstraintListSingleton;
  return tConstraintListSingleton::Instance();
}

void tAbstractPort::GetConnectionPartners(util::tSimpleList<tAbstractPort*>& result, bool outgoing_edges, bool incoming_edges, bool finstructed_edges_only)
{
  result.Clear();
  util::tArrayWrapper<tAbstractPort*>* it = NULL;

  if (outgoing_edges)
  {
    it = edges_src->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      tAbstractPort* target = it->Get(i);
      if (target == NULL || (finstructed_edges_only && (!IsEdgeFinstructed(i))))
      {
        continue;
      }
      result.Add(target);
    }
  }

  if (incoming_edges && (!finstructed_edges_only))
  {
    it = edges_dest->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      tAbstractPort* target = it->Get(i);
      if (target == NULL)
      {
        continue;
      }
      result.Add(target);
    }
  }
}

int16 tAbstractPort::GetMinNetworkUpdateIntervalForSubscription() const
{
  int16 result = util::tShort::cMAX_VALUE;
  int16 t = 0;

  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL && port->GetStrategy() > 0)
    {
      if ((t = port->GetMinNetUpdateInterval()) >= 0 && t < result)
      {
        result = t;
      }
    }
  }
  it = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL && port->GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
    {
      if ((t = port->GetMinNetUpdateInterval()) >= 0 && t < result)
      {
        result = t;
      }
    }
  }
  return result == util::tShort::cMAX_VALUE ? -1 : result;
}

int16 tAbstractPort::GetStrategyRequirement() const
{
  if (IsInputPort())
  {
    if (GetFlag(tPortFlags::cPUSH_STRATEGY))
    {
      if (GetFlag(tPortFlags::cUSES_QUEUE))
      {
        int qlen = GetMaxQueueLengthImpl();
        return static_cast<int16>((qlen > 0 ? qlen : util::tShort::cMAX_VALUE));
      }
      else
      {
        return 1;
      }
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return static_cast<int16>((IsConnected() ? 0 : -1));
  }
}

tAbstractPort::tConnectDirection tAbstractPort::InferConnectDirection(const tAbstractPort& other) const
{
  // If one port is no proxy port (only emits or accepts data), direction is clear
  if (!GetFlag(tPortFlags::cPROXY))
  {
    return GetFlag(tPortFlags::cEMITS_DATA) ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
  }
  if (!other.GetFlag(tPortFlags::cPROXY))
  {
    return other.GetFlag(tPortFlags::cACCEPTS_DATA) ? tConnectDirection::TO_TARGET : tConnectDirection::TO_SOURCE;
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
                                ((this_aggregator == other_aggregator) || (this_aggregator->GetParent() == other_aggregator->GetParent() && this_aggregator->GetFlag(tEdgeAggregator::cIS_INTERFACE) && other_aggregator->GetFlag(tEdgeAggregator::cIS_INTERFACE)));

  // Ports of network interfaces typically determine connection direction
  if (this->GetFlag(tCoreFlags::cNETWORK_ELEMENT))
  {
    return_to_target = this_output_proxy;
  }
  else if (other.GetFlag(tCoreFlags::cNETWORK_ELEMENT))
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
  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    if (it->Get(i) == &target)
    {
      return true;
    }
  }

  it = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    if (it->Get(i) == &target)
    {
      return true;
    }
  }
  return false;
}

bool tAbstractPort::IsConnectedToReversePushSources() const
{
  util::tArrayWrapper<tAbstractPort*>* it = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL && port->GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
    {
      return true;
    }
  }
  return false;
}

bool tAbstractPort::IsEdgeFinstructed(int idx) const
{
  if (idx < 0)
  {
    return false;
  }
  else if (idx < 16)
  {
    uint16_t flag = (uint16_t)(1 << idx);
    return (outgoing_edges_finstructed & flag) != 0;
  }
  else
  {
    internal::tFinstructedEdgeInfo* info = GetAnnotation<internal::tFinstructedEdgeInfo>();
    return info != NULL && info->outgoing_edges_finstructed[idx - 16];
  }
}

util::tString tAbstractPort::MakeAbsoluteLink(const util::tString& rel_link) const
{
  if (rel_link.length() > 0 && rel_link[0] == '/')
  {
    return rel_link;
  }
  tFrameworkElement* relative_to = GetParent();
  util::tString rel_link2 = rel_link;
  while (boost::starts_with(rel_link2, "../"))
  {
    rel_link2 = rel_link2.substr(3);
    relative_to = relative_to->GetParent();
  }
  return relative_to->GetQualifiedLink() + "/" + rel_link2;
}

bool tAbstractPort::MayConnectTo(tAbstractPort& target, bool warn_if_impossible) const
{
  if (!GetFlag(tPortFlags::cEMITS_DATA))
  {
    if (warn_if_impossible)
    {
      FINROC_LOG_PRINT_TO(edges, WARNING, "Cannot connect to target port '", target.GetQualifiedName(), "', because this (source) port does not emit data.");
    }
    return false;
  }

  if (!target.GetFlag(tPortFlags::cACCEPTS_DATA))
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

void tAbstractPort::PrintNotReadyMessage(const char* extra_message)
{
  if (IsDeleted())
  {
    FINROC_LOG_PRINT(DEBUG, "Port is about to be deleted. ", extra_message, " (This may happen occasionally due to non-blocking nature)");
  }
  else
  {
    FINROC_LOG_PRINT(WARNING, "Port has not been initialized yet and thus cannot be used. Fix your application. ", extra_message);
  }
}

uint tAbstractPort::ProcessFlags(const tPortCreationInfoBase& pci)
{
  uint flags = pci.flags;
  assert((((flags & cBULK_N_EXPRESS) != cBULK_N_EXPRESS)) && "Cannot be bulk and express port at the same time");
  assert((pci.data_type != NULL));
  if ((flags & cBULK_N_EXPRESS) == 0)
  {
    // no priority flags set... typical case... get them from type
    flags |= tFinrocTypeInfo::IsCCType(pci.data_type) ? tPortFlags::cIS_EXPRESS_PORT : tPortFlags::cIS_BULK_PORT;
  }
  if ((flags & tPortFlags::cPUSH_STRATEGY_REVERSE) != 0)
  {
    flags |= tPortFlags::cMAY_ACCEPT_REVERSE_DATA;
  }

  return flags | tCoreFlags::cIS_PORT;
}

bool tAbstractPort::PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner)
{
  tLock lock2(GetRegistryLock());

  // step1: determine max queue length (strategy) for this port
  int16 max = static_cast<int16>(std::min(GetStrategyRequirement(), util::tShort::cMAX_VALUE));
  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  util::tArrayWrapper<tAbstractPort*>* it_prev = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL)
    {
      max = static_cast<int16>(std::max(max, port->GetStrategy()));
    }
  }

  // has max length (strategy) for this port changed? => propagate to sources
  bool change = (max != strategy);

  // if origin wants a push - and we are a "source" port - provide this push (otherwise - "push wish" should be propagated further)
  if (push_wanter != NULL)
  {
    bool source_port = (strategy >= 1 && max >= 1) || edges_dest->IsEmpty();
    if (!source_port)
    {
      bool all_sources_reverse_pushers = true;
      for (int i = 0, n = it_prev->Size(); i < n; i++)
      {
        tAbstractPort* port = it_prev->Get(i);
        if (port != NULL && port->IsReady() && (!port->ReversePushStrategy()))
        {
          all_sources_reverse_pushers = false;
          break;
        }
      }
      source_port = all_sources_reverse_pushers;
    }
    if (source_port)
    {
      if (IsReady() && push_wanter->IsReady() && (!GetFlag(tPortFlags::cNO_INITIAL_PUSHING)) && (!push_wanter->GetFlag(tPortFlags::cNO_INITIAL_PUSHING)))
      {
        FINROC_LOG_PRINT_TO(initial_pushes, DEBUG_VERBOSE_1, "Performing initial push from ", GetQualifiedName(), " to ", push_wanter->GetQualifiedName());
        InitialPushTo(*push_wanter, false);
      }
      push_wanter = NULL;
    }
  }

  // okay... do we wish to receive a push?
  // yes if...
  //  1) we are target of a new connection, have a push strategy, no other sources, and partner is no reverse push source
  //  2) our strategy changed to push, and exactly one source
  int other_sources = 0;
  for (int i = 0, n = it_prev->Size(); i < n; i++)
  {
    tAbstractPort* port = it_prev->Get(i);
    if (port != NULL && port->IsReady() && port != new_connection_partner)
    {
      other_sources++;
    }
  }
  bool request_push = ((new_connection_partner != NULL) && (max >= 1) && (other_sources == 0) && (!new_connection_partner->ReversePushStrategy())) || ((max >= 1 && strategy < 1) && (other_sources == 1));

  // register strategy change
  if (change)
  {
    strategy = max;
  }

  ForwardStrategy(strategy, request_push ? this : NULL);  // forward strategy... do it anyway, since new ports may have been connected

  if (change)    // do this last to ensure that all relevant strategies have been set, before any network updates occur
  {
    PublishUpdatedInfo(tRuntimeListener::cCHANGE);
  }

  return change;
}

void tAbstractPort::RawConnectToTarget(tAbstractPort& target, bool finstructed)
{
  tEdgeAggregator::EdgeAdded(*this, target);

  size_t idx = edges_src->Add(&target, false);
  target.edges_dest->Add(this, false);
  if (finstructed)
  {
    SetEdgeFinstructed(idx, true);
  }

  PublishUpdatedEdgeInfo(tRuntimeListener::cADD, target);
}

void tAbstractPort::RemoveInternal(tAbstractPort& src, tAbstractPort& dest)
{
  tEdgeAggregator::EdgeRemoved(src, dest);

  dest.edges_dest->Remove(&src);
  src.edges_src->Remove(&dest);

  src.ConnectionRemoved(dest);
  dest.ConnectionRemoved(src);

  if (!src.IsConnected())
  {
    src.strategy = -1;
  }
  if (!dest.IsConnected())
  {
    dest.strategy = -1;
  }

  src.PublishUpdatedEdgeInfo(tRuntimeListener::cADD, dest);
  dest.PropagateStrategy(NULL, NULL);
  src.PropagateStrategy(NULL, NULL);
}

void tAbstractPort::SerializeOutgoingConnections(rrlib::serialization::tOutputStream& co)
{
  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  int8 count = 0;
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    if (it->Get(i) != NULL)
    {
      count++;
    }
  }
  co.WriteByte(count);
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* as = it->Get(i);
    if (as != NULL)
    {
      co.WriteInt(as->GetHandle());
      co.WriteBoolean(IsEdgeFinstructed(i));
    }
  }
}

void tAbstractPort::SetEdgeFinstructed(int idx, bool value)
{
  if (idx < 0)
  {
    return;
  }
  else if (idx < 16)
  {
    uint16_t flag = (uint16_t)(1 << idx);
    if (value)
    {
      outgoing_edges_finstructed |= flag;
    }
    else
    {
      outgoing_edges_finstructed &= ~flag;
    }
  }
  else
  {
    internal::tFinstructedEdgeInfo* info = GetAnnotation<internal::tFinstructedEdgeInfo>();
    if (info == NULL)
    {
      if (!value)
      {
        return;
      }
      info = new internal::tFinstructedEdgeInfo();
      AddAnnotation(info);
    }
    info->outgoing_edges_finstructed[idx - 16] = value;
  }
}

void tAbstractPort::SetMaxQueueLength(int queue_length)
{
  if (!GetFlag(tPortFlags::cHAS_QUEUE))
  {
    FINROC_LOG_PRINT(WARNING, "warning: tried to set queue length on port without queue - ignoring");
    return;
  }
  {
    tLock lock2(GetRegistryLock());
    if (queue_length <= 1)
    {
      RemoveFlag(tPortFlags::cUSES_QUEUE);
      ClearQueueImpl();
    }
    else if (queue_length != GetMaxQueueLengthImpl())
    {
      SetMaxQueueLengthImpl(queue_length);
      SetFlag(tPortFlags::cUSES_QUEUE);  // publishes change
    }
    PropagateStrategy(NULL, NULL);
  }
}

void tAbstractPort::SetMinNetUpdateInterval(int interval2)
{
  tLock lock2(GetRegistryLock());
  int16 interval = static_cast<int16>(std::min(interval2, static_cast<int>(util::tShort::cMAX_VALUE)));
  if (min_net_update_time != interval)
  {
    min_net_update_time = interval;
    CommitUpdateTimeChange();
  }
}

void tAbstractPort::SetPushStrategy(bool push)
{
  tLock lock2(GetRegistryLock());
  if (push == GetFlag(tPortFlags::cPUSH_STRATEGY))
  {
    return;
  }
  SetFlag(tPortFlags::cPUSH_STRATEGY, push);
  PropagateStrategy(NULL, NULL);
}

void tAbstractPort::SetReversePushStrategy(bool push)
{
  if (!AcceptsReverseData() || push == GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
  {
    return;
  }

  {
    tLock lock2(GetRegistryLock());
    SetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE, push);
    if (push && IsReady())    // strategy change
    {
      util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
      for (int i = 0, n = it->Size(); i < n; i++)
      {
        tAbstractPort* ap = it->Get(i);
        if (ap != NULL && ap->IsReady())
        {
          FINROC_LOG_PRINT_TO(initial_pushes, DEBUG_VERBOSE_1, "Performing initial reverse push from ", ap->GetQualifiedName(), " to ", GetQualifiedName());
          ap->InitialPushTo(*this, true);
          break;
        }
      }
    }
    this->PublishUpdatedInfo(tRuntimeListener::cCHANGE);
  }
}

void tAbstractPort::UpdateEdgeStatistics(tAbstractPort& source, tAbstractPort& target, rrlib::rtti::tGenericObject* data)
{
  tEdgeAggregator::UpdateEdgeStatistics(source, target, tFinrocTypeInfo::EstimateDataSize(data));
}

} // namespace finroc
} // namespace core

