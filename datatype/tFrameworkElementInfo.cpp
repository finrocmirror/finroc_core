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
#include "core/datatype/tFrameworkElementInfo.h"
#include "core/buffers/tCoreInput.h"
#include "core/port/net/tRemoteTypes.h"
#include "core/buffers/tCoreOutput.h"
#include "core/tFrameworkElement.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
const int8 tFrameworkElementInfo::cEDGE_CHANGE;
const int tFrameworkElementInfo::cPARENT_FLAGS_TO_STORE;
const int tFrameworkElementInfo::cEDGE_AGG_PARENT_FLAGS_TO_STORE;

tFrameworkElementInfo::tFrameworkElementInfo() :
    links(),
    link_count(0),
    handle(0),
    type(NULL),
    flags(0),
    strategy(0),
    min_net_update_time(0),
    connections(),
    op_code(0)
{
}

void tFrameworkElementInfo::Deserialize(tCoreInput* is, tRemoteTypes& type_lookup)
{
  Reset();
  op_code = is->ReadByte();

  // read common info
  handle = is->ReadInt();
  flags = is->ReadInt();
  if (op_code == tRuntimeListener::cREMOVE)
  {
    return;
  }
  bool port_only_client = is->ReadBoolean();

  // read links
  link_count = 0;
  int8 next = 0;
  if (op_code == tRuntimeListener::cADD)
  {
    while ((next = is->ReadByte()) != 0)
    {
      tLinkInfo* li = &(links[link_count]);
      li->extra_flags = next & cPARENT_FLAGS_TO_STORE;
      if ((li->extra_flags & tCoreFlags::cEDGE_AGGREGATOR) > 0)
      {
        li->extra_flags |= ((static_cast<int>(is->ReadByte())) << 8);
      }
      li->name = is->ReadString();
      if (!port_only_client)
      {
        li->parent = is->ReadInt();
      }
      else
      {
        li->parent = 0;
      }
      link_count++;
    }
    assert((link_count > 0));
  }

  // possibly read port specific info
  if ((flags & tCoreFlags::cIS_PORT) > 0)
  {
    type = type_lookup.GetLocalType(is->ReadShort());
    strategy = is->ReadShort();
    min_net_update_time = is->ReadShort();

    if (!port_only_client)
    {
      int8 cnt = is->ReadByte();
      for (int i = 0; i < cnt; i++)
      {
        connections.Add(is->ReadInt());
      }
    }
  }
}

int tFrameworkElementInfo::FilterParentFlags(int extra_flags)
{
  if ((extra_flags & tCoreFlags::cEDGE_AGGREGATOR) != 0)
  {
    return extra_flags & cEDGE_AGG_PARENT_FLAGS_TO_STORE;
  }
  return extra_flags & cPARENT_FLAGS_TO_STORE;
}

util::tString tFrameworkElementInfo::GetOpCodeString() const
{
  switch (op_code)
  {
  case tRuntimeListener::cADD:
    return "ADD";
  case tRuntimeListener::cCHANGE:
    return "CHANGE";
  case tRuntimeListener::cREMOVE:
    return "REMOVE";
  case cEDGE_CHANGE:
    return "EDGE_CHANGE";
  default:
    return "INVALID OPCODE";
  }
}

void tFrameworkElementInfo::Reset()
{
  handle = 0;
  type = NULL;
  flags = 0;
  strategy = 0;
  min_net_update_time = 0;
  link_count = 0;
  connections.Clear();
}

void tFrameworkElementInfo::SerializeFrameworkElement(tFrameworkElement* fe, int8 op_code_, tCoreOutput* tp, tFrameworkElementTreeFilter element_filter, util::tStringBuilder& tmp)
{
  tp->WriteByte(op_code_);  // write opcode (see base class)

  // write common info
  tp->WriteInt(fe->GetHandle());
  tp->WriteInt(fe->GetAllFlags());
  if (op_code_ == tRuntimeListener::cREMOVE)
  {
    return;
  }
  assert((!fe->IsDeleted()));
  int cnt = fe->GetLinkCount();
  tp->WriteBoolean(element_filter.IsPortOnlyFilter());

  // write links (only when creating element)
  if (op_code_ == tRuntimeListener::cADD)
  {
    if (element_filter.IsPortOnlyFilter())
    {
      for (int i = 0; i < cnt; i++)
      {
        bool unique = fe->GetQualifiedLink(tmp, i);
        tp->WriteByte(1 | (unique ? tCoreFlags::cGLOBALLY_UNIQUE_LINK : 0));
        tp->WriteString(tmp.Substring(1));
      }
    }
    else
    {
      for (int i = 0; i < cnt; i++)
      {
        // we only serialize parents that target is interested in
        tFrameworkElement* parent = fe->GetParent(i);
        if (element_filter.Accept(parent, tmp))
        {
          // serialize 1 for another link - ORed with CoreFlags for parent LINK_ROOT and GLOBALLY_UNIQUE
          tp->WriteByte(1 | (parent->GetAllFlags() & cPARENT_FLAGS_TO_STORE));
          if (parent->GetFlag(tCoreFlags::cEDGE_AGGREGATOR))
          {
            tp->WriteByte((parent->GetAllFlags() & tEdgeAggregator::cALL_EDGE_AGGREGATOR_FLAGS) >> 8);
          }
          fe->WriteDescription(tp, i);
          tp->WriteInt(parent->GetHandle());
        }
      }
    }
    tp->WriteByte(0);
  }

  // possibly write port info
  if (fe->IsPort())
  {
    tAbstractPort* port = static_cast<tAbstractPort*>(fe);

    tp->WriteShort(port->GetDataType().GetUid());
    tp->WriteShort(port->GetStrategy());
    tp->WriteShort(port->GetMinNetUpdateInterval());

    if (element_filter.IsAcceptAllFilter())
    {
      port->SerializeOutgoingConnections(tp);
    }
    else if (!element_filter.IsPortOnlyFilter())
    {
      tp->WriteByte(0);
    }
  }
}

const util::tString tFrameworkElementInfo::ToString() const
{
  if (link_count > 0)
  {
    return GetOpCodeString() + " " + links[0].name + " (" + handle + ") - parent: " + links[0].parent + " - flags: " + flags;
  }
  else
  {
    return GetOpCodeString() + " (" + handle + ") - flags: " + flags;
  }
}

} // namespace finroc
} // namespace core

