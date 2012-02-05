/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "core/finstructable/tGroupInterface.h"
#include "core/tAnnotatable.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/tCoreFlags.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tGroupInterface> tGroupInterface::cCREATE_ACTION("Default Interface", util::tTypedClass<tGroupInterface>());
tConstructorCreateModuleAction<tGroupInterface, tGroupInterface::tDataClassification, tGroupInterface::tPortDirection, bool, bool> tGroupInterface::cCOMPLEX_CREATE_ACTION("Interface", util::tTypedClass<tGroupInterface>(), "Data classification, Port direction, Shared?, Unique Links");

tGroupInterface::tGroupInterface(tFrameworkElement* parent, const util::tString& name) :
  tPortGroup(parent, name, ::finroc::core::tEdgeAggregator::cIS_INTERFACE, 0),
  ports("Ports", this)
{
  ports.GetValue()->InitialSetup(this, 0, true);
}

tGroupInterface::tGroupInterface(tFrameworkElement* parent, const util::tString& name, tGroupInterface::tDataClassification data_class, tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link) :
  tPortGroup(parent, name, ComputeFlags(data_class, shared, unique_link), ComputePortFlags(port_dir, shared, unique_link)),
  ports("Ports", this)
{
  ports.GetValue()->InitialSetup(this, ComputePortFlags(port_dir, shared, unique_link), port_dir == eBOTH);
}

uint tGroupInterface::ComputeFlags(tGroupInterface::tDataClassification data_class, bool shared, bool unique_link)
{
  uint flags = ::finroc::core::tEdgeAggregator::cIS_INTERFACE;
  if (data_class == eSENSOR_DATA)
  {
    flags |= ::finroc::core::tEdgeAggregator::cSENSOR_DATA;
  }
  else if (data_class == eCONTROLLER_DATA)
  {
    flags |= ::finroc::core::tEdgeAggregator::cCONTROLLER_DATA;
  }
  if (shared)
  {
    flags |= tCoreFlags::cSHARED;
  }
  if (unique_link)
  {
    flags |= tCoreFlags::cGLOBALLY_UNIQUE_LINK;
  }
  return flags;
}

uint tGroupInterface::ComputePortFlags(tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link)
{
  uint flags = 0;
  if (shared)
  {
    flags |= tCoreFlags::cSHARED;
  }
  if (unique_link)
  {
    flags |= tCoreFlags::cGLOBALLY_UNIQUE_LINK;
  }
  if (port_dir == eINPUT_ONLY)
  {
    flags |= tPortFlags::cINPUT_PROXY;
  }
  else if (port_dir == eOUTPUT_ONLY)
  {
    flags |= tPortFlags::cOUTPUT_PROXY;
  }
  return flags;
}

} // namespace finroc
} // namespace core

