/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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

#ifndef core__parameter__tStaticParameterImplNumeric_h__
#define core__parameter__tStaticParameterImplNumeric_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/datatype/tUnit.h"
#include "core/datatype/tBounds.h"
#include "core/parameter/tStaticParameterBase.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/rtti/rtti.h"
#include "core/datatype/tNumber.h"
#include "core/parameter/tStaticParameter.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Numeric static parameter implementation.
 */
template<typename T>
class tStaticParameterImplNumeric : public tStaticParameterImplStandard<tNumber>
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
    rrlib::rtti::tGenericObject* go = ValPointer();
    return go->GetData<tNumber>();
  }

  /*!
   * \param cn New Value
   */
  inline void Set(tNumber cn)
  {
    if (unit != &(tUnit::cNO_UNIT) && cn.GetUnit() != unit)
    {
      if (cn.GetUnit() == &(tUnit::cNO_UNIT))
      {
        cn.SetUnit(unit);
      }
      else
      {
        cn.SetValue(cn.GetUnit()->ConvertTo(cn.DoubleValue(), unit), unit);
      }
    }

    double val = cn.DoubleValue();
    if (!bounds.InBounds(val))
    {
      if (bounds.Discard())
      {
        return;
      }
      else if (bounds.AdjustToRange())
      {
        cn.SetValue(bounds.ToBounds(val), cn.GetUnit());
      }
      else if (bounds.ApplyDefault())
      {
        cn.SetValue(default_val, unit);
      }
    }
    GetBuffer()->SetValue(cn);
  }

public:

  tStaticParameterImplNumeric(const util::tString& name, T default_value = 0, tUnit* unit = &tUnit::cNO_UNIT, bool constructor_prototype = false) :
    tStaticParameterImplStandard<tNumber>(name, constructor_prototype),
    unit(unit),
    bounds(),
    default_val(default_value)
  {
    if (!constructor_prototype)
    {
      Set(default_value);
    }
  }

  tStaticParameterImplNumeric(const util::tString& name, T default_value, tBounds<T> bounds, tUnit* unit = &tUnit::cNO_UNIT, bool constructor_prototype = false) :
    tStaticParameterImplStandard<tNumber>(name, constructor_prototype),
    unit(unit),
    bounds(bounds),
    default_val(default_value)
  {
    if (!constructor_prototype)
    {
      Set(default_value);
    }
  }

  tStaticParameterImplNumeric(const tPortCreationInfo<T>& pci) :
    tStaticParameterImplStandard<tNumber>(pci),
    unit(pci.unit),
    bounds(pci.GetBounds()),
    default_val(pci.default_value_set ? *pci.GetDefault() : (typeid(T).name() == typeid(unsigned int).name() ? pci.flags : 0))
  {
    Set(default_val);
  }

  virtual ::finroc::core::tStaticParameterBase* DeepCopy()
  {
    return new tStaticParameterImplNumeric<T>(GetName(), default_val, bounds, unit, false);
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

  virtual void Set(const util::tString& new_value)
  {
    tNumber cn;
    rrlib::serialization::tStringInputStream sis(new_value);
    cn.Deserialize(sis);
    Set(cn);
  }

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

#endif // core__parameter__tStaticParameterImplNumeric_h__
