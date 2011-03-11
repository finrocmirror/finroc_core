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
#include "core/buffers/tCoreInput.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/net/tRemoteTypes.h"

namespace finroc
{
namespace core
{
tCoreInput::tCoreInput() :
    rrlib::serialization::tInputStream(),
    buffer_source(NULL),
    type_translation(NULL)
{
}

rrlib::serialization::tGenericObject* tCoreInput::ReadObject(bool in_inter_thread_container)
{
  //readSkipOffset();
  rrlib::serialization::tDataTypeBase dt = ReadType();
  if (dt == NULL)
  {
    return NULL;
  }
  if (buffer_source == NULL && tFinrocTypeInfo::IsStdType(dt))    // skip object?
  {
    //toSkipTarget();
    throw util::tRuntimeException(util::tStringBuilder("Buffer source does not support type ") + dt.GetName(), CODE_LOCATION_MACRO);
    //return null;
  }
  else
  {
    rrlib::serialization::tGenericObject* buffer = tFinrocTypeInfo::IsStdType(dt) ? static_cast<rrlib::serialization::tGenericObject*>(buffer_source->GetUnusedBufferRaw(dt)->GetObject()) : (in_inter_thread_container ? static_cast<rrlib::serialization::tGenericObject*>(tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(dt)->GetObject()) : static_cast<rrlib::serialization::tGenericObject*>(tThreadLocalCache::Get()->GetUnusedBuffer(dt)->GetObject()));
    buffer->Deserialize(*this);
    return buffer;
  }
}

rrlib::serialization::tDataTypeBase tCoreInput::ReadType()
{
  int16 type_uid = ReadShort();
  if (type_uid == -1)
  {
    return NULL;
  }
  rrlib::serialization::tDataTypeBase dt = type_translation == NULL ? rrlib::serialization::tDataTypeBase::GetType(type_uid) : type_translation->GetLocalType(type_uid);
  return dt;
}

} // namespace finroc
} // namespace core

