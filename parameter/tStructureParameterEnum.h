/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef core__parameter__tStructureParameterEnum_h__
#define core__parameter__tStructureParameterEnum_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/datatype/tEnumValue.h"
#include "rrlib/serialization/tDataType.h"
#include "core/parameter/tStructureParameter.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Enum structure parameter
 */
template<typename E>
class tStructureParameterEnum : public tStructureParameter<tEnumValue>
{
private:

  /*! String constants for enum values */
  std::shared_ptr<util::tSimpleList<util::tString> > string_constants;

  /*!
   * \param i integer
   * \return Enum value for this integer
   */
  inline E GetValueForInt(int i)
  {
    return (E)i;
  }

public:

  tStructureParameterEnum(const util::tString& name, bool constructor_prototype = false) :
      tStructureParameter<tEnumValue>(name, GetDataType(), constructor_prototype),
      string_constants()
  {}

  /*!
   * \param name Parameter name
   * \param default_value Default Value
   * \param string_constants String constants for enum values (comma-separated string)
   */
  tStructureParameterEnum(const util::tString& name, E default_value, bool constructor_prototype, std::shared_ptr<util::tSimpleList<util::tString> >& string_constants_);

  /*!
   * \param name Parameter name
   * \param default_value Default Value
   * \param string_constants String constants for enum values (comma-separated string)
   */
  tStructureParameterEnum(const util::tString& name, E default_value, const util::tString& string_constants_);

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterEnum<E>(GetName(), GetValueForInt(0), false, string_constants);
  }

  /*!
   * \return Current value
   */
  inline E Get()
  {
    return GetValueForInt(::finroc::core::tStructureParameter<tEnumValue>::GetValue()->Get());
  }

  /*! Helper to get this safely during static initialization */
  inline static rrlib::serialization::tDataType<tEnumValue> GetDataType()
  {
    return rrlib::serialization::tDataType<tEnumValue>();
  }

  /*!
   * \param default_value new value
   */
  void Set(E default_value);

};

} // namespace finroc
} // namespace core

#include "core/parameter/tStructureParameterEnum.hpp"

#endif // core__parameter__tStructureParameterEnum_h__
