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

namespace finroc
{
namespace core
{
template<typename T>
tStructureParameter<T>::tStructureParameter(const util::tString& name, tDataType* type, bool constructor_prototype) :
    tStructureParameterBase(name, type, constructor_prototype)
{
}

template<typename T>
tStructureParameter<T>::tStructureParameter(const util::tString& name, tDataType* type, bool constructor_prototype, const util::tString& default_value) :
    tStructureParameterBase(name, type, constructor_prototype)
{
  util::tString dv = default_value;
  if ((!constructor_prototype) && dv.Length() > 0)
  {
    try
    {
      Set(dv);
    }
    catch (const util::tException& e)
    {
      throw util::tRuntimeException(e, CODE_LOCATION_MACRO);
    }
  }
}

template<typename T>
tStructureParameter<T>::tStructureParameter(const util::tString& name, tDataType* type, const util::tString& default_value) :
    tStructureParameterBase(name, type, false)
{
  // this(name,type,false,defaultValue);
  util::tString dv = default_value;
  if ((!false) && dv.Length() > 0)
  {
    try
    {
      Set(dv);
    }
    catch (const util::tException& e)
    {
      throw util::tRuntimeException(e, CODE_LOCATION_MACRO);
    }
  }
}

template<typename T>
tStructureParameter<T>::tStructureParameter(const util::tString& name, tDataType* type) :
    tStructureParameterBase(name, type, false)
{
  // this(name,type,"");
  // this(name,type,false,defaultValue);
  util::tString dv = "";
  if ((!false) && dv.Length() > 0)
  {
    try
    {
      Set(dv);
    }
    catch (const util::tException& e)
    {
      throw util::tRuntimeException(e, CODE_LOCATION_MACRO);
    }
  }
}

} // namespace finroc
} // namespace core

