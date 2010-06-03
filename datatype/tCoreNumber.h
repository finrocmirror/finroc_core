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

#ifndef CORE__DATATYPE__TCORENUMBER_H
#define CORE__DATATYPE__TCORENUMBER_H

#include "core/datatype/tUnit.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tExpressData.h"
#include "core/portdatabase/tCopyable.h"
#include "core/portdatabase/tTypedObjectImpl.h"

namespace finroc
{
namespace core
{
class tConstant;
class tDataType;

/*!
 * \author Max Reichardt
 *
 * This class stores numbers (with units) of different types.
 */
class tCoreNumber : public util::tNumber, public tExpressData, public tCopyable<tCoreNumber>, public tTypedObject
{
public:

  enum tType { eINT, eLONG, eFLOAT, eDOUBLE, eCONSTANT };

private:

  /*! Current numerical data */
  union
  {
    int ival;
    int64 lval;
    double dval;
    float fval;
  };

  /*! Current Type */
  tCoreNumber::tType num_type;

  /*! Unit of data */
  tUnit* unit;

public:

  /*! Zero Constant */
  static tCoreNumber cZERO;

  /*! Register Data type */
  static tDataType* const cNUM_TYPE;

  /*@Override
  public short getUid() {
      return 8; //(short)serialVersionUID;
  //}*/

  // number serialization:
  // (1st type byte) - last bit unit
  // -64 = 8 Byte int
  // -63 = 4 Byte int
  // -62 = 2 Byte int
  // -61 = Double
  // -60 = Float
  // -59 = Constant
  // -58 to 63 absolute value
  static const int8 cINT64 = -64, cINT32 = -63, cINT16 = -62, cFLOAT64 = -61, cFLOAT32 = -60, cCONST = -59, cMIN_BARRIER = -58;

private:

  /*!
   * \return Current value Constant (only works if type is Type.CONSTANT)
   */
  tConstant* GetConstant() const;

  inline int8 PrepFirstByte(int8 value2) const
  {
    int tmp = (value2 << 1);
    return static_cast<int8>(((unit == &(tUnit::cNO_UNIT) || num_type == tCoreNumber::eCONSTANT) ? tmp : (tmp | 1)));
  }

public:

  // All kinds of variations of constructors

  tCoreNumber(const tCoreNumber& from)
  {
    unit = from.unit;
    type = from.type;
    lval = from.lval; // will copy any type of value
    type = cNUM_TYPE;
  }

  //  @InCppFile
  //  @InCpp("NUM_TYPE = DataTypeRegister::getInstance()->getDataTypeEntry<CoreNumber>();")
  //  public static void staticInit() {
  //      dataType = DataTypeRegister.getInstance().getDataTypeEntry(CoreNumber.class);
  //  }

  tCoreNumber() :
      lval(0),
      num_type(),
      unit(&tUnit::cNO_UNIT)
  {
    type = cNUM_TYPE;
  }

  tCoreNumber(int value_, tUnit* unit_ = &tUnit::cNO_UNIT) :
      ival(value_),
      num_type(eINT),
      unit(unit_)
  {
    type = cNUM_TYPE;
  }

  tCoreNumber(int64 value_, tUnit* unit_ = &tUnit::cNO_UNIT) :
      lval(value_),
      num_type(eLONG),
      unit(unit_)
  {
    type = cNUM_TYPE;
  }

  tCoreNumber(double value_, tUnit* unit_ = &tUnit::cNO_UNIT) :
      dval(value_),
      num_type(eDOUBLE),
      unit(unit_)
  {
    type = cNUM_TYPE;
  }

  tCoreNumber(float value_, tUnit* unit_ = &tUnit::cNO_UNIT) :
      fval(value_),
      num_type(eFLOAT),
      unit(unit_)
  {
    type = cNUM_TYPE;
  }

  tCoreNumber(const util::tNumber& value_, tUnit* unit_ = &tUnit::cNO_UNIT) :
      num_type(),
      unit(NULL)
  {
    type = cNUM_TYPE;
    SetValue(value_, unit_);
  }

  virtual void CopyFrom(const tCoreNumber& source);

  virtual void Deserialize(tCoreInput& ois);

  template <typename T>
  T TValue() const
  {
    switch (num_type)
    {
    case eINT:
      return static_cast<T>(ival);
    case eLONG:
      return static_cast<T>(lval);
    case eDOUBLE:
      return static_cast<T>(dval);
    case eFLOAT:
      return static_cast<T>(fval);
    case eCONSTANT:
      return static_cast<T>(unit->GetValue());
    default:
      assert(false);
      return 0;
    }
  }

  inline virtual double DoubleValue() const
  {
    return TValue<double>();
  }

  virtual bool Equals(const util::tObject& other) const;

  inline virtual float FloatValue() const
  {
    return TValue<float>();
  }

  inline static tDataType* GetDataType()
  {
    assert((cNUM_TYPE != NULL));
    return cNUM_TYPE;
  }

  /*!
   * \return Unit of data
   */
  tUnit* GetUnit() const;

  inline virtual int IntValue() const
  {
    return TValue<int>();
  }

  inline bool IsDouble(double i, tUnit* unit_) const
  {
    return num_type == eDOUBLE && dval == i && this->unit == unit_;
  }

  inline bool IsFloat(float i, tUnit* unit_) const
  {
    return num_type == eFLOAT && fval == i && this->unit == unit_;
  }

  /*!
   * \return Is this a double or float number?
   */
  inline bool IsFloatingPoint()
  {
    return num_type == tCoreNumber::eFLOAT || num_type == tCoreNumber::eDOUBLE;
  }

  inline bool IsInt(int i, tUnit* unit_) const
  {
    return num_type == eINT && ival == i && this->unit == unit_;
  }

  inline bool IsLong(int64 i, tUnit* unit_) const
  {
    return num_type == eLONG && lval == i && this->unit == unit_;
  }

  inline virtual int64 LongValue() const
  {
    return TValue<int64>();
  }

  virtual void Serialize(tCoreOutput& oos) const;

  // All kinds of variations of setters
  inline void SetValue(int value_)
  {
    SetValue(value_, &(tUnit::cNO_UNIT));
  }

  inline void SetValue(int value_, tUnit* unit_)
  {
    ival = value_;
    this->unit = unit_;
    num_type = eINT;
  }

  inline void SetValue(int64 value_)
  {
    SetValue(value_, &(tUnit::cNO_UNIT));
  }

  inline void SetValue(int64 value_, tUnit* unit_)
  {
    lval = value_;
    this->unit = unit_;
    num_type = eLONG;
  }

  inline void SetValue(float value_)
  {
    SetValue(value_, &(tUnit::cNO_UNIT));
  }

  inline void SetValue(float value_, tUnit* unit_)
  {
    fval = value_;
    this->unit = unit_;
    num_type = eFLOAT;
  }

  inline void SetValue(double value_)
  {
    SetValue(value_, &(tUnit::cNO_UNIT));
  }

  inline void SetValue(double value_, tUnit* unit_)
  {
    dval = value_;
    this->unit = unit_;
    num_type = eDOUBLE;
  }

  void SetValue(tConstant* value_);

  inline void SetValue(const util::tNumber& value_)
  {
    SetValue(value_, &(tUnit::cNO_UNIT));
  }

  void SetValue(const util::tNumber& value_, tUnit* unit_);

  void SetValue(const tCoreNumber& value_);

  virtual const util::tString ToString() const;

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TCORENUMBER_H
