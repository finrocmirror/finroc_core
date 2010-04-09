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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__TPORTCREATIONINFO_H
#define CORE__PORT__TPORTCREATIONINFO_H

#include "core/portdatabase/tDataType.h"
#include "core/datatype/tUnit.h"

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
class tPortCreationInfo : public util::tObject
{
public:

  /*! number of send buffers */
  int send_buffer_size, alt_send_buffer_size;

  /*! SI Unit of port. NULL for no unit = provides raw numbers */
  tUnit* unit;

  /*! Input Queue size; value <= 0 means flexible size */
  int max_queue_size;

  /*! Port flags */
  int flags;

  /*! Minimum Network update interval; value < 0 => default values */
  int16 min_net_update_interval;

  /*! Data type of port */
  tDataType* data_type;

  /*! Parent of port */
  tFrameworkElement* parent;

  /*!
   * Only relevant for Port sets: Does Port set manage ports?
   * Is port responsible itself for invalidating outdated port values? (usually not the case... only with some PortSets)
   */
  bool manages_ports;

  /*! Port name/description */
  util::tString description;

  /*! Default value of port */
  tPortCreationInfo(int flags_);

  tPortCreationInfo(const util::tString& description_, int flags_);

  tPortCreationInfo(const util::tString& description_, tDataType* data_type_, int flags_);

  tPortCreationInfo(const util::tString& description_, tFrameworkElement* parent_, tDataType* data_type_, int flags_);

  tPortCreationInfo(const util::tString& description_, tFrameworkElement* parent_, int flags_);

  tPortCreationInfo(const util::tString& description_, tFrameworkElement* parent_, int flags_, tUnit* unit_);

  tPortCreationInfo(tDataType* data_type_, int flags_);

  tPortCreationInfo(tDataType* data_type_, int flags_, bool manages_ports_);

  tPortCreationInfo(int flags_, tUnit* unit_);

  tPortCreationInfo(int flags_, bool manages_ports_);

  tPortCreationInfo();

  tPortCreationInfo(const util::tString& description_);

  tPortCreationInfo(tFrameworkElement* parent_);

  tPortCreationInfo(tFrameworkElement* parent_, tDataType* dt);

  tPortCreationInfo(const util::tString& description_, int flags_, int q_size);

  tPortCreationInfo(const util::tString& description_, tDataType* data_type_, int flags_, int q_size);

  // derive methods: Copy port creation info and change something

  tPortCreationInfo Derive(const util::tString& new_description);

  tPortCreationInfo Derive(const util::tString& new_description, tFrameworkElement* parent_);

  tPortCreationInfo Derive(const util::tString& new_description, tFrameworkElement* parent_, tDataType* type);

  tPortCreationInfo Derive(tDataType* type);

  tPortCreationInfo Derive(int flags_);

  inline bool GetFlag(int flag)
  {
    return (flags & flag) > 0;
  }

  void SetFlag(int flag, bool value);

  inline tPortCreationInfo SetManagesPort(bool manages_ports2)
  {
    manages_ports = manages_ports2;
    return *this;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__TPORTCREATIONINFO_H
