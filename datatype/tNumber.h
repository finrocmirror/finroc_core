/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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

#ifndef core__datatype__tNumber_h__
#define core__datatype__tNumber_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/rtti.h"
#include "core/datatype/tUnit.h"
#include <endian.h>

namespace finroc
{
namespace core
{
class tConstant;

/*!
 * \author Max Reichardt
 *
 * This class stores numbers (with units) of different types.
 */
class tNumber
{
public:

  enum tType { eINT, eFLOAT, eDOUBLE, eCONSTANT };

private:

  template <typename T>
  friend struct tCoreNumberPointerGetter;

  template <typename T, size_t S>
  friend struct tCoreNumberPointerGetterBase;

  /*! Current numerical data */
  union
  {
    int64 ival;
    double dval;
    float fval;
  };

  /*! Current Type */
  tNumber::tType num_type;

  /*! Unit of data */
  union
  {
    tUnit* unit;         // if num_type != eCONSTANT;
    tConstant* constant; // if num_type == eCONSTANT;
  };

public:

  explicit operator bool ()
  {
    return (bool) this->Value <int>();
  }

  /*! Zero Constant */
  static tNumber cZERO;

  /*! Register Data type */
  static const rrlib::rtti::tDataTypeBase cTYPE;

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

  inline int8 PrepFirstByte(int8 value2) const
  {
    int tmp = (value2 << 1);
    return static_cast<int8>(((unit == &(tUnit::cNO_UNIT) || num_type == eCONSTANT) ? tmp : (tmp | 1)));
  }

public:

  // All kinds of variations of constructors

  tNumber(const tNumber& from)
  {
    unit = from.unit;
    num_type = from.num_type;
    ival = from.ival; // will copy any type of value
  }

  tNumber() :
    ival(0),
    num_type(eINT),
    unit(&tUnit::cNO_UNIT)
  {
  }

  /*! constructor for all integral types. */
  template<typename T>
  tNumber(T value, typename std::enable_if<std::is_integral<T>::value, tUnit>::type* unit = &tUnit::cNO_UNIT) :
    ival(value),
    num_type(eINT),
    unit(unit)
  {
  }

  tNumber(double value, tUnit* unit = &tUnit::cNO_UNIT) :
    dval(value),
    num_type(eDOUBLE),
    unit(unit)
  {
  }

  tNumber(float value, tUnit* unit = &tUnit::cNO_UNIT) :
    fval(value),
    num_type(eFLOAT),
    unit(unit)
  {
  }

  inline void CopyFrom(const tNumber& source)
  {
    num_type = source.num_type;
    unit = source.unit;
    ival = source.ival;
  }

  void Deserialize(rrlib::serialization::tInputStream& ois);

  void Deserialize(rrlib::serialization::tStringInputStream& is);

  inline double DoubleValue() const
  {
    return Value<double>();
  }

  bool Equals(const tNumber& other) const;

  inline float FloatValue() const
  {
    return Value<float>();
  }

  inline static rrlib::rtti::tDataTypeBase GetDataType()
  {
    assert((cTYPE != NULL));
    return cTYPE;
  }

  template <typename T>
  T* GetValuePtr();

  /*!
   * \return Unit of data
   */
  inline tUnit* GetUnit() const;

  inline int IntValue() const
  {
    return Value<int>();
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
    return num_type == eFLOAT || num_type == eDOUBLE;
  }

  inline bool IsInt(int i, tUnit* unit_) const
  {
    return num_type == eINT && ival == i && this->unit == unit_;
  }

  inline int64 LongValue() const
  {
    return Value<int64>();
  }

  void Serialize(rrlib::serialization::tOutputStream& oos) const;

  void Serialize(rrlib::serialization::tStringOutputStream& os) const
  {
    os.Append(ToString());
  }

  /*!
   * Changes unit
   *
   * \param unit2 new unit
   */
  inline void SetUnit(tUnit* unit2)
  {
    assert((unit2 != NULL));
    unit = unit2;
  }

  // All kinds of variations of setters
  template<typename T>
  inline void SetValue(T value, typename std::enable_if<std::is_integral<T>::value, tUnit>::type* unit = &tUnit::cNO_UNIT)
  {
    this->ival = value;
    this->unit = unit;
    num_type = eINT;
  }

  inline void SetValue(float value, tUnit* unit = &tUnit::cNO_UNIT)
  {
    this->fval = value;
    this->unit = unit;
    num_type = eFLOAT;
  }

  inline void SetValue(double value, tUnit* unit = &tUnit::cNO_UNIT)
  {
    this->dval = value;
    this->unit = unit;
    num_type = eDOUBLE;
  }

  void SetValue(tConstant* value);

  void SetValue(const tNumber& value)
  {
    this->unit = value.unit;
    this->num_type = value.num_type;
    this->ival = value.ival;
  }

  void SetValue(const tNumber& value, tUnit* u)
  {
    SetValue(value);
    SetUnit(u);
  }

  const util::tString ToString() const;

  // returns raw numeric value
  template <typename T>
  T Value() const;

  bool operator<(const tNumber& other) const;
};

template <typename T>
struct tCoreNumberPointerGetter
{
  static T* GetDataPtr(tNumber* num)
  {
    num->SetValue(num->Value<T>());
#if __BYTE_ORDER == __ORDER_BIG_ENDIAN
    assert(sizeof(ival) - sizeof(T) >= 0);
    char* tmp = (char*)(num->ival);
    tmp += (sizeof(ival) - sizeof(T));
    return (T*)tmp;
#else
    return (T*)(num->ival);
#endif
  }
};

template <>
struct tCoreNumberPointerGetter<float>
{
  static float* GetDataPtr(tNumber* num)
  {
    num->SetValue(num->Value<float>());
    return &num->fval;
  }
};

template <>
struct tCoreNumberPointerGetter<double>
{
  static double* GetDataPtr(tNumber* num)
  {
    num->SetValue(num->Value<double>());
    return &num->dval;
  }
};

// Get Pointer to current value (that can also be used to set value)
template <typename T>
T* tNumber::GetValuePtr()
{
  return tCoreNumberPointerGetter<T>::GetDataPtr(this);

};

inline rrlib::serialization::tOutputStream& operator<<(rrlib::serialization::tOutputStream& stream, const tNumber& n)
{
  n.Serialize(stream);
  return stream;
}

inline rrlib::serialization::tInputStream& operator>>(rrlib::serialization::tInputStream& stream, tNumber& n)
{
  n.Deserialize(stream);
  return stream;
}

inline rrlib::serialization::tStringOutputStream& operator<<(rrlib::serialization::tStringOutputStream& stream, const tNumber& n)
{
  n.Serialize(stream);
  return stream;
}

inline rrlib::serialization::tStringInputStream& operator>>(rrlib::serialization::tStringInputStream& stream, tNumber& n)
{
  n.Deserialize(stream);
  return stream;
}

} // namespace finroc
} // namespace core

#include "core/datatype/tConstant.h"

extern template class rrlib::rtti::tDataType<finroc::core::tNumber>;

namespace finroc
{
namespace core
{
tUnit* tNumber::GetUnit() const
{
  return num_type == eCONSTANT ? constant->GetValue().GetUnit() : unit;
}

template <typename T>
T tNumber::Value() const
{
  switch (num_type)
  {
  case eINT:
    return static_cast<T>(ival);
  case eDOUBLE:
    return static_cast<T>(dval);
  case eFLOAT:
    return static_cast<T>(fval);
  case eCONSTANT:
    return constant->Value<T>();
  default:
    assert(false && "Possibly not a Number at this memory address?");
    return 0;
  }
}

} // namespace finroc
} // namespace core

#endif // core__datatype__tNumber_h__
