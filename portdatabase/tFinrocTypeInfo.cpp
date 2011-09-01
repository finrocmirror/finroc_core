/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
int tFinrocTypeInfo::initialized_types = 0;
const int tFinrocTypeInfo::cMAX_TYPES;
const int tFinrocTypeInfo::cMAX_CCTYPES;

int tFinrocTypeInfo::EstimateDataSize(rrlib::serialization::tGenericObject* data)
{
  if (IsCCType(data->GetType()))
  {
    return data->GetType().GetSize();
  }
  else
  {
    return 4096;  // very imprecise... but doesn't matter currently
  }
}

rrlib::serialization::tDataTypeBase tFinrocTypeInfo::GetFromCCIndex(int16 cc_type_index)
{
  for (int16 i = 0; i < cMAX_TYPES; i++)
  {
    if (Get(i).GetType() == eCC && Get(i).cc_index == cc_type_index)
    {
      return rrlib::serialization::tDataTypeBase::GetType(i);
    }
  }
  throw util::tRuntimeException("Type not found", CODE_LOCATION_MACRO);
}

tFinrocTypeInfo* tFinrocTypeInfo::InfoArray()
{
  static tFinrocTypeInfo info[cMAX_TYPES];
  return info;
}

void tFinrocTypeInfo::Init(tFinrocTypeInfo::tType type_)
{
  if (this->type != eSTD)
  {
    return;
  }
  this->type = type_;

  assert(type_ != eCC && "This is now done automatically");
}

void tFinrocTypeInfo::InitMoreTypes()
{
  static util::tAtomicInt last_cc_index(0);
  static util::tMutex mutex;
  util::tLock lock(mutex); // we need exclusive access on index variables
  for (; initialized_types < rrlib::serialization::tDataTypeBase::GetTypeCount(); initialized_types++)
  {
    tFinrocTypeInfo& finfo = InfoArray()[initialized_types];
    if (finfo.type == eSTD)
    {
      rrlib::serialization::tDataTypeBase dt = rrlib::serialization::tDataTypeBase::GetType((short)initialized_types);
      if (typeutil::IsCCType(dt))
      {
        finfo.type = eCC;
        if (dt.GetRttiName() == typeid(bool).name() ||
            ((dt.GetTypeTraits() & rrlib::serialization::trait_flags::cIS_INTEGRAL) == 0 &&
             (dt.GetTypeTraits() & rrlib::serialization::trait_flags::cIS_FLOATING_POINT) == 0))
        {
          // increase cc index if we have no numeric type
          finfo.cc_index = static_cast<int16>(last_cc_index.GetAndIncrement());
        }
      }
    }
  }
}

void tFinrocTypeInfo::SetUpdateTime(int16 new_update_time)
{
  update_time = new_update_time;
  //RuntimeSettings.getInstance().getSharedPorts().publishUpdatedDataTypeInfo(this);
  tRuntimeSettings::GetInstance()->NotifyUpdateTimeChangeListener(GetDataType(), new_update_time);
}

} // namespace finroc
} // namespace core

