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
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
tLinkEdge::tLinkEdge(const util::tString& source_link_, int target_handle, bool finstructed) :
  source_link(source_link_),
  target_link(""),
  port_handle(target_handle),
  next(NULL),
  finstructed(finstructed)
{
  // this(sourceLink_,"",targetHandle);
  if (source_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(const util::tString& source_link_, const util::tString& target_link_, bool finstructed) :
  source_link(source_link_),
  target_link(target_link_),
  port_handle(-1),
  next(NULL),
  finstructed(finstructed)
{
  // this(sourceLink_,targetLink_,-1);
  if (source_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(int source_handle, const util::tString& target_link_, bool finstructed) :
  source_link(""),
  target_link(target_link_),
  port_handle(source_handle),
  next(NULL),
  finstructed(finstructed)
{
  // this("",targetLink_,sourceHandle);
  if (source_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::tLinkEdge(const util::tString& source_link_, const util::tString& target_link_, int port_handle_, bool finstructed) :
  source_link(source_link_),
  target_link(target_link_),
  port_handle(port_handle_),
  next(NULL),
  finstructed(finstructed)
{
  if (source_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(source_link, this);
  }
  if (target_link.length() > 0)
  {
    tRuntimeEnvironment::GetInstance()->AddLinkEdge(target_link, this);
  }
}

tLinkEdge::~tLinkEdge()
{
  {
    rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance()->GetRegistryLock());
    if (source_link.length() > 0)
    {
      tRuntimeEnvironment::GetInstance()->RemoveLinkEdge(source_link, this);
    }
    if (target_link.length() > 0)
    {
      tRuntimeEnvironment::GetInstance()->RemoveLinkEdge(target_link, this);
    }
  }
}

void tLinkEdge::LinkAdded(tRuntimeEnvironment& re, const util::tString& link, tAbstractPort& port)
{
  {
    rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance()->GetRegistryLock());
    if (link.compare(source_link) == 0)
    {
      tAbstractPort* target = target_link.length() > 0 ? re.GetPort(target_link) : re.GetPort(port_handle);
      if (target != NULL)
      {
        port.ConnectToTarget(target, finstructed);
      }
    }
    else
    {
      tAbstractPort* source = source_link.length() > 0 ? re.GetPort(source_link) : re.GetPort(port_handle);
      if (source != NULL)
      {
        port.ConnectToSource(source, finstructed);
      }
    }
  }
}

} // namespace finroc
} // namespace core

