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

#ifndef CORE__PARAMETER__TBOOLSTRUCTUREPARAMETER_H
#define CORE__PARAMETER__TBOOLSTRUCTUREPARAMETER_H

#include "core/portdatabase/tDataTypeRegister.h"
#include "core/datatype/tCoreBoolean.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/parameter/tStructureParameter.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Boolean Structure parameter.
 */
class tBoolStructureParameter : public tStructureParameter<tCoreBoolean>
{
public:

  tBoolStructureParameter(const util::tString& name, bool default_value, bool constructor_prototype) :
      tStructureParameter<tCoreBoolean>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreBoolean>()), constructor_prototype, "")
  {
    if (!constructor_prototype)
    {
      Set(default_value);
    }
  }

  tBoolStructureParameter(const util::tString& name, bool default_value) :
      tStructureParameter<tCoreBoolean>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreBoolean>()), false, "")
  {
    // this(name,defaultValue,false);
    if (!false)
    {
      Set(default_value);
    }
  }

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tBoolStructureParameter(GetName(), false, false);
  }

  /*!
   * \return Current value
   */
  inline bool Get()
  {
    return ::finroc::core::tStructureParameter<tCoreBoolean>::GetValue()->Get();
  }

  /*!
   * \param new_value New Value
   */
  inline void Set(bool new_value)
  {
    ::finroc::core::tStructureParameter<tCoreBoolean>::GetValue()->Set(new_value);
  }

  /*!
   * Interprets/returns value in other (cloned) list
   *
   * \param list other list
   * \return Value in other list
   */
  /*public boolean interpretSpec(StructureParameterList list) {
      BoolStructureParameter param = (BoolStructureParameter)list.get(listIndex);
      assert(param.getType() == getType());
      return param.get();
  }*/

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TBOOLSTRUCTUREPARAMETER_H
