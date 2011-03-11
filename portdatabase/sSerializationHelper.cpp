/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "core/portdatabase/sSerializationHelper.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/tMultiTypePortDataBufferPool.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/sSerialization.h"

namespace finroc
{
namespace core
{
rrlib::serialization::tDataTypeBase sSerializationHelper::GetTypedStringDataType(const rrlib::serialization::tDataTypeBase& expected, const util::tString& s)
{
  if (s.StartsWith("\\("))
  {
    util::tString st = s.Substring(2, s.IndexOf(")"));
    rrlib::serialization::tDataTypeBase dt = rrlib::serialization::tDataTypeBase::FindType(st);
    return dt;
  }
  return expected;
}

void sSerializationHelper::TypedStringDeserialize(rrlib::serialization::tSerializable* cs, const util::tString& s)
{
  util::tString s2 = s;
  if (s2.StartsWith("\\("))
  {
    s2 = s2.Substring(s2.IndexOf(")") + 1);
  }
  rrlib::serialization::tStringInputStream sis(s2);
  cs->Deserialize(sis);
}

rrlib::serialization::tGenericObject* sSerializationHelper::TypedStringDeserialize(const rrlib::serialization::tDataTypeBase& expected, tMultiTypePortDataBufferPool* buffer_pool, const util::tString& s)
{
  rrlib::serialization::tDataTypeBase type = GetTypedStringDataType(expected, s);
  util::tString s2 = s;
  if (s2.StartsWith("\\("))
  {
    s2 = s2.Substring(s2.IndexOf(")") + 1);
  }

  if (buffer_pool == NULL && tFinrocTypeInfo::IsStdType(type))    // skip object?
  {
    //toSkipTarget();
    throw util::tRuntimeException(util::tStringBuilder("Buffer source does not support type ") + type.GetName(), CODE_LOCATION_MACRO);
    //return null;
  }
  else
  {
    rrlib::serialization::tGenericObject* buffer = tFinrocTypeInfo::IsStdType(type) ? buffer_pool->GetUnusedBuffer(type)->GetObject() : tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(type)->GetObject();
    rrlib::serialization::tStringInputStream sis(s2);
    buffer->Deserialize(sis);
    return buffer;
  }
}

util::tString sSerializationHelper::TypedStringSerialize(const rrlib::serialization::tDataTypeBase& expected, rrlib::serialization::tSerializable* cs, rrlib::serialization::tDataTypeBase cs_type)
{
  util::tString s = rrlib::serialization::sSerialization::Serialize(*cs);
  if (expected != cs_type)
  {
    return util::tStringBuilder("\\(") + cs_type.GetName() + ")" + s;
  }
  if (s.StartsWith("\\"))
  {
    return util::tStringBuilder("\\") + s;
  }
  return s;
}

} // namespace finroc
} // namespace core

