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

#ifndef core__parameter__tStructureParameterBool_h__
#define core__parameter__tStructureParameterBool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tStructureParameterBase.h"
#include "core/datatype/tBoolean.h"
#include "rrlib/serialization/tDataTypeBase.h"
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
class tStructureParameterBool : public tStructureParameter<tBoolean>
{
public:

  tStructureParameterBool(const util::tString& name, bool default_value, bool constructor_prototype) :
      tStructureParameter<tBoolean>(name, GetDataType(), constructor_prototype, "")
  {
    if (!constructor_prototype)
    {
      Set(default_value);
    }
  }

  tStructureParameterBool(const util::tString& name, bool default_value) :
      tStructureParameter<tBoolean>(name, GetDataType(), false, "")
  {
    // this(name,defaultValue,false);
    if (!false)
    {
      Set(default_value);
    }
  }

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterBool(GetName(), false, false);
  }

  /*!
   * \return Current value
   */
  inline bool Get()
  {
    return ::finroc::core::tStructureParameter<tBoolean>::GetValue()->Get();
  }

  /*! Helper to get this safely during static initialization */
  inline static rrlib::serialization::tDataTypeBase GetDataType()
  {
    return rrlib::serialization::tDataType<tBoolean>();
  }

  /*!
   * \param new_value New Value
   */
  inline void Set(bool new_value)
  {
    ::finroc::core::tStructureParameter<tBoolean>::GetValue()->Set(new_value);
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameterBool_h__
