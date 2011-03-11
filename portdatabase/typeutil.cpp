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

#include "core/portdatabase/typeutil.h"
#include "rrlib/serialization/tDataType.h"

#ifdef _LIB_RRLIB_UTIL_PRESENT_
#include "rrlib/util/tTime.h"
#endif

#ifdef _LIB_RRLIB_MATH_PRESENT_
#include "rrlib/math/tPose3D.h"
#include "rrlib/math/tPose2D.h"
#endif

namespace finroc
{
namespace core
{
namespace typeutil
{

template <typename T>
void InitCCType()
{
  tFinrocTypeInfo::Get(rrlib::serialization::tDataType<T>()).Init(tFinrocTypeInfo::eCC);
}

void InitCCTypes()
{

#ifdef _LIB_RRLIB_UTIL_PRESENT_
  InitCCType<rrlib::util::tTime>();
#endif

#ifdef _LIB_RRLIB_MATH_PRESENT_
  InitCCType<rrlib::math::tPose3D>();
  InitCCType<rrlib::math::tPose2D>();
#endif

}
}
} // namespace finroc
} // namespace core
