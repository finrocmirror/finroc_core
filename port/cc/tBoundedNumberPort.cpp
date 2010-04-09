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
#include "core/port/cc/tBoundedNumberPort.h"
#include "core/datatype/tCoreNumber.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPortData.h"

namespace finroc
{
namespace core
{
tBoundedNumberPort::tBoundedNumberPort(tPortCreationInfo pci, tBounds b) :
    tNumberPort(ProcessPciBNP(pci)),
    bounds(b)
{
}

void tBoundedNumberPort::NonStandardAssign(tThreadLocalCache* tc)
{
  const tCoreNumber* cn = reinterpret_cast<tCoreNumber*>((tc->data->GetDataPtr()));
  double val = cn->DoubleValue();
  if (!bounds.InBounds(val))
  {
    if (bounds.Discard())
    {
      tc->ref = this->value;
      tc->data = tc->ref->GetContainer();
    }
    else if (bounds.AdjustToRange())
    {
      tCCPortDataContainer<>* container = ::finroc::core::tCCPortBase::GetUnusedBuffer(tc);
      tCoreNumber* cnc = reinterpret_cast<tCoreNumber*>(container->GetDataPtr());
      tc->data = container;
      tc->ref = container->GetCurrentRef();
      cnc->SetValue(bounds.ToBounds(val), cn->GetUnit());
    }
    else if (bounds.ApplyDefault())
    {
      tc->data = tc->GetUnusedBuffer(tCoreNumber::cNUM_TYPE);
      tc->ref = tc->data->GetCurrentRef();
      tc->data->Assign(reinterpret_cast<tCCPortData*>(bounds.GetOutOfBoundsDefault()));
      tc->data->SetRefCounter(0);  // locks will be added during assign
    }
  }
  //super.assign(tc); done anyway
}

} // namespace finroc
} // namespace core

