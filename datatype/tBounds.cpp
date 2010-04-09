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
#include "core/datatype/tBounds.h"

namespace finroc
{
namespace core
{
tBounds::tBounds() :
    min(0),
    max(0),
    action(tBounds::eNONE),
    out_of_bounds_default()
{
}

tBounds::tBounds(double min_, double max_, bool adjust_to_range) :
    min(min_),
    max(max_),
    action(adjust_to_range ? tBounds::eADJUST_TO_RANGE : tBounds::eDISCARD),
    out_of_bounds_default()
{
}

tBounds::tBounds(double min_, double max_, tCoreNumber* out_of_bounds_default_) :
    min(min_),
    max(max_),
    action(tBounds::eAPPLY_DEFAULT),
    out_of_bounds_default()
{
  this->out_of_bounds_default.SetValue(*out_of_bounds_default_);
}

tBounds::tBounds(double min_, double max_, tConstant* out_of_bounds_default_) :
    min(min_),
    max(max_),
    action(tBounds::eAPPLY_DEFAULT),
    out_of_bounds_default()
{
  this->out_of_bounds_default.SetValue(out_of_bounds_default_);
}

double tBounds::ToBounds(double val)
{
  if (val < min)
  {
    return min;
  }
  else if (val > max)
  {
    return max;
  }
  return val;

}

} // namespace finroc
} // namespace core

