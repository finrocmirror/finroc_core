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
#include "core/port/tPortFlags.h"
#include "core/port/stream/tNewConnectionHandler.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/std/tPortDataReference.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
template<typename T>
tInputStreamPort<T>::tInputStreamPort(const util::tString& description, tPortCreationInfo<T> pci, tInputPacketProcessor<T>* user, tNewConnectionHandler* conn_handler)
{
  this->wrapped = new tPortImpl<T*>(ProcessPCI(pci, description), user, conn_handler);
}

template<typename T>
tPortCreationInfo<T> tInputStreamPort<T>::ProcessPCI(tPortCreationInfo<T> pci, const util::tString& description)
{
  pci.max_queue_size = util::tInteger::cMAX_VALUE;   // unlimited size
  pci.description = description;
  pci.SetFlag(tPortFlags::cHAS_AND_USES_QUEUE, true);
  pci.SetFlag(tPortFlags::cOUTPUT_PORT, false);
  //pci.setFlag(PortFlags.ACCEPTS_REVERSE_DATA, false);
  return ::finroc::core::tPort::ProcessPci(pci);
}

template<typename T>
tInputStreamPort<T>::tPortImpl<T>::tPortImpl(tPortCreationInfo<T> pci, tInputPacketProcessor<T>* user_, tNewConnectionHandler* conn_handler_) :
  tPortBase(ProcessPci(pci)),
  dequeue_buffer(),
  user(user_),
  conn_handler(conn_handler_)
{
}

template<typename T>
void tInputStreamPort<T>::tPortImpl<T>::NewConnection(tAbstractPort* partner)
{
  if (conn_handler != NULL)
  {
    conn_handler->HandleNewConnection(partner);
  }
}

template<typename T>
void tInputStreamPort<T>::tPortImpl<T>::NonStandardAssign(tPublishCache& pc)
{
  if (user == NULL || ProcessPacket(static_cast<T*>(pc.cur_ref->GetData()->GetData())))
  {
    ::finroc::core::tPortBase::NonStandardAssign(pc);  // enqueue
  }
}

template<typename T>
bool tInputStreamPort<T>::tPortImpl<T>::ProcessPacket(T* data)
{
  try
  {
    return user->ProcessPacket(data);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Error while processing packet: ", e);
  }
  return false;
}

template<typename T>
void tInputStreamPort<T>::tPortImpl<T>::ProcessPackets()
{
  DequeueAllRaw(dequeue_buffer);
  rrlib::serialization::tGenericObject* pdr = NULL;
  while ((pdr = dequeue_buffer.DequeueAutoLocked()) != NULL)
  {
    user->ProcessPacket(pdr->GetData<T>());
  }
  ReleaseAutoLocks();
}

} // namespace finroc
} // namespace core

