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

#ifndef core__parameter__tStructureParameterNumeric_h__
#define core__parameter__tStructureParameterNumeric_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/datatype/tUnit.h"
#include "core/datatype/tBounds.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/serialization/tGenericObject.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/datatype/tNumber.h"
#include "core/parameter/tStructureParameter.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Numeric Structure parameter.
 */
template<typename T>
class tStructureParameterNumeric : public tStructureParameter<tNumber>
{
private:

  /*! Unit of parameter */
  tUnit* unit;

  /*! Bounds of this parameter */
  tBounds<T> bounds;

  /*! Default value */
  T default_val;

  /*!
   * \return CoreNumber buffer
   */
  inline tNumber* GetBuffer()
  {
    rrlib::serialization::tGenericObject* go = this->cc_value->GetObject();
    return go->GetData<tNumber>();
  }

  /*!
   * Adjust value to parameter constraints
   *
   * \param cCurrent CoreNumber buffer
   */
  void Set(tNumber cn);

public:

  tStructureParameterNumeric(const util::tString& name, T default_value, bool constructor_prototype);

  tStructureParameterNumeric(const util::tString& name, T default_value);

  tStructureParameterNumeric(const util::tString& name, T default_value, bool constructor_prototype, tBounds<T> bounds_);

  tStructureParameterNumeric(const util::tString& name, T default_value, tBounds<T> bounds2);

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterNumeric<T>(GetName(), default_val, false, bounds);
  }

  /*!
   * (not real-time capable in Java)
   * \return Current value
   */
  inline T Get()
  {
    tNumber* cn = GetBuffer();
    return cn->Value<T>();
  }

  /*!
   * \return Bounds of this parameter
   */
  inline tBounds<T> GetBounds()
  {
    return bounds;
  }

  /*! Helper to get this safely during static initialization */
  inline static rrlib::serialization::tDataTypeBase GetDataType()
  {
    return rrlib::serialization::tDataType<tNumber>();
  }

  virtual void Set(const util::tString& new_value);

  /*!
   * \param new_value New Value
   */
  inline void Set(T new_value)
  {
    tNumber cn(new_value);
    Set(cn);
  }

};

} // namespace finroc
} // namespace core

#include "core/parameter/tStructureParameterNumeric.hpp"

#endif // core__parameter__tStructureParameterNumeric_h__
