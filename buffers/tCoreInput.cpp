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
#include "core/portdatabase/tDataType.h"
#include "core/buffers/tCoreInput.h"
#include "core/portdatabase/tTypedObject.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tThreadLocalCache.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/port/net/tRemoteTypes.h"

namespace finroc
{
namespace core
{
tCoreInput::tCoreInput() :
    util::tInputStreamBuffer(),
    buffer_source(NULL),
    type_translation(NULL)
{
}

tCoreInput::tCoreInput(::std::tr1::shared_ptr<const util::tConstSource> source) :
    util::tInputStreamBuffer(source),
    buffer_source(NULL),
    type_translation(NULL)
{
}

tCoreInput::tCoreInput(const util::tConstSource* source) :
    util::tInputStreamBuffer(source),
    buffer_source(NULL),
    type_translation(NULL)
{
}

tCoreInput::tCoreInput(::std::tr1::shared_ptr<util::tSource> source) :
    util::tInputStreamBuffer(source),
    buffer_source(NULL),
    type_translation(NULL)
{
}

tCoreInput::tCoreInput(util::tSource* source) :
    util::tInputStreamBuffer(source),
    buffer_source(NULL),
    type_translation(NULL)
{
}

tTypedObject* tCoreInput::ReadObject(bool in_inter_thread_container)
{
  //readSkipOffset();
  tDataType* dt = ReadType();
  if (dt == NULL)
  {
    return NULL;
  }
  if (buffer_source == NULL && dt->IsStdType())    // skip object?
  {
    //toSkipTarget();
    throw util::tRuntimeException(util::tStringBuilder("Buffer source does not support type ") + dt->GetName(), CODE_LOCATION_MACRO);
    //return null;
  }
  else
  {
    tTypedObject* buffer = dt->IsStdType() ? static_cast<tTypedObject*>(buffer_source->GetUnusedBuffer(dt)) : (in_inter_thread_container ? static_cast<tTypedObject*>(tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(dt)) : static_cast<tTypedObject*>(tThreadLocalCache::Get()->GetUnusedBuffer(dt)));
    buffer->Deserialize(*this);
    return buffer;
  }
}

tDataType* tCoreInput::ReadType()
{
  int16 type_uid = ReadShort();
  if (type_uid == -1)
  {
    return NULL;
  }
  tDataType* dt = type_translation == NULL ? tDataTypeRegister::GetInstance()->GetDataType(type_uid) : type_translation->GetLocalType(type_uid);
  return dt;
}

} // namespace finroc
} // namespace core

