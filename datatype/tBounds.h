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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__DATATYPE__TBOUNDS_H
#define CORE__DATATYPE__TBOUNDS_H

#include "core/datatype/tCoreNumber.h"
#include "core/portdatabase/tCoreSerializableImpl.h"

namespace finroc
{
namespace core
{
class tDataType;
class tConstant;
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * Information about bounds used, for instance, in bounded port or numerical setting
 * (Not meant to be used as port data)
 */
class tBounds : public tCoreSerializable
{
public:

  enum tOutOfBoundsAction { eNONE, eDISCARD, eADJUST_TO_RANGE, eAPPLY_DEFAULT };

private:

  /*! Minimum and maximum bounds - Double for simplicity & efficiency reasons */
  double min, max;

  tBounds::tOutOfBoundsAction action;

  /*! Default value when value is out of bounds */
  tCoreNumber out_of_bounds_default;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  /*! dummy constructor for no bounds */
  tBounds();

  /*!
   * \param min Minimum bound
   * \param max Maximum bound
   * \param adjust_to_range Adjust values lying outside to range (or rather discard them)?
   */
  tBounds(double min_, double max_, bool adjust_to_range);

  /*!
   * \param min Minimum bound
   * \param max Maximum bound
   * \param out_of_bounds_default Default value when value is out of bounds
   */
  tBounds(double min_, double max_, tCoreNumber* out_of_bounds_default_);

  /*!
   * \param min Minimum bound
   * \param max Maximum bound
   * \param out_of_bounds_default Default value when value is out of bounds
   */
  tBounds(double min_, double max_, tConstant* out_of_bounds_default_);

  /*!
   * \return Adjust value to range?
   */
  inline bool AdjustToRange()
  {
    return action == tBounds::eADJUST_TO_RANGE;
  }

  /*!
   * \return Adjust value to range?
   */
  inline bool ApplyDefault()
  {
    return action == tBounds::eAPPLY_DEFAULT;
  }

  virtual void Deserialize(tCoreInput& is);

  /*!
   * \return Discard values which are out of bounds?
   */
  inline bool Discard()
  {
    return action == tBounds::eDISCARD;
  }

  /*!
   * \return Default value when value is out of bounds
   */
  inline tCoreNumber* GetOutOfBoundsDefault()
  {
    return &(out_of_bounds_default);
  }

  inline tDataType* GetType()
  {
    return cTYPE;
  }

  /*!
   * Does value lie within bounds ?
   *
   * \param val Value
   * \return Answer
   */
  inline bool InBounds(double val)
  {
    return val >= min && val <= max;
  }

  virtual void Serialize(tCoreOutput& os) const;

  /*!
   * Sets bounds to new value
   *
   * \param new_bounds new value
   */
  void Set(const tBounds& new_bounds);

  /*!
   * \param val Value to adjust to range
   * \return Adjusted value
   */
  double ToBounds(double val);

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TBOUNDS_H
