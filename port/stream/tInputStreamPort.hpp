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
#include "core/port/std/tPortDataReference.h"
#include "core/port/tPortFlags.h"
#include "finroc_core_utils/log/tLogUser.h"
#include "core/port/std/tPortQueue.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
template<typename T>
tInputStreamPort<T>::tInputStreamPort(const util::tString& description, tPortCreationInfo pci, tInputPacketProcessor<T>* user_) :
    tPort<T>(ProcessPCI(pci, description)),
    dequeue_buffer(),
    user(user_)
{
}

template<typename T>
void tInputStreamPort<T>::NonStandardAssign(tPublishCache& pc)
{
  if (user == NULL || ProcessPacket(static_cast<T*>(pc.cur_ref->GetData())))
  {
    ::finroc::core::tPortBase::NonStandardAssign(pc);  // enqueue
  }
}

template<typename T>
tPortCreationInfo tInputStreamPort<T>::ProcessPCI(tPortCreationInfo pci, const util::tString& description)
{
  pci.max_queue_size = util::tInteger::cMAX_VALUE;   // unlimited size
  pci.description = description;
  pci.SetFlag(tPortFlags::cHAS_AND_USES_QUEUE, true);
  pci.SetFlag(tPortFlags::cOUTPUT_PORT, false);
  //pci.setFlag(PortFlags.ACCEPTS_REVERSE_DATA, false);
  return pci;
}

template<typename T>
bool tInputStreamPort<T>::ProcessPacket(T* data)
{
  try
  {
    return user->ProcessPacket(data);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Error while processing packet: ", e);
  }
  return false;
}

template<typename T>
void tInputStreamPort<T>::ProcessPackets()
{
  this->queue->DequeueAll(dequeue_buffer);
  T* pdr = NULL;
  while ((pdr = dequeue_buffer.DequeueUnsafe()) != NULL)
  {
    user->ProcessPacket(pdr);
    pdr->GetManager()->ReleaseLock();
  }
}

} // namespace finroc
} // namespace core

