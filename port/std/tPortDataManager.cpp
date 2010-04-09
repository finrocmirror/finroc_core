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
#include "core/port/std/tPortDataImpl.h"

#include "core/port/std/tPortDataManager.h"
#include "core/port/std/tPortDataCreationInfo.h"

namespace finroc
{
namespace core
{
tPortDataManager tPortDataManager::cPROTOTYPE;
size_t tPortDataManager::cREF_COUNTERS_OFFSET = ((char*)&(tPortDataManager::cPROTOTYPE.ref_counters[0])) - ((char*)&(tPortDataManager::cPROTOTYPE));
tPortDataManager::~tPortDataManager()
{
  delete data;
  printf("Deleting Manager %p\n", this);
}

const size_t tPortDataManager::cNUMBER_OF_REFERENCES;
const size_t tPortDataManager::cREF_INDEX_MASK;

tPortDataManager::tPortDataManager(tDataType* dt, const tPortData* port_data) :
    ref_counters(),
    unused(true),
    data(NULL),
    reuse_counter(0)
{
  assert((port_data == NULL || port_data->GetType() == dt || dt->IsTransactionType()) && "Prototype needs same data type");

  //System.out.println("New port data manager");

  //ownerThread = ThreadUtil.getCurrentThreadId();

  //PortDataCreationInfo.get().setType(type) - wäre obsolet, wenn man Typ irgendwie anders bekommen könnte

  tPortDataCreationInfo* pdci = tPortDataCreationInfo::Get();
  pdci->SetManager(this);
  pdci->SetPrototype(port_data);

  data = (tPortData*)dt->CreateInstance();
  printf("Creating Manager %p\n", this);

  pdci->Reset();
  pdci->InitUnitializedObjects();
}

void tPortDataManager::DangerousDirectRecycle()
{
  data->HandleRecycle();
  reuse_counter++;
  ::finroc::util::tReusable::Recycle();
}

const util::tString tPortDataManager::ToString() const
{
  return util::tStringBuilder("PortDataManager for ") + (data != NULL ? data->ToString() : "null content") + " (Locks: " + GetCurrentRefCounter()->GetLocks() + ")";
}

} // namespace finroc
} // namespace core

