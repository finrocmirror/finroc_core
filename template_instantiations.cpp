/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#include "core/port/tPort.h"
#include "core/datatype/tBoolean.h"
#include "core/datatype/tEnumValue.h"
#include "core/parameter/tParameterNumeric.h"

namespace finroc
{
namespace core
{

// diverse explicit template instantiations
template class tPort<int>;
template class tPort<long long int>;
template class tPort<float>;
template class tPort<double>;
template class tPort<tNumber>;
template class tPort<tCoreString>;
template class tPort<tBoolean>;
template class tPort<tEnumValue>;
template class tPort<rrlib::serialization::tMemoryBuffer>;

template class tParameter<int>;
template class tParameter<long long int>;
template class tParameter<float>;
template class tParameter<double>;
template class tParameter<tNumber>;
template class tParameter<tCoreString>;
template class tParameter<tBoolean>;
template class tParameter<tEnumValue>;
template class tParameter<rrlib::serialization::tMemoryBuffer>;

template class tParameterNumeric<int>;
template class tParameterNumeric<long long int>;
template class tParameterNumeric<float>;
template class tParameterNumeric<double>;

} // namespace finroc
} // namespace core

