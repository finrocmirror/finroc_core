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

namespace finroc
{
namespace core
{

// diverse explicit template instantiations
template class tPort<int8_t>;
template class tPort<int16_t>;
template class tPort<int>;
template class tPort<long int>;
template class tPort<long long int>;
template class tPort<uint8_t>;
template class tPort<uint16_t>;
template class tPort<unsigned int>;
template class tPort<unsigned long int>;
template class tPort<unsigned long long int>;
template class tPort<float>;
template class tPort<double>;
template class tPort<tNumber>;
template class tPort<tCoreString>;
template class tPort<tBoolean>;
template class tPort<tEnumValue>;
template class tPort<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

