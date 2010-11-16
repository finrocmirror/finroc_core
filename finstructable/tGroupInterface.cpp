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
#include "core/parameter/tStructureParameterList.h"
#include "core/tCoreFlags.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tGroupInterface> tGroupInterface::cCREATE_ACTION("Default Interface", util::tTypedClass<tGroupInterface>());
tConstructorCreateModuleAction<tGroupInterface, tGroupInterface::tDataClassification, tGroupInterface::tPortDirection, bool, bool> tGroupInterface::cCOMPLEX_CREATE_ACTION("Interface", util::tTypedClass<tGroupInterface>(), "Data classification, Port direction, Shared?, Unique Links");

tGroupInterface::tGroupInterface(const util::tString& description, tFrameworkElement* parent) :
    tEdgeAggregator(description, parent, ::finroc::core::tEdgeAggregator::cIS_INTERFACE),
    ports(new tStructureParameter<tPortCreationList>("Ports", tPortCreationList::cTYPE))
{
  AddAnnotation(new tStructureParameterList(ports));
  ports->GetValue()->InitialSetup(this, 0, true);
}

tGroupInterface::tGroupInterface(const util::tString& description, tFrameworkElement* parent, tGroupInterface::tDataClassification data_class, tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link) :
    tEdgeAggregator(description, parent, ComputePortFlags(data_class, port_dir, shared, unique_link)),
    ports(new tStructureParameter<tPortCreationList>("Ports", tPortCreationList::cTYPE))
{
  AddAnnotation(new tStructureParameterList(ports));
  ports->GetValue()->InitialSetup(this, 0, port_dir == tGroupInterface::eBOTH);
}

int tGroupInterface::ComputePortFlags(tGroupInterface::tDataClassification data_class, tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link)
{
  int flags = ::finroc::core::tEdgeAggregator::cIS_INTERFACE;
  if (data_class == tGroupInterface::eSENSOR_DATA)
  {
    flags |= ::finroc::core::tEdgeAggregator::cSENSOR_DATA;
  }
  else if (data_class == tGroupInterface::eCONTROLLER_DATA)
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
  if (port_dir == tGroupInterface::eINPUT_ONLY)
  {
    flags |= tPortFlags::cINPUT_PROXY;
  }
  else if (port_dir == tGroupInterface::eOUTPUT_ONLY)
  {
    flags |= tPortFlags::cOUTPUT_PROXY;
  }
  return flags;
}

} // namespace finroc
} // namespace core

