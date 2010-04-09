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
#include "core/port/rpc/tInterfacePort.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tInterfacePort::tInterfacePort(const util::tString& description, tFrameworkElement* parent, tDataType* data_type, tInterfacePort::tType type_) :
    tAbstractPort(ProcessPci((tPortCreationInfo(description, parent, data_type, 0)), type_)),
    type(type_),
    edges_src(),
    edges_dest(),
    buf_pool((type_ == tInterfacePort::eRouting) ? NULL : new tMultiTypePortDataBufferPool())
{
  // this(new PortCreationInfo(description,parent,dataType,0),type);
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::tInterfacePort(const util::tString& description, tFrameworkElement* parent, tDataType* data_type, tInterfacePort::tType type_, int custom_flags) :
    tAbstractPort(ProcessPci((tPortCreationInfo(description, parent, data_type, custom_flags)), type_)),
    type(type_),
    edges_src(),
    edges_dest(),
    buf_pool((type_ == tInterfacePort::eRouting) ? NULL : new tMultiTypePortDataBufferPool())
{
  // this(new PortCreationInfo(description,parent,dataType,customFlags),type);
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::tInterfacePort(tPortCreationInfo pci, tInterfacePort::tType type_) :
    tAbstractPort(ProcessPci(pci, type_)),
    type(type_),
    edges_src(),
    edges_dest(),
    buf_pool((type_ == tInterfacePort::eRouting) ? NULL : new tMultiTypePortDataBufferPool())
{
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::~tInterfacePort()
{
  if (buf_pool != NULL)
  {
    delete buf_pool;
  }

}

tInterfacePort* tInterfacePort::GetServer()
{
  tInterfacePort* current = this;
  while (true)
  {
    tInterfacePort* last = current;
    util::tArrayWrapper<tInterfacePort*>* it = current->edges_dest.GetIterable();
    for (size_t i = 0u, n = it->Size(); i < n; i++)
    {
      tInterfacePort* ip = static_cast<tInterfacePort*>(it->Get(i));
      if (ip != NULL)
      {
        current = ip;
        break;
      }
    }

    if (current == NULL || current == last)
    {
      return NULL;
    }

    if (current->GetType() == tInterfacePort::eServer || current->GetType() == tInterfacePort::eNetwork)
    {
      return current;
    }
  }
}

tPortData* tInterfacePort::GetUnusedBuffer(tDataType* dt)
{
  assert((!dt->IsCCType()));
  assert((buf_pool != NULL));
  return buf_pool->GetUnusedBuffer(dt);
}

tPortCreationInfo tInterfacePort::ProcessPci(tPortCreationInfo pci, tInterfacePort::tType type_)
{
  switch (type_)
  {
  case eServer:
    pci.flags |= tPortFlags::cEMITS_DATA | tPortFlags::cOUTPUT_PORT;
    break;
  case eClient:
    pci.flags |= tPortFlags::cACCEPTS_DATA;
    break;
  case eNetwork:
  case eRouting:
    pci.flags |= tPortFlags::cEMITS_DATA | tPortFlags::cACCEPTS_DATA;
    break;
  }
  return pci;
}

void tInterfacePort::RawConnectToTarget(tAbstractPort* target)
{
  tInterfacePort* target2 = static_cast<tInterfacePort*>(target);

  // disconnect old port(s) - should always be max. one - however, defensive implementation
  while (target2->edges_dest.Size() > 0)    // disconnect old port
  {
    target2->edges_dest.GetIterable()->Get(0)->DisconnectFrom(target2);
  }

  ::finroc::core::tAbstractPort::RawConnectToTarget(target);
}

} // namespace finroc
} // namespace core

