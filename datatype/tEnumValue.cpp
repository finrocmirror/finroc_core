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
#include "core/datatype/tEnumValue.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/portdatabase/tTypedObjectImpl.h"

namespace finroc
{
namespace core
{
tDataType* tEnumValue::cTYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tEnumValue>());

void tEnumValue::Deserialize(const util::tString& s)
{
  value = util::tInteger::ParseInt(s.Substring(0, s.IndexOf("|")));
}

void tEnumValue::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  // make as fault-tolerant as possible
  util::tString name = node.GetTextContent();
  value = GetStringAsValue(name);

  if (value == -1)
  {
    if (!node.HasAttribute("value"))
    {
      throw util::tException("Cannot deserialize enum value");
    }
    value = node.GetIntAttribute("value");
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot find enum value for string ", name, ". Relying on integer constant ", value, " instead.");
  }
}

int tEnumValue::GetStringAsValue(const util::tString& name)
{
  for (size_t i = 0u; i < string_constants->Size(); i++)
  {
    if (string_constants->Get(i).Equals(name))
    {
      return i;
    }
  }
  return -1;
}

util::tString tEnumValue::Serialize() const
{
  util::tStringBuilder sb;
  sb.Append(value).Append("|").Append(string_constants->Get(0));
  for (size_t i = 1u; i < string_constants->Size(); i++)
  {
    sb.Append(",");
    sb.Append(string_constants->Get(i));
  }
  return sb.ToString();
}

} // namespace finroc
} // namespace core

