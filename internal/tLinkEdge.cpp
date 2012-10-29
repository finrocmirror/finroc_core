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
/*!\file    core/internal/tLinkEdge.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tLinkEdge.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

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
namespace internal
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
// workaround for gcc 4.5
static std::array<tLinkEdge::tPortReference, 2> CreatePortReferenceArray(const tLinkEdge::tPortReference& port1, const tLinkEdge::tPortReference& port2)
{
  std::array<tLinkEdge::tPortReference, 2> result = {{ port1, port2 }};
  return result;
}

tLinkEdge::tLinkEdge(const tPortReference& port1, const tPortReference& port2, bool both_connect_directions, bool finstructed) :
  ports(CreatePortReferenceArray(port1, port2)),
  both_connect_directions(both_connect_directions),
  next_edge(NULL),
  finstructed(finstructed)
{
  if (ports[0].link.length() == 0 && ports[1].link.length() == 0)
  {
    FINROC_LOG_PRINT(ERROR, "At least one of two ports needs to be linked. Otherwise, it does not make sense to use this class.");
    abort();
  }
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  for (size_t i = 0; i < 2; i++)
  {
    if (ports[i].link.length() > 0)
    {
      tRuntimeEnvironment::GetInstance().AddLinkEdge(ports[i].link, *this);
    }
  }
}

tLinkEdge::~tLinkEdge()
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  for (size_t i = 0; i < 2; i++)
  {
    if (ports[i].link.length() > 0)
    {
      tRuntimeEnvironment::GetInstance().RemoveLinkEdge(ports[i].link, *this);
    }
  }
}

void tLinkEdge::LinkAdded(tRuntimeEnvironment& re, const tString& link, tAbstractPort& port) const
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  if (link.compare(ports[0].link) == 0)
  {
    tAbstractPort* target = ports[1].link.length() > 0 ? re.GetPort(ports[1].link) : ports[1].pointer;
    if (target)
    {
      port.ConnectTo(*target, both_connect_directions ? tAbstractPort::tConnectDirection::AUTO : tAbstractPort::tConnectDirection::TO_TARGET, finstructed);
    }
  }
  else if (link.compare(ports[1].link) == 0)
  {
    tAbstractPort* source = ports[0].link.length() > 0 ? re.GetPort(ports[0].link) : ports[0].pointer;
    if (source)
    {
      port.ConnectTo(*source, both_connect_directions ? tAbstractPort::tConnectDirection::AUTO : tAbstractPort::tConnectDirection::TO_SOURCE, finstructed);
    }
  }
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
