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

#ifndef core__datatype__tBounds_h__
#define core__datatype__tBounds_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Information about bounds used, for instance, in bounded port or numerical setting
 * (Not meant to be used as port data)
 */
template<typename T>
class tBounds
{
public:

  enum tOutOfBoundsAction { eNONE, eDISCARD, eADJUST_TO_RANGE, eAPPLY_DEFAULT };

private:

  /*! Minimum and maximum bounds - Double for simplicity & efficiency reasons */
  T min, max;

  /*!
   * Action to perform when value is out of range
   * Apply default value when value is out of range? (or rather adjust to minimum or maximum or discard value?)
   */
  tOutOfBoundsAction action;

  /*! Default value when value is out of bounds */
  T out_of_bounds_default;

public:

  /*! dummy constructor for no bounds */
  template < bool INIT0 = std::is_fundamental<T>::value >
  tBounds(typename std::enable_if<INIT0, void>::type* v = 0) :
    min(0),
    max(0),
    action(eNONE),
    out_of_bounds_default(0)
  {
  }

  template < bool INIT0 = std::is_fundamental<T>::value >
  tBounds(typename std::enable_if < !INIT0, void >::type* v = 0) :
    min(),
    max(),
    action(eNONE),
    out_of_bounds_default()
  {
  }

  /*!
   * \param min Minimum bound
   * \param max Maximum bound
   * \param adjust_to_range Adjust values lying outside to range (or rather discard them)?
   */
  tBounds(T min, T max, bool adjust_to_range = true) :
    min(min),
    max(max),
    action(adjust_to_range ? eADJUST_TO_RANGE : eDISCARD),
    out_of_bounds_default()
  {
  }

  /*!
   * \param min Minimum bound
   * \param max Maximum bound
   * \param out_of_bounds_default Default value when value is out of bounds
   */
  template <bool B = std::is_same<T, bool>::value>
  tBounds(T min, T max, typename std::enable_if < !B, T >::type out_of_bounds_default) :
    min(min),
    max(max),
    action(eAPPLY_DEFAULT),
    out_of_bounds_default()
  {
    this->out_of_bounds_default = out_of_bounds_default;
  }

  /*!
   * \return Adjust value to range?
   */
  inline bool AdjustToRange() const
  {
    return action == eADJUST_TO_RANGE;
  }

  /*!
   * \return Adjust value to range?
   */
  inline bool ApplyDefault() const
  {
    return action == eAPPLY_DEFAULT;
  }

  /*!
   * \return Discard values which are out of bounds?
   */
  inline bool Discard() const
  {
    return action == eDISCARD;
  }

  /*!
   * \return Maximum value
   */
  inline T GetMax() const
  {
    return max;
  }

  /*!
   * \return Minimum value
   */
  inline T GetMin() const
  {
    return min;
  }

  /*!
   * \return Default value when value is out of bounds
   */
  inline tOutOfBoundsAction GetOutOfBoundsAction() const
  {
    return action;
  }

  /*!
   * \return Default value when value is out of bounds
   */
  inline const T GetOutOfBoundsDefault() const
  {
    return out_of_bounds_default;
  }

  /*!
   * Does value lie within bounds ?
   *
   * \param val Value
   * \return Answer
   */
  inline bool InBounds(T val) const
  {
    return (!(val < min)) && (!(max < val));
  }

  /*!
   * Sets bounds to new value
   *
   * \param new_bounds new value
   */
  inline void Set(const tBounds<T>& new_bounds)
  {
    action = new_bounds.action;
    max = new_bounds.max;
    min = new_bounds.min;
    out_of_bounds_default = new_bounds.out_of_bounds_default;
  }

  /*!
   * \param val Value to adjust to range
   * \return Adjusted value
   */
  inline T ToBounds(T val) const
  {
    if (val < min)
    {
      return min;
    }
    else if (max < val)
    {
      return max;
    }
    return val;
  }
};

template <typename T>
inline rrlib::serialization::tOutputStream& operator<<(rrlib::serialization::tOutputStream& stream, const tBounds<T>& bounds)
{
  stream << bounds.GetMin() << bounds.GetMax() << bounds.GetOutOfBoundsAction();
  if (bounds.GetOutOfBoundsAction() == tBounds<T>::eAPPLY_DEFAULT)
  {
    stream << bounds.GetOutOfBoundsDefault();
  }
  return stream;
}

template <typename T>
inline rrlib::serialization::tInputStream& operator>>(rrlib::serialization::tInputStream& stream, tBounds<T>& bounds)
{
  T min, max, ood;
  typename tBounds<T>::tOutOfBoundsAction action;
  stream >> min >> max >> action;
  if (action == tBounds<T>::eAPPLY_DEFAULT)
  {
    stream >> ood;
    bounds = tBounds<T>(min, max, ood);
  }
  else
  {
    bounds = tBounds<T>(min, max, action == tBounds<T>::eADJUST_TO_RANGE);
  }
  return stream;
}

} // namespace finroc
} // namespace core

#endif // core__datatype__tBounds_h__
