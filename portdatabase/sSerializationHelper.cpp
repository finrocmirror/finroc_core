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
#include "core/portdatabase/tDataType.h"
#include "core/portdatabase/sSerializationHelper.h"
#include "rrlib/finroc_core_utils/stream/tChunkedBuffer.h"
#include "core/buffers/tCoreOutput.h"
#include "core/buffers/tCoreInput.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/portdatabase/tTypedObjectImpl.h"
#include "core/port/tMultiTypePortDataBufferPool.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
void sSerializationHelper::Serialize2(tCoreOutput& os, const void* const port_data2, tDataType* type)
{
  os.Write(((char*)port_data2) + type->virtual_offset, type->sizeof_ - type->virtual_offset);
}
void sSerializationHelper::Deserialize2(tCoreInput& is, void* port_data2, tDataType* type)
{
  is.ReadFully(((char*)port_data2) + type->virtual_offset, type->sizeof_ - type->virtual_offset);
}

const char const_array_sSerializationHelper_0[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
::finroc::util::tArrayWrapper<char> sSerializationHelper::cTO_HEX(const_array_sSerializationHelper_0, 16);
::finroc::util::tArrayWrapper<int> sSerializationHelper::cTO_INT(256);

void sSerializationHelper::DeserializeFromHexString(tCoreSerializable* cs, const util::tString& s)
{
  util::tChunkedBuffer cb(false);
  tCoreOutput co(&(cb));
  if ((s.Length() % 2) != 0)
  {
    throw util::tException("not a valid hex string (should have even number of chars)");
  }
  for (size_t i = 0u; i < s.Length(); i++)
  {
    uint c1 = s.CharAt(i);
    i++;
    uint c2 = s.CharAt(i);
    if (cTO_INT[c1] < 0 || cTO_INT[c2] < 0)
    {
      throw util::tException(util::tStringBuilder("invalid hex chars: ") + c1 + c2);
    }
    int b = (cTO_INT[c1] << 4) | cTO_INT[c2];
    co.WriteByte(static_cast<int8>(b));
  }
  co.Close();
  tCoreInput ci(&(cb));
  cs->Deserialize(ci);
  ci.Close();
}

tDataType* sSerializationHelper::GetTypedStringDataType(tDataType* expected, const util::tString& s)
{
  if (s.StartsWith("\\("))
  {
    util::tString st = s.Substring(2, s.IndexOf(")"));
    tDataType* dt = tDataTypeRegister::GetInstance()->GetDataType(st);
    return dt;
  }
  return expected;
}

util::tString sSerializationHelper::SerializeToHexString(const tCoreSerializable* cs)
{
  util::tChunkedBuffer cb(false);
  tCoreOutput co(&(cb));
  cs->Serialize(co);
  co.Close();
  util::tStringBuilder sb((cb.GetCurrentSize() * 2) + 1);
  tCoreInput ci(&(cb));
  while (ci.MoreDataAvailable())
  {
    uint8_t b = ci.ReadByte();
    uint b1 = b >> 4u;
    uint b2 = b & 0xFu;
    sb.Append(cTO_HEX[b1]);
    sb.Append(cTO_HEX[b2]);
  }
  ci.Close();
  return sb.ToString();
}

void sSerializationHelper::StaticInit()
{
  for (size_t i = 0u; i < cTO_INT.length; i++)
  {
    cTO_INT[i] = -1;
  }
  cTO_INT['0'] = 0;
  cTO_INT['1'] = 1;
  cTO_INT['2'] = 2;
  cTO_INT['3'] = 3;
  cTO_INT['4'] = 4;
  cTO_INT['5'] = 5;
  cTO_INT['6'] = 6;
  cTO_INT['7'] = 7;
  cTO_INT['8'] = 8;
  cTO_INT['9'] = 9;
  cTO_INT['A'] = 0xA;
  cTO_INT['B'] = 0xB;
  cTO_INT['C'] = 0xC;
  cTO_INT['D'] = 0xD;
  cTO_INT['E'] = 0xE;
  cTO_INT['F'] = 0xF;
  cTO_INT['a'] = 0xA;
  cTO_INT['b'] = 0xB;
  cTO_INT['c'] = 0xC;
  cTO_INT['d'] = 0xD;
  cTO_INT['e'] = 0xE;
  cTO_INT['f'] = 0xF;
}

void sSerializationHelper::TypedStringDeserialize(tCoreSerializable* cs, const util::tString& s)
{
  util::tString s2 = s;
  if (s2.StartsWith("\\("))
  {
    s2 = s2.Substring(s2.IndexOf(")") + 1);
  }
  cs->Deserialize(s2);
}

tTypedObject* sSerializationHelper::TypedStringDeserialize(tDataType* expected, tMultiTypePortDataBufferPool* buffer_pool, const util::tString& s)
{
  tDataType* type = GetTypedStringDataType(expected, s);
  util::tString s2 = s;
  if (s2.StartsWith("\\("))
  {
    s2 = s2.Substring(s2.IndexOf(")") + 1);
  }

  if (buffer_pool == NULL && type->IsStdType())    // skip object?
  {
    //toSkipTarget();
    throw util::tRuntimeException(util::tStringBuilder("Buffer source does not support type ") + type->GetName(), CODE_LOCATION_MACRO);
    //return null;
  }
  else
  {
    tTypedObject* buffer = type->IsStdType() ? static_cast<tTypedObject*>(buffer_pool->GetUnusedBuffer(type)) : static_cast<tTypedObject*>(tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(type));
    buffer->Deserialize(s2);
    return buffer;
  }
}

util::tString sSerializationHelper::TypedStringSerialize(tDataType* expected, tTypedObject* cs)
{
  util::tString s = cs->Serialize();
  if (expected != cs->GetType())
  {
    return util::tStringBuilder("\\(") + (*cs->GetType()) + ")" + s;
  }
  if (s.StartsWith("\\"))
  {
    return util::tStringBuilder("\\") + s;
  }
  return s;
}

} // namespace finroc
} // namespace core

