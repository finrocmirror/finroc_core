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
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/datatype/tConstant.h"

#include "core/datatype/tCoreNumber.h"
#include "core/portdatabase/tDataTypeRegister.h"

namespace finroc
{
namespace core
{
tCoreNumber tCoreNumber::cZERO(0);
tDataType* const tCoreNumber::cNUM_TYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreNumber>());
const int8 tCoreNumber::cINT64, tCoreNumber::cINT32, tCoreNumber::cINT16, tCoreNumber::cFLOAT64, tCoreNumber::cFLOAT32, tCoreNumber::cCONST, tCoreNumber::cMIN_BARRIER;

void tCoreNumber::CopyFrom(const tCoreNumber& source)
{
  num_type = source.num_type;
  unit = source.unit;

  lval = source.lval;
}

void tCoreNumber::Deserialize(tCoreInput& ois)
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

bool tCoreNumber::Equals(const util::tObject& other) const
{
  if (!(typeid(other) == typeid(tCoreNumber)))
  {
    return false;
  }
  const tCoreNumber& o = static_cast<const tCoreNumber&>(other);
  bool value_matches = (lval == o.lval);
  return o.num_type == num_type && o.unit == unit && value_matches;
}

tConstant* tCoreNumber::GetConstant() const
{
  return (static_cast<tConstant*>(unit));
}

tUnit* tCoreNumber::GetUnit() const
{
  return num_type == tCoreNumber::eCONSTANT ? GetConstant()->unit : unit;
}

void tCoreNumber::Serialize(tCoreOutput& oos) const
{
  if (num_type == tCoreNumber::eLONG || num_type == tCoreNumber::eINT)
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
  else if (num_type == tCoreNumber::eDOUBLE)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT64));

    oos.WriteDouble(dval);
  }
  else if (num_type == tCoreNumber::eFLOAT)
  {
    oos.WriteByte(PrepFirstByte(cFLOAT32));

    oos.WriteFloat(fval);
  }
  else if (num_type == tCoreNumber::eCONSTANT)
  {
    oos.WriteByte(PrepFirstByte(cCONST));
    oos.WriteByte((static_cast<tConstant*>(unit))->GetConstantId());
  }
}

void tCoreNumber::SetValue(tConstant* value_)
{
  this->unit = value_;
  num_type = eCONSTANT;
}

void tCoreNumber::SetValue(const util::tNumber& value_, tUnit* unit_)
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
  else if (typeid(value_) == typeid(tCoreNumber))
  {
    SetValue(static_cast<const tCoreNumber&>(value_));
  }
  else
  {
    SetValue(value_.DoubleValue());
  }
}

void tCoreNumber::SetValue(const tCoreNumber& value_)
{
  this->unit = value_.unit;
  this->num_type = value_.num_type;

  this->dval = value_.dval;
}

const util::tString tCoreNumber::ToString() const
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

