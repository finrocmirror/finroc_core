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
#include "rrlib/serialization/tInputStream.h"
#include "core/parameter/tParameterNumeric.h"
#include "core/tRuntimeSettings.h"
#include "rrlib/serialization/tOutputStream.h"
#include "core/portdatabase/tFinrocTypeInfo.h"

namespace finroc
{
namespace core
{
tRemoteTypes::tRemoteTypes() :
    types(200u, 2u),
    update_times(200u, 2u),
    local_types_sent(0),
    global_default(0)
{
}

void tRemoteTypes::Deserialize(rrlib::serialization::tInputStream& ci)
{
  rrlib::logging::tLogStream ls = FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain);
  if (types.Size() == 0)
  {
    assert(((!Initialized())) && "Already initialized");
    global_default = ci.ReadShort();
    ls << "Connection Partner knows types:" << std::endl;
  }
  else
  {
    ls << "Connection Partner knows more types:" << std::endl;
  }
  int16 next = ci.ReadShort();
  while (next != -1)
  {
    int16 time = ci.ReadShort();

    ci.ReadByte();

    util::tString name = ci.ReadString();
    int16 checked_types = rrlib::serialization::tDataTypeBase::GetTypeCount();
    rrlib::serialization::tDataTypeBase local = rrlib::serialization::tDataTypeBase::FindType(name);
    ls << "- " << name << " (" << next << ") - " << (local != NULL ? "available here, too" : "not available here") << std::endl;
    int types_size = types.Size();  // to avoid warning
    assert((next == types_size));
    tEntry e(local);
    e.types_checked = checked_types;

    if (local == NULL)
    {
      e.name = name;
    }

    types.Add(e, true);
    if (local != NULL)
    {
      while (static_cast<int16>(update_times.Size()) < rrlib::serialization::tDataTypeBase::GetTypeCount())
      {
        update_times.Add(static_cast<int16>(-1), true);
      }
      update_times.GetIterable()->Set(local.GetUid(), time);
    }
    next = ci.ReadShort();
  }
  ;
}

int16 tRemoteTypes::GetTime(const rrlib::serialization::tDataTypeBase& data_type)
{
  assert(((Initialized())) && "Not initialized");
  while (static_cast<int16>(update_times.Size()) < data_type.GetUid())
  {
    update_times.Add(static_cast<int16>(-1), true);
  }
  return update_times.GetIterable()->Get(data_type.GetUid());
}

rrlib::serialization::tDataTypeBase tRemoteTypes::ReadType(rrlib::serialization::tInputStream& is)
{
  int16 uid = is.ReadShort();
  if (uid == -2)
  {
    // we get info on more data
    Deserialize(is);
    uid = is.ReadShort();
  }
  assert(((Initialized())) && "Not initialized");
  int types_size = types.Size();  // to avoid warning
  if (uid < 0 || uid >= types_size)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Corrupt type information from received from connection partner: ", uid);
    throw util::tRuntimeException("Corrupt type information from received from connection partner", CODE_LOCATION_MACRO);
  }

  tEntry& e = types.GetIterable()->Get(uid);
  if (e.local_data_type == NULL && e.types_checked < rrlib::serialization::tDataTypeBase::GetTypeCount())
  {
    // we have more types locally... maybe we can resolve missing types now
    e.types_checked = rrlib::serialization::tDataTypeBase::GetTypeCount();
    e.local_data_type = rrlib::serialization::tDataTypeBase::FindType(e.name);
  }
  return e.local_data_type;
}

void tRemoteTypes::SerializeLocalDataTypes(rrlib::serialization::tOutputStream& co)
{
  if (local_types_sent == 0)
  {
    int t = tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME->GetValue();
    co.WriteShort(static_cast<int16>(t));
  }
  int16 type_count = rrlib::serialization::tDataTypeBase::GetTypeCount();
  for (int16 i = local_types_sent, n = type_count; i < n; i++)
  {
    rrlib::serialization::tDataTypeBase dt = rrlib::serialization::tDataTypeBase::GetType(i);

    co.WriteShort(dt.GetUid());
    co.WriteShort(tFinrocTypeInfo::Get(i).GetUpdateTime());
    co.WriteByte(tFinrocTypeInfo::Get(i).GetType());
    co.WriteString(dt.GetName());
  }
  co.WriteShort(-1);  // terminator
  local_types_sent = type_count;
}

void tRemoteTypes::SetTime(rrlib::serialization::tDataTypeBase dt, int16 new_time)
{
  assert(((Initialized())) && "Not initialized");
  if (dt == NULL)
  {
    assert((new_time >= 0));
    global_default = new_time;
  }
  else
  {
    while (static_cast<int16>(update_times.Size()) < rrlib::serialization::tDataTypeBase::GetTypeCount())
    {
      update_times.Add(static_cast<int16>(-1), true);
    }
    update_times.GetIterable()->Set(dt.GetUid(), new_time);
  }
}

void tRemoteTypes::WriteType(rrlib::serialization::tOutputStream& os, rrlib::serialization::tDataTypeBase dt)
{
  int count = rrlib::serialization::tDataTypeBase::GetTypeCount();
  if (count > local_types_sent)
  {
    os.WriteShort(-2);
    SerializeLocalDataTypes(os);
  }
  os.WriteShort(dt.GetUid());
}

tRemoteTypes::tEntry::tEntry() :
    local_data_type(NULL),
    types_checked(0),
    name()
{}

tRemoteTypes::tEntry::tEntry(rrlib::serialization::tDataTypeBase local) :
    local_data_type(local),
    types_checked(0),
    name()
{
}

} // namespace finroc
} // namespace core

