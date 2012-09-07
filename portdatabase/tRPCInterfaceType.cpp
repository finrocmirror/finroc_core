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
#include "core/portdatabase/tPortFactory.h"
#include "core/port/rpc/tInterfacePort.h"

namespace finroc
{
namespace core
{

namespace internal
{

class tRPCPortFactory : public tPortFactory
{
  virtual tAbstractPort& CreatePort(const std::string& port_name, tFrameworkElement& parent, const rrlib::rtti::tDataTypeBase& dt, uint flags)
  {
    assert(tFinrocTypeInfo::IsMethodType(dt));
    const uint keep_flags = 0xFFFFFFFF & (~tPortFlags::cEMITS_DATA) & (~tPortFlags::cACCEPTS_DATA) & (~tPortFlags::cMAY_ACCEPT_REVERSE_DATA);
    return *(new tInterfacePort(port_name, &parent, dt, tInterfacePort::tType::ROUTING, flags & keep_flags));
  }
};

}

tRPCInterfaceType::tRPCInterfaceType(const util::tString& name, tPortInterface* methods) :
  rrlib::rtti::tDataTypeBase(GetDataTypeInfo(name))
{
  tFinrocTypeInfo::Get(*this).Init(methods);
  AddAnnotation<tPortFactory>(new internal::tRPCPortFactory());
}

rrlib::rtti::tDataTypeBase::tDataTypeInfoRaw* tRPCInterfaceType::GetDataTypeInfo(const util::tString& name)
{
  static std::vector<std::unique_ptr<rrlib::rtti::tDataTypeBase::tDataTypeInfoRaw>> auto_delete;
  ::rrlib::rtti::tDataTypeBase dt = FindType(name);
  if (dt != NULL)
  {
    return const_cast<tDataTypeBase::tDataTypeInfoRaw*>(dt.GetInfo());
  }
  rrlib::rtti::tDataTypeBase::tDataTypeInfoRaw* info = new rrlib::rtti::tDataTypeBase::tDataTypeInfoRaw();
  auto_delete.emplace_back(info);
  info->SetName(name);
  return info;
}

} // namespace finroc
} // namespace core

