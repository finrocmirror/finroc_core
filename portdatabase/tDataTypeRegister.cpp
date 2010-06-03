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
#include "core/tRuntimeEnvironment.h"

#include "core/portdatabase/tDataTypeRegister.h"

namespace finroc
{
namespace core
{
tDataTypeRegister::~tDataTypeRegister()
{
  tRuntimeEnvironment::Shutdown();
  for (size_t i = 0; i < data_types.length; i++)
  {
    if (data_types[i] != NULL)
    {
      delete data_types[i];
    }
  }
}

tDataTypeRegister::tDataTypeRegister() :
    data_types(tDataType::cMAX_TYPES),
    next_data_type_uid(tDataType::cMAX_CHEAP_COPYABLE_TYPES),
    next_cCData_type_uid(1)
{
  //      // TODO init data types
  //
}

tDataType* tDataTypeRegister::AddDataType(tDataType* dt)
{
  assert((dt != NULL));
  int16 index = dt->IsCCType() ? next_cCData_type_uid++ : next_data_type_uid++;
  assert((data_types[index] == NULL));
  dt->SetUid(index);
  data_types[index] = dt;

  initial_lookup[dt->rtti_name] = dt;
  assert(initial_lookup[dt->rtti_name] == dt);
  return dt;
}

tDataType* tDataTypeRegister::GetDataType(const util::tString& name)
{
  for (int i = 0; i < GetMaxTypeIndex(); i++)
  {
    tDataType* dt = data_types[i];
    if (dt != NULL && dt->GetName().Equals(name))
    {
      return dt;
    }
  }
  return NULL;
}

} // namespace finroc
} // namespace core

