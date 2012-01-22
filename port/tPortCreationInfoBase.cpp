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
tPortCreationInfoBase::tPortCreationInfoBase(uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  description(""),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(data_type_),
  parent(NULL),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, tFrameworkElement* parent_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(data_type_),
  parent(parent_),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, tFrameworkElement* parent_, uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(parent_),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const rrlib::serialization::tDataTypeBase& data_type_, uint flags_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(flags_),
  min_net_update_interval(-1),
  data_type(data_type_),
  parent(NULL),
  manages_ports(false),
  description(""),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase() :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(0),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  description(""),
  lock_order(-1)
{}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(16),
  flags(0),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, uint flags_, int q_size) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(q_size),
  flags(flags_ | tPortFlags::cHAS_QUEUE | tPortFlags::cUSES_QUEUE),
  min_net_update_interval(-1),
  data_type(NULL),
  parent(NULL),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase::tPortCreationInfoBase(const util::tString& description_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_, int q_size) :
  send_buffer_size(-1),
  alt_send_buffer_size(-1),
  unit(&(tUnit::cNO_UNIT)),
  max_queue_size(q_size),
  flags(flags_ | tPortFlags::cHAS_QUEUE | tPortFlags::cUSES_QUEUE),
  min_net_update_interval(-1),
  data_type(data_type_),
  parent(NULL),
  manages_ports(false),
  description(description_),
  lock_order(-1)
{
}

tPortCreationInfoBase tPortCreationInfoBase::Derive(const util::tString& new_description)
{
  tPortCreationInfoBase pci2(*this);
  pci2.description = new_description;
  return pci2;
}

tPortCreationInfoBase tPortCreationInfoBase::Derive(const util::tString& new_description, tFrameworkElement* parent_)
{
  tPortCreationInfoBase pci2(*this);
  pci2.description = new_description;
  pci2.parent = parent_;
  return pci2;
}

tPortCreationInfoBase tPortCreationInfoBase::Derive(const util::tString& new_description, tFrameworkElement* parent_, const rrlib::serialization::tDataTypeBase& type)
{
  tPortCreationInfoBase pci2(*this);
  pci2.description = new_description;
  pci2.parent = parent_;
  pci2.data_type = type;
  return pci2;
}

tPortCreationInfoBase tPortCreationInfoBase::Derive(const rrlib::serialization::tDataTypeBase& type)
{
  tPortCreationInfoBase pci2(*this);
  pci2.data_type = type;
  return pci2;
}

tPortCreationInfoBase tPortCreationInfoBase::Derive(uint flags_)
{
  tPortCreationInfoBase pci2(*this);
  pci2.flags = flags_;
  return pci2;
}

tPortCreationInfoBase tPortCreationInfoBase::LockOrderDerive(int lock_order_)
{
  tPortCreationInfoBase pci2(*this);
  pci2.lock_order = lock_order_;
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

} // namespace finroc
} // namespace core

