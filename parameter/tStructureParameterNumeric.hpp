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
#include "rrlib/serialization/tStringInputStream.h"

namespace finroc
{
namespace core
{
template<typename T>
tStructureParameterNumeric<T>::tStructureParameterNumeric(const util::tString& name, T default_value, bool constructor_prototype) :
    tStructureParameter<tNumber>(name, GetDataType(), constructor_prototype),
    unit(&(tUnit::cNO_UNIT)),
    bounds(tBounds<T>()),
    default_val(default_value)
{
  // this(name,defaultValue,constructorPrototype,new Bounds<T>());
  if (!constructor_prototype)
  {
    Set(default_value);
  }
}

template<typename T>
tStructureParameterNumeric<T>::tStructureParameterNumeric(const util::tString& name, T default_value) :
    tStructureParameter<tNumber>(name, GetDataType(), false),
    unit(&(tUnit::cNO_UNIT)),
    bounds(tBounds<T>()),
    default_val(default_value)
{
  // this(name,defaultValue,false,new Bounds<T>());
  if (!false)
  {
    Set(default_value);
  }
}

template<typename T>
tStructureParameterNumeric<T>::tStructureParameterNumeric(const util::tString& name, T default_value, bool constructor_prototype, tBounds<T> bounds_) :
    tStructureParameter<tNumber>(name, GetDataType(), constructor_prototype),
    unit(&(tUnit::cNO_UNIT)),
    bounds(bounds_),
    default_val(default_value)
{
  if (!constructor_prototype)
  {
    Set(default_value);
  }
}

template<typename T>
tStructureParameterNumeric<T>::tStructureParameterNumeric(const util::tString& name, T default_value, tBounds<T> bounds2) :
    tStructureParameter<tNumber>(name, GetDataType(), false),
    unit(&(tUnit::cNO_UNIT)),
    bounds(bounds2),
    default_val(default_value)
{
  // this(name,defaultValue,false,bounds2);
  if (!false)
  {
    Set(default_value);
  }
}

template<typename T>
void tStructureParameterNumeric<T>::Set(const util::tString& new_value)
{
  tNumber cn;
  rrlib::serialization::tStringInputStream sis(new_value);
  cn.Deserialize(sis);
  Set(cn);
}

template<typename T>
void tStructureParameterNumeric<T>::Set(tNumber cn)
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

} // namespace finroc
} // namespace core

