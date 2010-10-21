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
#include "core/portdatabase/tDataType.h"
#include "core/port/tAbstractPort.h"
#include "core/tLockOrderLevels.h"
#include "core/tRuntimeListener.h"
#include "core/tLinkEdge.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "core/port/net/tNetPort.h"
#include "core/port/tEdgeAggregator.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tAbstractPort::~tAbstractPort()
{
  if (AsNetPort() != NULL)
  {
    tNetPort* nt = AsNetPort();
    delete nt;
  }

  //delete linksTo;
  delete link_edges;
}

const int tAbstractPort::cPULL_TIMEOUT;
const int8 tAbstractPort::cNO_CHANGE, tAbstractPort::cCHANGED, tAbstractPort::cCHANGED_INITIAL;
const int tAbstractPort::cBULK_N_EXPRESS;

tAbstractPort::tAbstractPort(tPortCreationInfo pci) :
    tFrameworkElement(pci.description, pci.parent, ProcessFlags(pci), pci.lock_order < 0 ? tLockOrderLevels::cPORT : pci.lock_order),
    changed(0),
    edges_src(NULL),
    edges_dest(NULL),
    link_edges(NULL),
    strategy(-1),
    data_type(pci.data_type),
    min_net_update_time(pci.min_net_update_interval)
{
  //      if (getFlag(PortFlags.IS_SHARED)) {
  //          createDefaultLink();
  //      }
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

void tAbstractPort::ConnectToSource(const util::tString& src_link)
{
  {
    util::tLock lock2(GetRegistryLock());
    if (IsDeleted())
    {
      return;
    }
    if (link_edges == NULL)    // lazy inizialization
    {
      link_edges = new util::tSimpleList<tLinkEdge*>();
    }
    for (size_t i = 0u; i < link_edges->Size(); i++)
    {
      if (link_edges->Get(i)->GetSourceLink().Equals(src_link))
      {
        return;
      }
    }
    link_edges->Add(new tLinkEdge(MakeAbsoluteLink(src_link), GetHandle()));
  }
}

void tAbstractPort::ConnectToTarget(tAbstractPort* target)
{
  {
    util::tLock lock2(GetRegistryLock());
    if (IsDeleted())
    {
      return;
    }
    if (MayConnectTo(target) && (!IsConnectedTo(target)))
    {
      RawConnectToTarget(target);
      //              strategy = (short)Math.max(0, strategy);
      //              target.strategy = (short)Math.max(0, target.strategy);
      target->PropagateStrategy(NULL, this);
      NewConnection(target);
      target->NewConnection(this);
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, edge_log, "creating Edge from ", GetQualifiedName(), " to ", target->GetQualifiedName());

      // check whether we need an initial reverse push
      ConsiderInitialReversePush(target);
    }
  }
}

void tAbstractPort::ConnectToTarget(const util::tString& dest_link)
{
  {
    util::tLock lock2(GetRegistryLock());
    if (IsDeleted())
    {
      return;
    }
    if (link_edges == NULL)    // lazy initialization
    {
      link_edges = new util::tSimpleList<tLinkEdge*>();
    }
    for (size_t i = 0u; i < link_edges->Size(); i++)
    {
      if (link_edges->Get(i)->GetTargetLink().Equals(dest_link))
      {
        return;
      }
    }
    link_edges->Add(new tLinkEdge(GetHandle(), MakeAbsoluteLink(dest_link)));
  }
}

void tAbstractPort::ConsiderInitialReversePush(tAbstractPort* target)
{
  if (IsReady() && target->IsReady())
  {
    if (ReversePushStrategy() && edges_src->CountElements() == 1)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, initial_push_log, "Performing initial reverse push from ", target->GetQualifiedName(), " to ", GetQualifiedName());
      target->InitialPushTo(this, true);
    }
  }
}

void tAbstractPort::DisconnectAll(bool incoming, bool outgoing)
{
  {
    util::tLock lock2(GetRegistryLock());

    // remove link edges
    if (link_edges != NULL)
    {
      for (size_t i = 0u; i < link_edges->Size(); i++)
      {
        tLinkEdge* le = link_edges->Get(i);
        if ((incoming && le->GetSourceLink().Length() > 0) || (outgoing && le->GetTargetLink().Length() > 0))
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
        if (target == NULL)
        {
          continue;
        }
        RemoveInternal(this, target);
      }
    }

    if (incoming)
    {
      it = edges_dest->GetIterable();
      for (int i = 0, n = it->Size(); i < n; i++)
      {
        tAbstractPort* target = it->Get(i);
        if (target == NULL)
        {
          continue;
        }
        RemoveInternal(target, this);
      }
    }
  }
}

void tAbstractPort::DisconnectFrom(tAbstractPort* target)
{
  bool found = false;
  {
    util::tLock lock2(GetRegistryLock());
    util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      if (it->Get(i) == target)
      {
        RemoveInternal(this, target);
        found = true;
      }
    }

    it = edges_dest->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      if (it->Get(i) == target)
      {
        RemoveInternal(target, this);
        found = true;
      }
    }
  }
  if (!found)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, edge_log, "edge not found in AbstractPort::disconnectFrom()");
  }
  // not found: throw error message?
}

void tAbstractPort::DisconnectFrom(const util::tString& link)
{
  {
    util::tLock lock2(GetRegistryLock());
    for (size_t i = 0u; i < link_edges->Size(); i++)
    {
      tLinkEdge* le = link_edges->Get(i);
      if (le->GetSourceLink().Equals(link) || le->GetTargetLink().Equals(link))
      {
        delete le;
      }
    }

    tAbstractPort* ap = GetRuntime()->GetPort(link);
    if (ap != NULL)
    {
      DisconnectFrom(this);
    }
  }
}

tNetPort* tAbstractPort::FindNetPort(util::tObject* belongs_to) const
{
  if (belongs_to == NULL)
  {
    return NULL;
  }
  util::tArrayWrapper<tAbstractPort*>* it = IsOutputPort() ? edges_src->GetIterable() : edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port != NULL && port->GetFlag(tCoreFlags::cNETWORK_ELEMENT))
    {
      tNetPort* np = port->AsNetPort();
      if (np->GetBelongsTo() == belongs_to)
      {
        return np;
      }
    }
  }
  return NULL;
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

void tAbstractPort::GetConnectionPartners(util::tSimpleList<tAbstractPort*>& result, bool outgoing_edges, bool incoming_edges)
{
  result.Clear();
  util::tArrayWrapper<tAbstractPort*>* it = NULL;

  if (outgoing_edges)
  {
    it = edges_src->GetIterable();
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

  if (incoming_edges)
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

bool tAbstractPort::IsConnectedTo(tAbstractPort* target)
{
  util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    if (it->Get(i) == target)
    {
      return true;
    }
  }

  it = edges_dest->GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    if (it->Get(i) == target)
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

util::tString tAbstractPort::MakeAbsoluteLink(const util::tString& rel_link)
{
  if (rel_link.StartsWith("/"))
  {
    return rel_link;
  }
  ::finroc::core::tFrameworkElement* relative_to = GetParent();
  util::tString rel_link2 = rel_link;
  while (rel_link2.StartsWith("../"))
  {
    rel_link2 = rel_link2.Substring(3);
    relative_to = relative_to->GetParent();
  }
  return relative_to->GetQualifiedLink() + "/" + rel_link2;
}

bool tAbstractPort::MayConnectTo(tAbstractPort* target)
{
  if (!(GetFlag(tPortFlags::cEMITS_DATA) && target->GetFlag(tPortFlags::cACCEPTS_DATA)))
  {
    return false;
  }

  // Check will be done by data type
  /*if (getFlag(PortFlags.IS_CC_PORT) != target.getFlag(PortFlags.IS_CC_PORT)) {
      return false;
  }
  if (getFlag(PortFlags.IS_INTERFACE_PORT) != target.getFlag(PortFlags.IS_INTERFACE_PORT)) {
      return false;
  }*/

  if (!data_type->IsConvertibleTo(target->data_type))
  {
    return false;
  }
  return MayConnectTo2(target);
}

void tAbstractPort::PrepareDelete()
{
  util::tLock lock1(this);

  //      // remove links
  //      if (linksTo != null) {
  //          for (@SizeT int i = 0; i < linksTo.size(); i++) {
  //              getRuntime().removeLink(linksTo.get(i));
  //          }
  //          linksTo.clear();
  //      }

  // disconnect all edges
  DisconnectAll();

  // publish deletion - done by FrameworkElement class now
  //publishUpdatedPortInfo();
}

int tAbstractPort::ProcessFlags(const tPortCreationInfo& pci)
{
  int flags = pci.flags;
  assert((((flags & cBULK_N_EXPRESS) != cBULK_N_EXPRESS)) && "Cannot be bulk and express port at the same time");
  assert((pci.data_type != NULL));
  if ((flags & cBULK_N_EXPRESS) == 0)
  {
    // no priority flags set... typical case... get them from type
    flags |= pci.data_type->IsCCType() ? tPortFlags::cIS_EXPRESS_PORT : tPortFlags::cIS_BULK_PORT;
  }
  if ((flags & tPortFlags::cPUSH_STRATEGY_REVERSE) != 0)
  {
    flags |= tPortFlags::cMAY_ACCEPT_REVERSE_DATA;
  }

  return flags | tCoreFlags::cIS_PORT;
}

bool tAbstractPort::PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner)
{
  {
    util::tLock lock2(GetRegistryLock());

    // step1: determine max queue length (strategy) for this port
    int16 max = static_cast<int16>(util::tMath::Min(GetStrategyRequirement(), util::tShort::cMAX_VALUE));
    util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
    util::tArrayWrapper<tAbstractPort*>* it_prev = edges_dest->GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      tAbstractPort* port = it->Get(i);
      if (port != NULL)
      {
        max = static_cast<int16>(util::tMath::Max(max, port->GetStrategy()));
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
        if (IsReady() && push_wanter->IsReady())
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, initial_push_log, "Performing initial push from ", GetQualifiedName(), " to ", push_wanter->GetQualifiedName());
          InitialPushTo(push_wanter, false);
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
}

void tAbstractPort::RawConnectToTarget(tAbstractPort* target)
{
  tEdgeAggregator::EdgeAdded(this, target);

  edges_src->Add(target, false);
  target->edges_dest->Add(this, false);

  PublishUpdatedEdgeInfo(tRuntimeListener::cADD, target);
}

void tAbstractPort::RemoveInternal(tAbstractPort* src, tAbstractPort* dest)
{
  tEdgeAggregator::EdgeRemoved(src, dest);

  dest->edges_dest->Remove(src);
  src->edges_src->Remove(dest);

  src->ConnectionRemoved(dest);
  dest->ConnectionRemoved(src);

  if (!src->IsConnected())
  {
    src->strategy = -1;
  }
  if (!dest->IsConnected())
  {
    dest->strategy = -1;
  }

  src->PublishUpdatedEdgeInfo(tRuntimeListener::cADD, dest);
  dest->PropagateStrategy(NULL, NULL);
  src->PropagateStrategy(NULL, NULL);
}

void tAbstractPort::SerializeOutgoingConnections(tCoreOutput* co)
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
  co->WriteByte(count);
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* as = it->Get(i);
    if (as != NULL)
    {
      co->WriteInt(as->GetHandle());
    }
  }
}

void tAbstractPort::SetMaxQueueLength(int queue_length)
{
  if (!GetFlag(tPortFlags::cHAS_QUEUE))
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "warning: tried to set queue length on port without queue - ignoring");
    return;
  }
  {
    util::tLock lock2(GetRegistryLock());
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
  {
    util::tLock lock2(GetRegistryLock());
    int16 interval = static_cast<int16>(util::tMath::Min(interval2, util::tShort::cMAX_VALUE));
    if (min_net_update_time != interval)
    {
      min_net_update_time = interval;
      CommitUpdateTimeChange();
    }
  }
}

void tAbstractPort::SetPushStrategy(bool push)
{
  {
    util::tLock lock2(GetRegistryLock());
    if (push == GetFlag(tPortFlags::cPUSH_STRATEGY))
    {
      return;
    }
    SetFlag(tPortFlags::cPUSH_STRATEGY, push);
    PropagateStrategy(NULL, NULL);
  }
}

void tAbstractPort::SetReversePushStrategy(bool push)
{
  if (!AcceptsReverseData() || push == GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
  {
    return;
  }

  {
    util::tLock lock2(GetRegistryLock());
    SetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE, push);
    if (push && IsReady())    // strategy change
    {
      util::tArrayWrapper<tAbstractPort*>* it = edges_src->GetIterable();
      for (int i = 0, n = it->Size(); i < n; i++)
      {
        tAbstractPort* ap = it->Get(i);
        if (ap != NULL && ap->IsReady())
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, initial_push_log, "Performing initial reverse push from ", ap->GetQualifiedName(), " to ", GetQualifiedName());
          ap->InitialPushTo(this, true);
          break;
        }
      }
    }
    this->PublishUpdatedInfo(tRuntimeListener::cCHANGE);
  }
}

void tAbstractPort::UpdateEdgeStatistics(tAbstractPort* source, tAbstractPort* target, tTypedObject* data)
{
  tEdgeAggregator::UpdateEdgeStatistics(source, target, tDataType::EstimateDataSize(data));
}

} // namespace finroc
} // namespace core

