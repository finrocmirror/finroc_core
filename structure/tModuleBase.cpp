//
// You received this file as part of Finroc
// A framework for integrated robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    tModuleBase.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2011-04-12
 *
 */
//----------------------------------------------------------------------
#include "core/structure/tModuleBase.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
using namespace finroc::core::structure;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// tModuleBase constructors
//----------------------------------------------------------------------
tModuleBase::tModuleBase(tFrameworkElement *parent, const util::tString &name)
    : tFrameworkElement(parent, name),
    parameters(new tFrameworkElement(this, "Parameters")),
    parameters_changed(true),
    auto_name_port_count(0),
    count_for_type(NULL)
{
  tStructureElementRegister::AddModule(this);
}

void tModuleBase::CheckParameters()
{
  if (parameters_changed)
  {
    parameters_changed = false;
    ParametersChanged();
  }
}

void tModuleBase::PortChanged(tAbstractPort* origin, const void* const& value)
{
  parameters_changed = true;
}

bool tModuleBase::HasAnyPortChanged(tEdgeAggregator* ea)
{
  bool result = false;
  tChildIterator ci(ea);
  tAbstractPort* ap = NULL;
  while ((ap = ci.NextPort()) != NULL)
  {
    result |= ap->HasChanged();
  }
  return result;
}

void tModuleBase::ResetChangedFlags(tEdgeAggregator* ea)
{
  tChildIterator ci(ea);
  tAbstractPort* ap = NULL;
  while ((ap = ci.NextPort()) != NULL)
  {
    ap->ResetChanged();
  }
}

