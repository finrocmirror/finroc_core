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
#include "core/datatype/tNumber.h"
#include "rrlib/rtti/tDataType.h"
#include "rrlib/serialization/serialization.h"
#include "core/datatype/tConstant.h"

namespace finroc
{
namespace core
{
tNumber tNumber::cZERO(0);
const rrlib::rtti::tDataTypeBase tNumber::cTYPE = rrlib::rtti::tDataType<tNumber>("Number");
const int8 tNumber::cINT64, tNumber::cINT32, tNumber::cINT16, tNumber::cFLOAT64, tNumber::cFLOAT32, tNumber::cCONST, tNumber::cMIN_BARRIER;

void tNumber::Deserialize(rrlib::serialization::tInputStream& ois)
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
    SetValue(ois.ReadInt());
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

void tNumber::Deserialize(rrlib::serialization::tStringInputStream& is)
{
  // scan for unit
  util::tString s = is.ReadWhile("-.", rrlib::serialization::tStringInputStream::cDIGIT | rrlib::serialization::tStringInputStream::cWHITESPACE | rrlib::serialization::tStringInputStream::cLETTER, true);
  util::tString num = s;
  for (size_t i = 0u; i < s.Length(); i++)
  {
    char c = s.CharAt(i);
    if (isalpha(c))
    {
      if ((c == 'e' || c == 'E') && (s.Length() > i + 1) && (c == '-' || isdigit(s.CharAt(i + 1))))
      {
        continue;  // exponent in decimal notation
      }
      num = s.Substring(0, i).Trim();
      util::tString unit_string = s.Substring(i).Trim();
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
    num_type = eINT;
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

bool tNumber::Equals(const tNumber& o) const
{
  return o.num_type == num_type && o.unit == unit && ival == o.ival;
}

void tNumber::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  if (num_type == eINT)
  {
    int64 value = ival;
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
  else if (num_type == eDOUBLE)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT64));

    oos.WriteDouble(dval);
  }
  else if (num_type == eFLOAT)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT32));

    oos.WriteFloat(fval);
  }
  else if (num_type == eCONSTANT)
  {
    oos.WriteByte(PrepFirstByte(cCONST));
    oos.WriteByte(constant->GetConstantId());
  }
  if (unit != &(tUnit::cNO_UNIT))
  {
    oos.WriteByte(unit->GetUid());
  }
}

void tNumber::SetValue(tConstant* value)
{
  this->constant = value;
  num_type = eCONSTANT;
}

const util::tString tNumber::ToString() const
{
  switch (num_type)
  {
  case eCONSTANT:
    return constant->ToString();
  case eINT:
    return util::tStringBuilder(ival) + unit->ToString();
  case eFLOAT:
    return util::tStringBuilder(fval) + unit->ToString();
  case eDOUBLE:
    return util::tStringBuilder(dval) + unit->ToString();

  default:
    return "Internal Error... shouldn't happen... whatever";
  }
}

bool tNumber::operator<(const tNumber& other) const
{
  if (unit != &(tUnit::cNO_UNIT) && other.unit != &(tUnit::cNO_UNIT))
  {
    double o = other.unit->ConvertTo(other.dval, unit);
    return o < dval;
  }
  switch (num_type)
  {
  case eINT:
    return ival < other.Value<int64_t>();
  case eDOUBLE:
    return dval < other.Value<double>();
  case eFLOAT:
    return fval < other.Value<float>();
  case eCONSTANT:
    return constant->GetValue() < other;
  default:
    assert(false && "Possibly not a Number at this memory address?");
    return 0;
  }
}


} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tNumber>;
