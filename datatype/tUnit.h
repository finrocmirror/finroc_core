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

#ifndef core__datatype__tUnit_h__
#define core__datatype__tUnit_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"

namespace finroc
{
namespace core
{
class tNumber;

/*!
 * \author Max Reichardt
 *
 * Utility class for SI units and constants.
 *
 * Class should initialize cleanly in C++.
 * Can be initialized separately from rest of framework.
 */
class tUnit : public util::tUncopyableObject
{
private:

  /*! Factor regarding base unit */
  double factor;

  /*! Group of units that unit is in */
  const util::tSimpleList<tUnit*>& group;

  /*! Unit description */
  util::tString description;

  /*! index in unit group */
  int index;

  /*! last assigned unique id for encoding and decoding */
  static int8 uid_counter;

  /*! unique id for encoding and decoding */
  int8 uid;

  /*! factors for conversion to other units in group */
  std::shared_ptr< ::finroc::util::tArrayWrapper<double> > factors;

  /*! Is this class a constant? */
  bool is_aConstant;

  /*! temp list for uidLookupTable (see below) */
  static util::tSimpleList<tUnit*> uid_lookup_table_temp;

  /*! No Unit - has Uid 0 */
  static util::tSimpleList<tUnit*> unknown;

  /*! Length Units */
  static util::tSimpleList<tUnit*> length;

  /*! Speed Units */
  static util::tSimpleList<tUnit*> speed;

  /*! Weight Units */
  static util::tSimpleList<tUnit*> weight;

  /*! Time Units */
  static util::tSimpleList<tUnit*> time;

  /*! Angular Units */
  static util::tSimpleList<tUnit*> angle;

  /*! Frequency */
  static util::tSimpleList<tUnit*> frequency;

  /*! Screen Units */
  static util::tSimpleList<tUnit*> screen;

public:

  static tUnit cNO_UNIT;

  static tUnit nm;

  static tUnit um;

  static tUnit mm;

  static tUnit cm;

  static tUnit dm;

  static tUnit m;

  static tUnit km;

  static tUnit km_h;

  static tUnit m_s;

  static tUnit mg;

  static tUnit g;

  static tUnit kg;

  static tUnit t;

  static tUnit mt;

  static tUnit ns;

  static tUnit us;

  static tUnit ms;

  static tUnit s;

  static tUnit min;

  static tUnit h;

  static tUnit day;

  static tUnit deg;

  static tUnit rad;

  static tUnit Hz;

  static tUnit Pixel;

private:

  /*!
   * Standard constructor for units
   *
   * \param group Group of units that unit is in
   * \param description Unit description
   * \param factor Factor regarding base unit
   */
  tUnit(util::tSimpleList<tUnit*>& group_, const util::tString& description_, double factor_);

  /*!
   * Precalculate conversion factors
   *
   * \param units Group of Units
   */
  static void CalculateFactors(util::tSimpleList<tUnit*>& units);

protected:

  /*!
   * Constructor for constants
   *
   * \param description Constant description
   * \param u Unit of constant
   */
  tUnit(const util::tString& description_, tUnit* u);

public:

  /*!
   * Converts value from this unit to other unit.
   *
   * \param value Value
   * \param u Other Unit
   * \return Result
   */
  double ConvertTo(double value, tUnit* to_unit) const;

  /*!
   * Is Unit convertible to other Unit?
   *
   * \param u other Unit
   * \return True if it is convertible.
   */
  inline bool ConvertibleTo(tUnit* u) const
  {
    return group.Contains(u);
  }

  /*!
   * Get conversion factor from this unit to other unit
   *
   * \param u other Unit
   * \return Factor
   */
  double GetConversionFactor(tUnit* u) const;

  /*!
   * Get conversion factor from this unit to other unit
   * Fast version (no checks).
   *
   * \param u other Unit
   * \return Factor
   */
  inline double GetConversionFactorUnchecked(tUnit* u) const
  {
    return (*(factors))[u->index];
  }

  /*!
   * \return Unit's uid
   */
  inline int8 GetUid() const
  {
    return uid;
  }

  /*!
   * \param uid Unit's uid
   * \return Unit with this Uid
   */
  inline static tUnit* GetUnit(int8 uid_)
  {
    //return uidLookupTable[uid];
    return uid_lookup_table_temp.Get(uid_);
  }

  /*!
   * \param unit_string (Unique) Name of unit
   * \return Unit - NO_UNIT if unit name could not be found
   */
  static tUnit* GetUnit(const util::tString& unit_string);

  /*!
   * \return Value of constant - Double.NaN for normal units
   */
  virtual const tNumber& GetValue() const;

  /*!
   * \return Is this class a constant ?
   */
  inline bool IsConstant() const
  {
    return is_aConstant;
  }

  /*! table for looking up a Unit using its Uid */
  //private static final Unit[] uidLookupTable;

  /*!
   * Initialize factors
   * Should be called once, initially
   */
  static void StaticInit();

  virtual const util::tString ToString() const
  {
    return description;
  }

};

} // namespace finroc
} // namespace core

#endif // core__datatype__tUnit_h__
