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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__DATATYPE__TCONSTANT_H
#define CORE__DATATYPE__TCONSTANT_H

#include "core/datatype/tNumber.h"
#include "core/datatype/tUnit.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This class contains a set of constants.
 * A constant is derived from the Unit class.
 * This is not absolutely clean, but more efficient.
 */
class tConstant : public tUnit
{
private:

  /*! Lookup table for constants (id => Constant) */
  static ::finroc::util::tArrayWrapper<tConstant*> constants;

  /*! last assigned unique id for encoding and decoding */
  static util::tAtomicInt constand_id_counter;

  /*! unique id for encoding and decoding (needs to be changed to short if there are more than 128 constants) */
  int8 constant_id;

  /*! Value of constant */
  tNumber value;

public:

  /*! Number type of constant */
  //private CoreNumber2.Type type;

  /*! Constants */
  static ::std::tr1::shared_ptr<tConstant> cNO_MIN_TIME_LIMIT;

  static ::std::tr1::shared_ptr<tConstant> cNO_MAX_TIME_LIMIT;

  /*! Unit of constant */
  ::finroc::core::tUnit* unit;

private:

  /*!
   * \param name Name of constant;
   * \param value Value of constant;
   */
  tConstant(const util::tString& name, tNumber value_);

public:

  /*!
   * \param uid Uid of constant to retrieve
   * \return Constant
   */
  inline static tConstant* GetConstant(int8 uid)
  {
    return constants[uid];
  }

  /*!
   * \return Constant id (for encoding)
   */
  inline int8 GetConstantId()
  {
    return constant_id;
  }

  virtual const tNumber& GetValue() const
  {
    return value;
  }

  inline static void StaticInit()
  {
    cNO_MIN_TIME_LIMIT = ::std::tr1::shared_ptr<tConstant>(new tConstant("No Limit", tNumber(-1, &(::finroc::core::tUnit::ms))));
    cNO_MAX_TIME_LIMIT = ::std::tr1::shared_ptr<tConstant>(new tConstant("No Limit", tNumber(util::tInteger::cMAX_VALUE, &(::finroc::core::tUnit::ms))));
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TCONSTANT_H
