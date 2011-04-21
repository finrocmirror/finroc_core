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
/*!\file    tGroup.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 */
//----------------------------------------------------------------------
#include "core/structure/tGroup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/thread/tPeriodicFrameworkElementTask.h"

#include "rrlib/logging/definitions.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
//using namespace finroc;
using namespace finroc::core::structure;
using namespace rrlib::logging;

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
// tGroup constructors
//----------------------------------------------------------------------
tGroup::tGroup(tFrameworkElement *parent, const util::tString &name, const util::tString &structure_config_file)
    : tFinstructableGroup(parent, name, structure_config_file),

    controller_input(new tEdgeAggregator(this, "Controller Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),
    controller_output(new tEdgeAggregator(this, "Controller Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),

    sensor_input(new tEdgeAggregator(this, "Sensor Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    sensor_output(new tEdgeAggregator(this, "Sensor Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    auto_name_port_count(0)
{
  tStructureElementRegister::AddModule(this);
}

////----------------------------------------------------------------------
//// tGroup SetParameter
////----------------------------------------------------------------------
//void tGroup::SetParameter(size_t index, const finroc::util::tString &new_value)
//{
//  finroc::core::tStructureParameterList* spl = static_cast<finroc::core::tStructureParameterList*>(this->GetAnnotation(finroc::core::tStructureParameterList::cTYPE));
//  finroc::core::tStructureParameterBase* param = spl->Get(index);
//  param->Set(new_value);
//
//  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter>> updating parameter with name: " << param->GetName();
//
//  this->StructureParametersChanged();
//}
//
//void tGroup::SetParameter(const finroc::util::tString &name, const finroc::util::tString &new_value)
//{
//  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter(" << name << ", " << new_value << ") called";
//
//  finroc::core::tStructureParameterList* spl = static_cast<finroc::core::tStructureParameterList*>(this->GetAnnotation(finroc::core::tStructureParameterList::cTYPE));
//
//  size_t dimension = spl->Size();
//  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter>> analysing structure parameter ist of size " << dimension;
//  for (size_t i = 0; i < dimension; ++i)
//  {
//    finroc::core::tStructureParameterBase* param = spl->Get(i);
//    if (param->GetName() == name)
//    {
//      this->SetParameter(i, new_value);
//      break;
//    }
//  }
//}
