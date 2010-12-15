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
#include "core/portdatabase/tDataTypeRegister.h"

namespace finroc
{
namespace core
{
template<typename E>
tStructureParameterEnum<E>::tStructureParameterEnum(const util::tString& name, E default_value, bool constructor_prototype, ::std::tr1::shared_ptr<util::tSimpleList<util::tString> > string_constants_) :
    tStructureParameter<tEnumValue>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tEnumValue>()), constructor_prototype),
    string_constants(string_constants_)
{
  Set(default_value);
  GetValue()->SetStringConstants(this->string_constants.get());
}

template<typename E>
tStructureParameterEnum<E>::tStructureParameterEnum(const util::tString& name, E default_value, const util::tString& string_constants_) :
    tStructureParameter<tEnumValue>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tEnumValue>())),
    string_constants(new util::tSimpleList<util::tString>())
{
  this->string_constants->AddAll(string_constants_.Split(","));
  Set(default_value);
  GetValue()->SetStringConstants(this->string_constants.get());
}

template<typename E>
void tStructureParameterEnum<E>::Set(E default_value)
{
  int i = static_cast<int>(default_value);
  ::finroc::core::tStructureParameter<tEnumValue>::GetValue()->Set(i);
}

} // namespace finroc
} // namespace core

