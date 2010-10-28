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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TENUMSTRUCTUREPARAMETER_H
#define CORE__PARAMETER__TENUMSTRUCTUREPARAMETER_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/datatype/tEnumValue.h"
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
class tEnumStructureParameter : public tStructureParameter<tEnumValue>
{
private:

  /*! String constants for enum values */
  ::std::tr1::shared_ptr<util::tSimpleList<util::tString> > string_constants;

  /*!
   * \param i integer
   * \return Enum value for this integer
   */
  inline E GetValueForInt(int i)
  {
    return (E)i;
  }

public:

  tEnumStructureParameter(const util::tString& name, bool constructor_prototype = false) :
      tStructureParameter<tEnumValue>(name, tDataTypeRegister::GetInstance()->GetDataType<tEnumValue>(), constructor_prototype),
      string_constants()
  {}

  /*!
   * \param name Parameter name
   * \param default_value Default Value
   * \param string_constants String constants for enum values (comma-separated string)
   */
  tEnumStructureParameter(const util::tString& name, bool constructor_prototype, E default_value, ::std::tr1::shared_ptr<util::tSimpleList<util::tString> > string_constants_);

  /*!
   * \param name Parameter name
   * \param default_value Default Value
   * \param string_constants String constants for enum values (comma-separated string)
   */
  tEnumStructureParameter(const util::tString& name, E default_value, const util::tString& string_constants_);

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tEnumStructureParameter<E>(GetName(), false, GetValueForInt(0), string_constants);
  }

  /*!
   * \return Current value
   */
  inline E Get()
  {
    return GetValueForInt(::finroc::core::tStructureParameter<tEnumValue>::GetValue()->Get());
  }

  /*!
   * \param default_value new value
   */
  void Set(E default_value);

  /*!
   * Interprets/returns value in other (cloned) list
   *
   * \param list other list
   * \return Value in other list
   */
  /*@SuppressWarnings("unchecked")
  public E interpretSpec(StructureParameterList list) {
      EnumStructureParameter<E> param = (EnumStructureParameter<E>)list.get(listIndex);
      assert(param.getType() == getType());
      return param.get();
  }*/

};

} // namespace finroc
} // namespace core

#include "core/parameter/tEnumStructureParameter.hpp"

#endif // CORE__PARAMETER__TENUMSTRUCTUREPARAMETER_H
