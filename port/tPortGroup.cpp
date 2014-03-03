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
/*!\file    core/port/tPortGroup.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tPortFactory.h"

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

tPortGroup::tPortGroup(tFrameworkElement* parent, const std::string& name, tFlags flags, tFlags default_port_flags) :
  tEdgeAggregator(parent, name, flags),
  default_port_flags(default_port_flags)
{
}

void tPortGroup::ConnectImpl(tPortGroup* other_group, const std::string& group_link, bool create_missing_ports, tAbstractPort* start_with, int count, const std::string& port_prefix, const std::string& other_port_prefix)
{
  int org_count = count;
  for (auto it = ChildPortsBegin(); it != ChildPortsEnd(); ++it)
  {
    tAbstractPort& child_port = *it;
    std::string name(child_port.GetName());
    if (&child_port == start_with)
    {
      start_with = NULL;
    }
    if (start_with || (name.compare(0, port_prefix.length(), port_prefix)) != 0) // skip if we have not reached start port or port does not start with 'port_prefix'?
    {
      continue;
    }
    if (count == 0)
    {
      return;
    }
    count--;
    name = name.substr(port_prefix.length());

    // connect-function specific part
    if (other_group)
    {
      tFrameworkElement* other_child_port = other_group->GetChild(other_port_prefix + name);
      if (other_child_port && other_child_port->IsPort())
      {
        child_port.ConnectTo(static_cast<tAbstractPort&>(*other_child_port));
      }
      else if (create_missing_ports)
      {
        other_child_port = other_group->CreatePort(other_port_prefix + name, child_port.GetDataType());
        child_port.ConnectTo(static_cast<tAbstractPort&>(*other_child_port));
      }
    }
    else if (group_link.length() > 0)
    {
      child_port.ConnectTo(group_link + "/" + other_port_prefix + name);
    }
    // connect-function specific part end
  }
  if (start_with)
  {
    FINROC_LOG_PRINT(WARNING, "Port ", start_with->GetQualifiedName(), " no child of ", this->GetQualifiedName(), ". Did not connect anything.");
  }
  if (count > 0)
  {
    FINROC_LOG_PRINT(WARNING, "Could only connect ", (org_count - count), " ports (", org_count, " desired).");
  }
}

tAbstractPort* tPortGroup::CreatePort(const std::string& name, rrlib::rtti::tType type, tFlags extra_flags)
{
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Creating port ", name, " in IOVector ", this->GetQualifiedLink());

  tAbstractPort* ap = NULL;
  ap = tPortFactory::CreatePort(name, *this, type, default_port_flags | extra_flags);
  if (ap)
  {
    ap->Init();
  }
  return ap;
}

void tPortGroup::DisconnectAll(bool incoming, bool outgoing, tAbstractPort* start_with, int count)
{
  for (auto it = ChildPortsBegin(); it != ChildPortsEnd(); ++it)
  {
    if (&(*it) == start_with)
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
    it->DisconnectAll(incoming, outgoing);
  }
}

tAbstractPort& tPortGroup::operator[](size_t index)
{
  auto it = ChildPortsBegin();
  for (size_t i = 0; i < index; i++)
  {
    ++it;
  }
  if (it == ChildPortsEnd())
  {
    throw std::runtime_error("Out of bounds");
  }
  return *it;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
