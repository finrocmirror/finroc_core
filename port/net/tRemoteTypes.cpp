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
#include "core/port/net/tRemoteTypes.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
tRemoteTypes::tRemoteTypes() :
    types(NULL),
    global_default(0)
{
}

void tRemoteTypes::Deserialize(tCoreInput* ci)
{
  assert(((!Initialized())) && "Already initialized");
  global_default = ci->ReadShort();
  types = new ::finroc::util::tArrayWrapper<tEntry>(ci->ReadShort());
  int16 next = ci->ReadShort();
  util::tSystem::out.Println("Connection Partner knows types:");
  while (next != -1)
  {
    int16 time = ci->ReadShort();
    util::tString name = ci->ReadString();
    tDataType* local = tDataTypeRegister::GetInstance()->GetDataType(name);
    util::tSystem::out.Println(util::tStringBuilder("- ") + name + " (" + next + ") - " + (local != NULL ? "available here, too" : "not available here"));
    tEntry e(time, local);
    (*(types))[next] = e;

    next = ci->ReadShort();
  }
}

tDataType* tRemoteTypes::GetLocalType(int16 uid)
{
  assert(((Initialized())) && "Not initialized");
  tEntry e = (*(types))[uid];
  if (e == NULL)
  {
    util::tSystem::out.Println(util::tStringBuilder("RemoteTypes: Unknown type ") + uid);
    return NULL;
  }
  return e.local_data_type;
}

void tRemoteTypes::SerializeLocalDataTypes(tDataTypeRegister* dtr, tCoreOutput* co)
{
  co->WriteShort(static_cast<int16>(tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME->Get()));
  co->WriteShort(dtr->GetMaxTypeIndex());
  for (int16 i = 0, n = static_cast<int16>(dtr->GetMaxTypeIndex()); i < n; i++)
  {
    tDataType* dt = dtr->GetDataType(i);
    if (dt != NULL)
    {
      co->WriteShort(dt->GetUid());
      co->WriteShort(dt->GetUpdateTime());
      co->WriteString(dt->GetName());
    }
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

tRemoteTypes::tEntry::tEntry(int16 time, tDataType* local) :
    update_time(time),
    local_data_type(local)
{
}

} // namespace finroc
} // namespace core

