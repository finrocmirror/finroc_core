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
#include "core/portdatabase/tRPCInterfaceType.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "rrlib/finroc_core_utils/tAutoDeleter.h"

namespace finroc
{
namespace core
{
tRPCInterfaceType::tRPCInterfaceType(const util::tString& name, tPortInterface* methods) :
    rrlib::serialization::tDataTypeBase(GetDataTypeInfo(name))
{
  tFinrocTypeInfo::Get(*this).Init(methods);
}

rrlib::serialization::tDataTypeBase::tDataTypeInfoRaw* tRPCInterfaceType::GetDataTypeInfo(const util::tString& name)
{
  ::rrlib::serialization::tDataTypeBase dt = FindType(name);
  if (dt != NULL)
  {
    return const_cast<tDataTypeBase::tDataTypeInfoRaw*>(dt.GetInfo());
  }
  rrlib::serialization::tDataTypeBase::tDataTypeInfoRaw* info = util::tAutoDeleter::AddStatic(new rrlib::serialization::tDataTypeBase::tDataTypeInfoRaw());
  info->SetName(name);
  return info;
}

} // namespace finroc
} // namespace core

