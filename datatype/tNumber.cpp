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
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/buffers/tCoreInput.h"
#include "core/datatype/tConstant.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tNumber tNumber::cZERO(0);
tDataType* const tNumber::cTYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tNumber>(), "Number");
const int8 tNumber::cINT64, tNumber::cINT32, tNumber::cINT16, tNumber::cFLOAT64, tNumber::cFLOAT32, tNumber::cCONST, tNumber::cMIN_BARRIER;

void tNumber::CopyFrom(const tNumber& source)
{
  num_type = source.num_type;
  unit = source.unit;

  lval = source.lval;
}

void tNumber::Deserialize(tCoreInput& ois)
{
  int8 first_byte = ois.ReadByte();
  bool has_unit = (first_byte & 1) > 0;
  switch (first_byte >> 1)
  {
  case cINT64:
    SetValue(ois.ReadLong());
    break;
  case cFLOAT64:

    SetValue(ois.ReadDouble());
    break;
  case cINT32:
    SetValue(static_cast<int>(ois.ReadInt()));
    break;
  case cFLOAT32:

    SetValue(ois.ReadFloat());
    break;
  case cINT16:
    SetValue(static_cast<int>(ois.ReadShort()));
    break;
  case cCONST:
    SetValue(tConstant::GetConstant(ois.ReadByte()));
    break;
  default:
    SetValue(static_cast<int>(first_byte) >> 1);
    break;
  }
  unit = has_unit ? tUnit::GetUnit(ois.ReadByte()) : &(tUnit::cNO_UNIT);
}

void tNumber::Deserialize(const util::tString& s)
{
  // scan for unit
  util::tString num = s;
  for (size_t i = 0u; i < s.Length(); i++)
  {
    char c = s.CharAt(i);
    if (util::tCharacter::IsLetter(c))
    {
      if ((c == 'e' || c == 'E') && (s.Length() > i + 1) && (c == '-' || util::tCharacter::IsDigit(s.CharAt(i + 1))))
      {
        continue;  // exponent in decimal notation
      }
      num = s.Substring(0, i);
      util::tString unit_string = s.Substring(i);
      unit = tUnit::GetUnit(unit_string);
      break;
    }
  }
  if (num.Contains(".") || num.Contains("e") || num.Contains("E"))
  {
    try
    {
      SetValue(util::tDouble::ParseDouble(num), unit);
    }
    catch (const util::tException& e)
    {
      SetValue(0);
      throw util::tException(e);
    }
  }
  else
  {
    num_type = tNumber::eLONG;
    try
    {
      int64 l = util::tLong::ParseLong(num);
      if (l > (static_cast<int64>(util::tInteger::cMIN_VALUE)) && l < (static_cast<int64>(util::tInteger::cMAX_VALUE)))
      {
        SetValue(static_cast<int>(l), unit);
      }
      else
      {
        SetValue(l, unit);
      }
    }
    catch (const util::tException& e)
    {
      SetValue(0);
      throw util::tException(e);
    }
  }
}

bool tNumber::Equals(const util::tObject& other) const
{
  if (!(typeid(other) == typeid(tNumber)))
  {
    return false;
  }
  const tNumber& o = static_cast<const tNumber&>(other);
  bool value_matches = (lval == o.lval);
  return o.num_type == num_type && o.unit == unit && value_matches;
}

tConstant* tNumber::GetConstant() const
{
  return (static_cast<tConstant*>(unit));
}

tUnit* tNumber::GetUnit() const
{
  return num_type == tNumber::eCONSTANT ? GetConstant()->unit : unit;
}

void tNumber::Serialize(tCoreOutput& oos) const
{
  if (num_type == tNumber::eLONG || num_type == tNumber::eINT)
  {
    int64 value = (num_type == eLONG) ? lval : ival;
    if (value >= cMIN_BARRIER && value <= 63)
    {
      oos.WriteByte(PrepFirstByte(static_cast<int8>(value)));
    }
    else if (value >= util::tShort::cMIN_VALUE && value <= util::tShort::cMAX_VALUE)
    {
      oos.WriteByte(PrepFirstByte(cINT16));
      oos.WriteShort(static_cast<int16>(value));
    }
    else if (value >= util::tInteger::cMIN_VALUE && value <= util::tInteger::cMAX_VALUE)
    {
      oos.WriteByte(PrepFirstByte(cINT32));
      oos.WriteInt(static_cast<int>(value));
    }
    else
    {
      oos.WriteByte(PrepFirstByte(cINT64));
      oos.WriteLong(value);
    }
  }
  else if (num_type == tNumber::eDOUBLE)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT64));

    oos.WriteDouble(dval);
  }
  else if (num_type == tNumber::eFLOAT)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT32));

    oos.WriteFloat(fval);
  }
  else if (num_type == tNumber::eCONSTANT)
  {
    oos.WriteByte(PrepFirstByte(cCONST));
    oos.WriteByte((static_cast<tConstant*>(unit))->GetConstantId());
  }
}

void tNumber::SetValue(tConstant* value_)
{
  this->unit = value_;
  num_type = eCONSTANT;
}

void tNumber::SetValue(const util::tNumber& value_, tUnit* unit_)
{
  this->unit = unit_;
  if (typeid(value_) == typeid(int64))
  {
    SetValue(value_.LongValue());
  }
  else if (typeid(value_) == typeid(int))
  {
    SetValue(value_.IntValue());
  }
  else if (typeid(value_) == typeid(float))
  {
    SetValue(value_.FloatValue());
  }
  else if (typeid(value_) == typeid(tNumber))
  {
    SetValue(static_cast<const tNumber&>(value_));
  }
  else
  {
    SetValue(value_.DoubleValue());
  }
}

void tNumber::SetValue(const tNumber& value_)
{
  this->unit = value_.unit;
  this->num_type = value_.num_type;

  this->dval = value_.dval;
}

const util::tString tNumber::ToString() const
{
  switch (num_type)
  {
  case eCONSTANT:
    return GetConstant()->ToString();

  case eINT:
    return util::tStringBuilder(ival) + unit->ToString();
  case eLONG:
    return util::tStringBuilder(lval) + unit->ToString();
  case eFLOAT:
    return util::tStringBuilder(fval) + unit->ToString();
  case eDOUBLE:
    return util::tStringBuilder(dval) + unit->ToString();

  default:
    return "Internal Error... shouldn't happen... whatever";
  }
}

} // namespace finroc
} // namespace core
