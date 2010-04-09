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
#include "core/tLinkEdge.h"

namespace finroc
{
namespace core
{
tLinkEdge::tLinkEdge(const util::tString& source_link_, int target_handle) :
    source_link(source_link_),
    target_link(""),
    port_handle(target_handle),
    next(NULL),
    obj_synch()
{
  // this(sourceLink_,"",targetHandle);
  if (source_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(const util::tString& source_link_, const util::tString& target_link_) :
    source_link(source_link_),
    target_link(target_link_),
    port_handle(-1),
    next(NULL),
    obj_synch()
{
  // this(sourceLink_,targetLink_,-1);
  if (source_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(int source_handle, const util::tString& target_link_) :
    source_link(""),
    target_link(target_link_),
    port_handle(source_handle),
    next(NULL),
    obj_synch()
{
  // this("",targetLink_,sourceHandle);
  if (source_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(const util::tString& source_link_, const util::tString& target_link_, int port_handle_) :
    source_link(source_link_),
    target_link(target_link_),
    port_handle(port_handle_),
    next(NULL),
    obj_synch()
{
  if (source_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::~tLinkEdge()
{
  util::tLock lock1(obj_synch);
  if (source_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->RemoveLinkEdge(source_link, this);
  }
  if (target_link.Length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->RemoveLinkEdge(target_link, this);
  }
}

void tLinkEdge::LinkAdded(tRuntimeEnvironment* re, const util::tString& link, tAbstractPort* port)
{
  util::tLock lock1(obj_synch);
  if (link.Equals(source_link))
  {
    tAbstractPort* target = target_link.Length() > 0 ? re->GetPort(target_link) : re->GetPort(port_handle);
    if (target != NULL)
    {
      port->ConnectToTarget(target);
    }
  }
  else
  {
    tAbstractPort* source = source_link.Length() > 0 ? re->GetPort(source_link) : re->GetPort(port_handle);
    if (source != NULL)
    {
      port->ConnectToSource(source);
    }
  }
}

} // namespace finroc
} // namespace core

