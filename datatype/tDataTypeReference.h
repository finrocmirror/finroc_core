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

#ifndef core__datatype__tDataTypeReference_h__
#define core__datatype__tDataTypeReference_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/datatype/tNumber.h"
#include "core/datatype/tCoreString.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Reference to data type (type doesn't need to exist in local runtime)
 */
class tDataTypeReference : public tCoreString
{
public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  tDataTypeReference() {}

  inline void DataType()
  {
    Set(tNumber::cTYPE);  // default is CoreNumber
  }

  /*!
   * \return Referenced data type - null if it doesn't exist in this runtime
   */
  inline rrlib::serialization::tDataTypeBase Get()
  {
    return rrlib::serialization::tDataTypeBase::FindType(GetBuffer().ToString());
  }

  /*!
   * \param dt new DataType to reference
   */
  inline void Set(rrlib::serialization::tDataTypeBase dt)
  {
    ::finroc::core::tCoreString::Set(dt.GetName());
  }

};

} // namespace finroc
} // namespace core

extern template class ::rrlib::serialization::tDataType<finroc::core::tDataTypeReference>;

#endif // core__datatype__tDataTypeReference_h__
