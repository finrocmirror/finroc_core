/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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
#include "core/datatype/tNumber.h"

namespace finroc
{
namespace core
{
::finroc::util::tArrayWrapper<tConstant*> tConstant::constants(128);
util::tAtomicInt tConstant::constand_id_counter(0);
std::shared_ptr<tConstant> tConstant::cNO_MIN_TIME_LIMIT;
std::shared_ptr<tConstant> tConstant::cNO_MAX_TIME_LIMIT;

tConstant::tConstant(const util::tString& name, const tNumber& value_) :
  tUnit(name, value_.GetUnit()),
  constant_id(static_cast<int8>(constand_id_counter.GetAndIncrement())),
  value(value_),
  unit(value_.GetUnit())
{
  if (typeid(*unit) == typeid(tConstant))
  {
    throw util::tRuntimeException("Constants not allowed as unit", CODE_LOCATION_MACRO);
  }
  constants[constant_id] = this;
}

} // namespace finroc
} // namespace core

