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
#include "core/parameter/tParameter.h"

#include "core/port/net/tRemoteTypes.h"
#include "core/buffers/tCoreInput.h"
#include "core/parameter/tParameterNumeric.h"
#include "core/tRuntimeSettings.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tFinrocTypeInfo.h"

namespace finroc
{
namespace core
{
tRemoteTypes::tRemoteTypes() :
    types(NULL),
    types_by_local_uid(NULL),
    global_default(0)
{
}

void tRemoteTypes::Deserialize(tCoreInput* ci)
{
  assert(((!Initialized())) && "Already initialized");
  global_default = ci->ReadShort();
  types = new ::finroc::util::tArrayWrapper<tEntry>(ci->ReadShort());
  int max_types = types->length + rrlib::serialization::tDataTypeBase::GetTypeCount();
  types_by_local_uid = new ::finroc::util::tArrayWrapper<tEntry>(max_types);
  int16 next = ci->ReadShort();
  rrlib::logging::tLogStream ls = FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain);
  ls << "Connection Partner knows types:" << std::endl;
  while (next != -1)
  {
    int16 time = ci->ReadShort();
    util::tString name = ci->ReadString();
    rrlib::serialization::tDataTypeBase local = rrlib::serialization::tDataTypeBase::FindType(name);
    ls << "- " << name << " (" << next << ") - " << (local != NULL ? "available here, too" : "not available here") << std::endl;
    tEntry e(time, local);
    (*(types))[next] = e;

    if (local != NULL)
    {
      (*(types_by_local_uid))[local.GetUid()] = e;
    }
    next = ci->ReadShort();
  }
  ;
}

rrlib::serialization::tDataTypeBase tRemoteTypes::GetLocalType(int16 uid)
{
  assert(((Initialized())) && "Not initialized");
  tEntry e = (*(types))[uid];
  if (e == NULL)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "RemoteTypes: Unknown type ", uid);
    return NULL;
  }
  return e.local_data_type;
}

void tRemoteTypes::SerializeLocalDataTypes(tCoreOutput* co)
{
  int t = tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME->GetValue();
  co->WriteShort(static_cast<int16>(t));
  int16 type_count = rrlib::serialization::tDataTypeBase::GetTypeCount();
  co->WriteShort(type_count);
  for (int16 i = 0, n = type_count; i < n; i++)
  {
    rrlib::serialization::tDataTypeBase dt = rrlib::serialization::tDataTypeBase::GetType(i);
    co->WriteShort(dt.GetUid());
    co->WriteShort(tFinrocTypeInfo::Get(i).GetUpdateTime());
    co->WriteString(dt.GetName());
  }
  co->WriteShort(-1);  // terminator
}

void tRemoteTypes::SetTime(int16 type_uid, int16 new_time)
{
  assert(((Initialized())) && "Not initialized");
  if (type_uid < 0)
  {
    assert((new_time >= 0));
    global_default = new_time;
  }
  else
  {
    (*(types))[type_uid].update_time = new_time;
  }
}

tRemoteTypes::tEntry::tEntry() :
    update_time(-1),
    local_data_type(NULL)
{}

tRemoteTypes::tEntry::tEntry(int16 time, rrlib::serialization::tDataTypeBase local) :
    update_time(time),
    local_data_type(local)
{
}

} // namespace finroc
} // namespace core

