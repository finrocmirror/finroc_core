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

#ifndef core__port__tPortCreationInfoBase_h__
#define core__port__tPortCreationInfoBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/datatype/tUnit.h"
#include "rrlib/serialization/tDataTypeBase.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * This class contains various information for the creation of ports.
 * If ports require parameters in their constructor, they should take an
 * instance of this class.
 * This way, only one constructor is needed per Port class.
 */
class tPortCreationInfoBase
{
public:

  /*! number of send buffers */
  int send_buffer_size, alt_send_buffer_size;

  /*! SI Unit of port. NULL for no unit = provides raw numbers */
  tUnit* unit;

  /*! Input Queue size; value <= 0 means flexible size */
  int max_queue_size;

  /*! Port flags */
  unsigned int flags;

  /*! Minimum Network update interval; value < 0 => default values */
  int16 min_net_update_interval;

  /*! Data type of port */
  rrlib::serialization::tDataTypeBase data_type;

  /*! Parent of port */
  tFrameworkElement* parent;

  /*!
   * Only relevant for Port sets: Does Port set manage ports?
   * Is port responsible itself for invalidating outdated port values? (usually not the case... only with some PortSets)
   */
  bool manages_ports;

  /*! Port name/description */
  util::tString description;

  /*! Lock order */
  int lock_order;

  /*! config entry in config file */
  util::tString config_entry;

  /*! Default value of port */
  tPortCreationInfoBase(uint flags_);

  tPortCreationInfoBase(const util::tString& description_, uint flags_);

  tPortCreationInfoBase(const util::tString& description_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_);

  tPortCreationInfoBase(const util::tString& description_, tFrameworkElement* parent_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_);

  tPortCreationInfoBase(const util::tString& description_, tFrameworkElement* parent_, uint flags_);

  tPortCreationInfoBase(const rrlib::serialization::tDataTypeBase& data_type_, uint flags_);

  tPortCreationInfoBase();

  tPortCreationInfoBase(const util::tString& description_);

  tPortCreationInfoBase(const util::tString& description_, uint flags_, int q_size);

  tPortCreationInfoBase(const util::tString& description_, const rrlib::serialization::tDataTypeBase& data_type_, uint flags_, int q_size);

  // derive methods: Copy port creation info and change something

  tPortCreationInfoBase Derive(const util::tString& new_description);

  tPortCreationInfoBase Derive(const util::tString& new_description, tFrameworkElement* parent_);

  tPortCreationInfoBase Derive(const util::tString& new_description, tFrameworkElement* parent_, const rrlib::serialization::tDataTypeBase& type);

  tPortCreationInfoBase Derive(const rrlib::serialization::tDataTypeBase& type);

  tPortCreationInfoBase Derive(uint flags_);

  inline bool GetFlag(uint flag)
  {
    return (flags & flag) > 0;
  }

  tPortCreationInfoBase LockOrderDerive(int lock_order_);

  void SetFlag(uint flag, bool value);

  inline tPortCreationInfoBase SetManagesPort(bool manages_ports2)
  {
    manages_ports = manages_ports2;
    return *this;
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortCreationInfoBase_h__
