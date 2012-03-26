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
#include "core/port/tPortCreationInfoBase.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tPortCreationInfoBase::tPortCreationInfoBase() :
  default_value(),
  bounds(),
  name_set(false),
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&tUnit::cNO_UNIT),
  max_queue_size(16),
  flags(0),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  name(""),
  lock_order(-1)
{}

tPortCreationInfoBase tPortCreationInfoBase::LockOrderDerive(int lock_order) const
{
  tPortCreationInfoBase pci2(*this);
  pci2.lock_order = lock_order;
  return pci2;
}

void tPortCreationInfoBase::SetFlag(uint flag, bool value)
{
  if (value)
  {
    flags |= flag;
  }
  else
  {
    flags &= ~flag;
  }
}

void tPortCreationInfoBase::SetString(const util::tString& s)
{
  if (!name_set)
  {
    name = s;
    name_set = true;
  }
  else
  {
    config_entry = s;
  }
}

} // namespace finroc
} // namespace core

