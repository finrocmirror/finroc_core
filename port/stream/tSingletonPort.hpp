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
#include "core/port/std/tPortDataReference.h"

namespace finroc
{
namespace core
{
template<typename T>
tSingletonPort<T>::tSingletonPort(tPortCreationInfo pci, T& singleton)
{
  this->wrapped = new tPortImpl<T>(AdjustPci(pci), singleton);
  Publish(tPortDataPtr<T>(&(singleton)));
}

template<typename T>
tPortCreationInfo tSingletonPort<T>::AdjustPci(tPortCreationInfo pci)
{
  pci.send_buffer_size = 1;
  pci.alt_send_buffer_size = 0;
  pci.SetFlag(tPortFlags::cPUSH_DATA_IMMEDIATELY | tPortFlags::cNON_STANDARD_ASSIGN, true);
  return ::finroc::core::tPort::ProcessPci(pci);
}

template<typename T>
tSingletonPort<T>::tPortImpl<T>::tPortImpl(tPortCreationInfo pci, T& singleton) :
    tPortBase(pci),
    singleton_value(singleton)
{
}

template<typename T>
void tSingletonPort<T>::tPortImpl<T>::NonStandardAssign(tPublishCache& pc)
{
  if (pc.cur_ref->GetData() != &(singleton_value))
  {
    throw util::tRuntimeException("Cannot change contents of Singleton-Port", CODE_LOCATION_MACRO);
  }
}

} // namespace finroc
} // namespace core

