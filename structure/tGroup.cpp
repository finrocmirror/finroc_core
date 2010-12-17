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

#include "core/port/tAbstractPort.h"
#include "core/tLinkEdge.h"

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
tGroup::tGroup(tFrameworkElement *parent, const util::tString &name)
    : tFinstructableGroup(parent, name),

    controller_input(new tEdgeAggregator(this, "Controller Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),
    controller_output(new tEdgeAggregator(this, "Controller Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),

    sensor_input(new tEdgeAggregator(this, "Sensor Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    sensor_output(new tEdgeAggregator(this, "Sensor Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA))
{}

//----------------------------------------------------------------------
// tGroup SetParameter
//----------------------------------------------------------------------
void tGroup::SetParameter(size_t index, const finroc::util::tString &new_value)
{
  finroc::core::tStructureParameterList* spl = static_cast<finroc::core::tStructureParameterList*>(this->GetAnnotation(finroc::core::tStructureParameterList::cTYPE));
  finroc::core::tStructureParameterBase* param = spl->Get(index);
  param->Set(new_value);

  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter>> updating parameter with name: " << param->GetName();

  this->StructureParametersChanged();
}

void tGroup::SetParameter(const finroc::util::tString &name, const finroc::util::tString &new_value)
{
  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter(" << name << ", " << new_value << ") called";

  finroc::core::tStructureParameterList* spl = static_cast<finroc::core::tStructureParameterList*>(this->GetAnnotation(finroc::core::tStructureParameterList::cTYPE));

  size_t dimension = spl->Size();
  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription() << "::SetParameter>> analysing structure parameter ist of size " << dimension;
  for (size_t i = 0; i < dimension; ++i)
  {
    finroc::core::tStructureParameterBase* param = spl->Get(i);
    if (param->GetName() == name)
    {
      this->SetParameter(i, new_value);
      break;
    }
  }
}

//----------------------------------------------------------------------
// tGroup TreeFilterCallback
//----------------------------------------------------------------------
void tGroup::TreeFilterCallback(tFrameworkElement *fe, rrlib::xml2::tXMLNode &root)
{
  util::tSimpleList<tAbstractPort *> connect_tmp;
  assert((fe->IsPort()));
  tAbstractPort *ap = static_cast<tAbstractPort *>(fe);
  ap->GetConnectionPartners(connect_tmp, true, false);

  for (size_t i = 0u; i < connect_tmp.Size(); ++i)
  {
    tAbstractPort *ap2 = connect_tmp.Get(i);

    // save edge?
    // check1: different finstructed elements as parent?
    tFrameworkElement *fe1 = ap->GetParentWithFlags(tCoreFlags::cFINSTRUCTED);
    tFrameworkElement *fe2 = ap2->GetParentWithFlags(tCoreFlags::cFINSTRUCTED);

    if (fe1 == fe2)
    {
      if (fe1 != NULL)
      {
        //  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription () << "::TreeFilterCallback>> different finstructed elements as parent: 1: " << fe1->GetDescription () << "2: " << fe2->GetDescription ();
      }
      else
      {
        //  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription () << "::TreeFilterCallback>> no parent element with core flag cFINSTRUCTED!: " << ap->GetDescription () << " and " << ap2->GetDescription ();
      }

      //      continue;
    }

    // check2: their deepest common finstructable_group parent is this
    finroc::core::tFrameworkElement *common_parent = ap->GetParent();
    while (!ap2->IsChildOf(common_parent))
    {
      common_parent = common_parent->GetParent();
    }
    finroc::core::tFrameworkElement *common_finstructable_parent = common_parent->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
    if (common_finstructable_parent != this)
    {
      if (common_finstructable_parent != NULL)
      {
        //  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription () << "::TreeFilterCallback>> parent is not 'this': " << common_finstructable_parent->GetDescription ();
      }
      else
      {
        //  FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription () << "::TreeFilterCallback>> parent is NULL";
      }
      //      continue;
    }

    // check3: only save non-volatile connections in this step
    if (ap->IsVolatile() || ap2->IsVolatile())
    {
      //      FINROC_LOG_STREAM(eLL_DEBUG) << this->GetDescription () << "::TreeFilterCallback>> connection is volatile: ap1: " << ap->IsVolatile () << "ap2: " << ap2->IsVolatile ();

      //      continue;
    }

    // save edge
    rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
//    edge.SetAttribute("src", GetEdgeLink(ap));
//    edge.SetAttribute("dest", GetEdgeLink(ap2));
  }

  // serialize link edges
  if (ap->GetLinkEdges() != NULL)
  {
    for (size_t i = 0u; i < ap->GetLinkEdges()->Size(); ++i)
    {
      tLinkEdge *le = ap->GetLinkEdges()->Get(i);
      if (le->GetSourceLink().Length() > 0)
      {
        // save edge
        rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
//        edge.SetAttribute("src", GetEdgeLink(le->GetSourceLink()));
//        edge.SetAttribute("dest", GetEdgeLink(ap));
      }
      else
      {
        // save edge
        rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
//        edge.SetAttribute("src", GetEdgeLink(ap));
//        edge.SetAttribute("dest", GetEdgeLink(le->GetTargetLink()));
      }
    }
  }
}
