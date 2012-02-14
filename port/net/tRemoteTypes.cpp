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
#include "core/port/net/tNetworkSettings.h"
#include "rrlib/serialization/serialization.h"
#include "core/parameter/tParameterNumeric.h"
#include "core/tRuntimeSettings.h"
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
  std::stringstream ls;
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
    int16 checked_types = rrlib::rtti::tDataTypeBase::GetTypeCount();
    rrlib::rtti::tDataTypeBase local = rrlib::rtti::tDataTypeBase::FindType(name);
    ls << "- " << name << " (" << next << ") - " << (local != NULL  && (!tFinrocTypeInfo::IsUnknownType(local)) ? "available here, too" : "not available here") << std::endl;
    __attribute__((unused)) // prevents warning in release mode
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
      while (static_cast<int16>(update_times.Size()) < rrlib::rtti::tDataTypeBase::GetTypeCount())
      {
        update_times.Add(static_cast<int16>(-1), true);
      }
      update_times.GetIterable()->Set(local.GetUid(), time);
    }
    next = ci.ReadShort();
  }
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, ls.str());
}

int16 tRemoteTypes::GetTime(const rrlib::rtti::tDataTypeBase& data_type)
{
  assert(((Initialized())) && "Not initialized");
  while (static_cast<int16>(update_times.Size()) <= data_type.GetUid())
  {
    update_times.Add(static_cast<int16>(-1), true);
  }
  return update_times.GetIterable()->Get(data_type.GetUid());
}

rrlib::rtti::tDataTypeBase tRemoteTypes::ReadType(rrlib::serialization::tInputStream& is)
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
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Corrupt type information from received from connection partner: ", uid);
    throw util::tRuntimeException("Corrupt type information from received from connection partner", CODE_LOCATION_MACRO);
  }

  tEntry& e = types.GetIterable()->Get(uid);
  if (e.local_data_type == NULL && e.types_checked < rrlib::rtti::tDataTypeBase::GetTypeCount())
  {
    // we have more types locally... maybe we can resolve missing types now
    e.types_checked = rrlib::rtti::tDataTypeBase::GetTypeCount();
    e.local_data_type = rrlib::rtti::tDataTypeBase::FindType(e.name);
  }
  return e.local_data_type;
}

void tRemoteTypes::SerializeLocalDataTypes(rrlib::serialization::tOutputStream& co)
{
  if (local_types_sent == 0)
  {
    int t = tNetworkSettings::GetInstance().default_minimum_network_update_time.Get();
    co.WriteShort(static_cast<int16>(t));
  }
  int16 type_count = rrlib::rtti::tDataTypeBase::GetTypeCount();
  for (int16 i = local_types_sent, n = type_count; i < n; i++)
  {
    rrlib::rtti::tDataTypeBase dt = rrlib::rtti::tDataTypeBase::GetType(i);

    co.WriteShort(dt.GetUid());
    co.WriteShort(tFinrocTypeInfo::Get(i).GetUpdateTime());
    co.WriteByte(tFinrocTypeInfo::Get(i).GetType());
    co.WriteString(dt.GetName());
  }
  co.WriteShort(-1);  // terminator
  local_types_sent = type_count;
}

void tRemoteTypes::SetTime(rrlib::rtti::tDataTypeBase dt, int16 new_time)
{
  assert(((Initialized())) && "Not initialized");
  if (dt == NULL)
  {
    assert((new_time >= 0));
    global_default = new_time;
  }
  else
  {
    while (static_cast<int16>(update_times.Size()) < rrlib::rtti::tDataTypeBase::GetTypeCount())
    {
      update_times.Add(static_cast<int16>(-1), true);
    }
    update_times.GetIterable()->Set(dt.GetUid(), new_time);
  }
}

void tRemoteTypes::WriteType(rrlib::serialization::tOutputStream& os, rrlib::rtti::tDataTypeBase dt)
{
  int count = rrlib::rtti::tDataTypeBase::GetTypeCount();
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

tRemoteTypes::tEntry::tEntry(rrlib::rtti::tDataTypeBase local) :
  local_data_type(local),
  types_checked(0),
  name()
{
}

} // namespace finroc
} // namespace core

