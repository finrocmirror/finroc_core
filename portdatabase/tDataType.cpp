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
#include "core/portdatabase/tDataType.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
const size_t tDataType::cMAX_CHEAP_COPYABLE_TYPES;
const size_t tDataType::cMAX_TYPES;

void tDataType::SetUpdateTime(int16 new_update_time)
{
  update_time = new_update_time;
  //RuntimeSettings.getInstance().getSharedPorts().publishUpdatedDataTypeInfo(this);
  tRuntimeSettings::GetInstance()->NotifyUpdateTimeChangeListener(this, new_update_time);
}

} // namespace finroc
} // namespace core

