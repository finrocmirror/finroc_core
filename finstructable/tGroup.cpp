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
#include "core/finstructable/tGroup.h"
#include "core/tAnnotatable.h"
#include "core/parameter/tStructureParameterList.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tGroup> tGroup::cCREATE_ACTION("Group", util::tTypedClass<tGroup>());

tGroup::tGroup(const util::tString& description, tFrameworkElement* parent) :
    tEdgeAggregator(description, parent, 0),
    ports(new tStructureParameter<tPortCreationList>("Ports", tPortCreationList::cTYPE))
{
  AddAnnotation(new tStructureParameterList(ports));
  ports->GetValue()->InitialSetup(this, 0, true);
}

} // namespace finroc
} // namespace core
