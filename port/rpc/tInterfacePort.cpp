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
#include "core/port/tMultiTypePortDataBufferPool.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tInterfacePort::tInterfacePort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& data_type, tInterfacePort::tType type_) :
  tAbstractPort(ProcessPci((tPortCreationInfoBase(name, parent, data_type, 0)), type_, -1)),
  type(type_),
  edges_src(),
  edges_dest(),
  buf_pool((type_ == tType::ROUTING) ? NULL : new tMultiTypePortDataBufferPool())
{
  // this(new PortCreationInfo(name,parent,dataType,0),type,-1);
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::tInterfacePort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& data_type, tInterfacePort::tType type_, uint custom_flags) :
  tAbstractPort(ProcessPci((tPortCreationInfoBase(name, parent, data_type, custom_flags)), type_, -1)),
  type(type_),
  edges_src(),
  edges_dest(),
  buf_pool((type_ == tType::ROUTING) ? NULL : new tMultiTypePortDataBufferPool())
{
  // this(new PortCreationInfo(name,parent,dataType,customFlags),type,-1);
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::tInterfacePort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& data_type, tInterfacePort::tType type_, uint custom_flags, int lock_level) :
  tAbstractPort(ProcessPci((tPortCreationInfoBase(name, parent, data_type, custom_flags)), type_, lock_level)),
  type(type_),
  edges_src(),
  edges_dest(),
  buf_pool((type_ == tType::ROUTING) ? NULL : new tMultiTypePortDataBufferPool())
{
  // this(new PortCreationInfo(name,parent,dataType,customFlags),type,lockLevel);
  InitLists(&(edges_src), &(edges_dest));
}

tInterfacePort::tInterfacePort(tPortCreationInfoBase pci, tInterfacePort::tType type_, int lock_level) :
  tAbstractPort(ProcessPci(pci, type_, lock_level)),
  type(type_),
  edges_src(),
  edges_dest(),
  buf_pool((type_ == tType::ROUTING) ? NULL : new tMultiTypePortDataBufferPool())
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
    util::tArrayWrapper<tInterfacePort*>* it = current->edges_src.GetIterable();
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

    if (current->GetType() == tType::SERVER || current->GetType() == tType::NETWORK)
    {
      return current;
    }
  }
}

tPortDataManager* tInterfacePort::GetUnusedBufferRaw(const rrlib::rtti::tDataTypeBase& dt)
{
  assert((!tFinrocTypeInfo::IsCCType(dt)));
  assert((buf_pool != NULL));
  return buf_pool->GetUnusedBuffer(dt);
}

tAbstractPort::tConnectDirection tInterfacePort::InferConnectDirection(const tAbstractPort& other) const
{
  // Check whether one of the two ports is connected to a server
  const tInterfacePort& other_interface_port = static_cast<const tInterfacePort&>(other);
  const tInterfacePort* server_port_of_this = (GetType() == tType::SERVER || GetType() == tType::NETWORK) ? this : GetServer();
  const tInterfacePort* server_port_of_other = (other_interface_port.GetType() == tType::SERVER || other_interface_port.GetType() == tType::NETWORK) ? &other_interface_port : other_interface_port.GetServer();
  if (server_port_of_this && server_port_of_other)
  {
    FINROC_LOG_PRINT(WARNING, "Both ports (this and %s) are connected to a server already.", other.GetQualifiedLink().c_str());
  }
  else if (server_port_of_this)
  {
    return tConnectDirection::TO_SOURCE;
  }
  else if (server_port_of_other)
  {
    return tConnectDirection::TO_TARGET;
  }

  return tAbstractPort::InferConnectDirection(other);
}

tPortCreationInfoBase tInterfacePort::ProcessPci(tPortCreationInfoBase pci, tInterfacePort::tType type_, int lock_level)
{
  switch (type_)
  {
  case tType::SERVER:
    pci.flags |= tPortFlags::cACCEPTS_DATA;
    break;
  case tType::CLIENT:
    pci.flags |= tPortFlags::cEMITS_DATA | tPortFlags::cOUTPUT_PORT;
    break;
  case tType::NETWORK:
  case tType::ROUTING:
    pci.flags |= tPortFlags::cEMITS_DATA | tPortFlags::cACCEPTS_DATA | tPortFlags::cPROXY;
    break;
  }
  if (lock_level >= 0)
  {
    pci.lock_order = lock_level;
  }
  return pci;
}

void tInterfacePort::RawConnectToTarget(tAbstractPort& target, bool finstructed)
{
  // Disconnect any server ports we might already be connected to.
  util::tArrayWrapper<tInterfacePort*>* it = edges_src.GetIterable();
  for (int i = 0, n = it->Size(); i < n; i++)
  {
    tAbstractPort* port = it->Get(i);
    if (port)
    {
      FINROC_LOG_PRINT_TO(edges, WARNING, "Port already connected to a server. Removing connection to '", port->GetQualifiedName(), "' and adding the new one to '", target.GetQualifiedName(), "'.");
      port->DisconnectFrom(*this);
    }
  }

  core::tAbstractPort::RawConnectToTarget(target, finstructed);
}

} // namespace finroc
} // namespace core

