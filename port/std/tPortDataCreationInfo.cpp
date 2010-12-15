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
#include "core/port/std/tPortDataCreationInfo.h"
#include "core/port/std/tPortData.h"

namespace finroc
{
namespace core
{
util::tThreadLocal<tPortDataCreationInfo> tPortDataCreationInfo::info;

tPortDataCreationInfo* tPortDataCreationInfo::Get()
{
  tPortDataCreationInfo* result = info.Get();
  if (result == NULL)
  {
    result = new tPortDataCreationInfo();
    info.Set(result);
  }
  return result;
}

void tPortDataCreationInfo::InitUnitializedObjects()
{
  util::tLock lock1(this);
  tPortData* pd;
  while (uninitialized_port_data.Size() > 0)
  {
    pd = uninitialized_port_data.Remove(0);
    pd->InitDataType();
  }
}

void tPortDataCreationInfo::Reset()
{
  manager = NULL;
  prototype = NULL;
  // do no empty list with unitialized objects
}

} // namespace finroc
} // namespace core

