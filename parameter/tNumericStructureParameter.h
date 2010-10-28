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

#ifndef CORE__PARAMETER__TNUMERICSTRUCTUREPARAMETER_H
#define CORE__PARAMETER__TNUMERICSTRUCTUREPARAMETER_H

#include "core/datatype/tUnit.h"
#include "core/datatype/tBounds.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/datatype/tCoreNumber.h"
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
class tNumericStructureParameter : public tStructureParameter<tCoreNumber>
{
private:

  /*! Unit of parameter */
  tUnit* unit;

  /*! Bounds of this parameter */
  tBounds bounds;

  /*! Default value */
  T default_val;

  /*!
   * \return CoreNumber buffer
   */
  inline tCoreNumber* GetBuffer()
  {
    return (reinterpret_cast<tCCInterThreadContainer<tCoreNumber>*>(this->cc_value))->GetData();
  }

  /*!
   * Adjust value to parameter constraints
   *
   * \param cCurrent CoreNumber buffer
   */
  void Set(tCoreNumber* cn);

public:

  tNumericStructureParameter(const util::tString& name, T default_value, bool constructor_prototype);

  tNumericStructureParameter(const util::tString& name, T default_value);

  tNumericStructureParameter(const util::tString& name, T default_value, bool constructor_prototype, tBounds bounds_);

  tNumericStructureParameter(const util::tString& name, T default_value, tBounds bounds2);

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tNumericStructureParameter<T>(GetName(), default_val, false, bounds);
  }

  /*!
   * (not real-time capable in Java)
   * \return Current value
   */
  inline T Get()
  {
    tCoreNumber* cn = GetBuffer();
    return cn->Value<T>();
  }

  /*!
   * \return Bounds of this paramete
   */
  inline tBounds GetBounds()
  {
    return bounds;
  }

  virtual void Set(const util::tString& new_value);

  /*!
   * \param new_value New Value
   */
  inline void Set(T new_value)
  {
    tCoreNumber cn(new_value);
    Set(&(cn));
  }

  /*!
   * Interprets/returns value in other (cloned) list
   *
   * \param list other list
   * \return Value in other list
   */
  /*@SuppressWarnings("unchecked")
  public T interpretSpec(StructureParameterList list) {
      NumericStructureParameter<T> param = (NumericStructureParameter<T>)list.get(listIndex);
      assert(param.getType() == getType());
      return param.get();
  }*/

};

} // namespace finroc
} // namespace core

#include "core/parameter/tNumericStructureParameter.hpp"

#endif // CORE__PARAMETER__TNUMERICSTRUCTUREPARAMETER_H
