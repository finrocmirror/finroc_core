/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#include "core/port/tPortGroup.h"
#include "core/port/tAbstractPort.h"
#include "core/port/std/tPortBase.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/rpc/tInterfacePort.h"

namespace finroc
{
namespace core
{

tPortGroup::tPortGroup(tFrameworkElement* parent, const util::tString& name, uint flags, int default_port_flags) :
    tEdgeAggregator(parent, name, flags),
    default_port_flags(default_port_flags)
{
}

void tPortGroup::ConnectImpl(int op, tPortGroup* group, const util::tString& group_link, bool create_missing_ports, tAbstractPort* start_with, int count, const util::tString& port_prefix, const util::tString& other_port_prefix)
{
  int org_count = count;
  tChildIterator ci(this);
  tAbstractPort* p = NULL;
  while ((p = ci.NextPort()) != NULL)
  {
    util::tString name(p->GetDescription());
    if (p == start_with)
    {
      start_with = NULL;
    }
    if (start_with != NULL || (!name.StartsWith(port_prefix)))
    {
      continue;
    }
    if (count == 0)
    {
      return;
    }
    count--;
    name = name.Substring(port_prefix.Length());

    // connect-function specific part
    if (op <= 1)
    {
      tFrameworkElement* child = group->GetChild(other_port_prefix + name);
      if (child != NULL && child->IsPort())
      {
        if (op == 0)
        {
          p->ConnectToSource(static_cast<tAbstractPort*>(child));
        }
        else
        {
          p->ConnectToTarget(static_cast<tAbstractPort*>(child));
        }
      }
      else if (create_missing_ports)
      {
        child = group->CreatePort(other_port_prefix + name, p->GetDataType(), 0);
        if (op == 0)
        {
          p->ConnectToSource(static_cast<tAbstractPort*>(child));
        }
        else
        {
          p->ConnectToTarget(static_cast<tAbstractPort*>(child));
        }
      }
    }
    else if (op == 2)
    {
      p->ConnectToSource(group_link + "/" + other_port_prefix + name);
    }
    else if (op == 3)
    {
      p->ConnectToTarget(group_link + "/" + other_port_prefix + name);
    }
    // connect-function specific part end

  }
  if (start_with != NULL)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Port ", start_with->GetQualifiedName(), " no child of ", this->GetQualifiedName(), ". Did not connect anything.");
  }
  if (count > 0)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Could only connect ", (org_count - count), " ports (", org_count, " desired).");
  }
}

tAbstractPort* tPortGroup::CreatePort(const util::tString& name, rrlib::serialization::tDataTypeBase type, int extra_flags)
{
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain, "Creating port ", name, " in IOVector ", this->GetQualifiedLink());
  tAbstractPort* ap = NULL;
  if (tFinrocTypeInfo::IsStdType(type))
  {
    ap = new tPortBase(tPortCreationInfoBase(name, this, type, default_port_flags | extra_flags));
  }
  else if (tFinrocTypeInfo::IsCCType(type))
  {
    ap = new tCCPortBase(tPortCreationInfoBase(name, this, type, default_port_flags | extra_flags));
  }
  else if (tFinrocTypeInfo::IsMethodType(type))
  {
    ap = new tInterfacePort(name, this, type, tInterfacePort::eRouting);
  }
  else
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Cannot create port with type: ", type.GetName());
  }
  if (ap != NULL)
  {
    ap->Init();
  }
  return ap;
}

void tPortGroup::DisconnectAll(bool incoming, bool outgoing, tAbstractPort* start_with, int count)
{
  tChildIterator ci(this);
  tAbstractPort* p = NULL;
  while ((p = ci.NextPort()) != NULL)
  {
    if (p == start_with)
    {
      start_with = NULL;
    }
    if (start_with != NULL)
    {
      continue;
    }
    if (count == 0)
    {
      return;
    }
    count--;
    p->DisconnectAll(incoming, outgoing);
  }
}

} // namespace finroc
} // namespace core

