//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/port/tEdgeAggregator.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/port/tEdgeAggregator.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tAbstractPort.h"
#include "core/port/tAggregatedEdge.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tEdgeAggregator::tEdgeAggregator(tFrameworkElement* parent, const tString& name, tFlags flags) :
  tFrameworkElement(parent, name, flags | tFlag::EDGE_AGGREGATOR),
  emerging_edges(),
  incoming_edges()
{
}

void tEdgeAggregator::EdgeAdded(tAbstractPort& source, tAbstractPort& target)
{
  tEdgeAggregator* src = GetAggregator(source);
  tEdgeAggregator* dest = GetAggregator(target);
  if (src && dest)
  {
    src->EdgeAdded(*dest, IsDataFlowType(source.GetDataType()));
  }
}

void tEdgeAggregator::EdgeAdded(tEdgeAggregator& dest, bool data_flow_type)
{
  tAggregatedEdge* ae = FindAggregatedEdge(dest);
  if (ae != NULL)
  {
    ae->GetCountVariable(data_flow_type)++;
    return;
  }

  // not found
  ae = new tAggregatedEdge(*this, dest);
  ae->GetCountVariable(data_flow_type) = 1;
  emerging_edges.Add(ae);
  dest.incoming_edges.Add(ae);
}

void tEdgeAggregator::EdgeRemoved(tAbstractPort& source, tAbstractPort& target)
{
  tEdgeAggregator* src = GetAggregator(source);
  tEdgeAggregator* dest = GetAggregator(target);
  if (src && dest)
  {
    src->EdgeRemoved(*dest, IsDataFlowType(source.GetDataType()));
  }
}

void tEdgeAggregator::EdgeRemoved(tEdgeAggregator& dest, bool data_flow_type)
{
  tAggregatedEdge* ae = FindAggregatedEdge(dest);
  if (ae)
  {
    ae->GetCountVariable(data_flow_type)--;
    if (ae->control_flow_edge_count + ae->data_flow_edge_count == 0)
    {
      emerging_edges.Remove(ae);
      dest.incoming_edges.Remove(ae);
      delete ae;
    }
    return;
  }
  FINROC_LOG_PRINT(ERROR, "Edge not found - this is inconsistent and likely programming error");
}

tAggregatedEdge* tEdgeAggregator::FindAggregatedEdge(tEdgeAggregator& dest)
{
  for (auto it = emerging_edges.Begin(); it != emerging_edges.End(); ++it)
  {
    if (&((*it)->destination) == &dest)
    {
      return &(**it);
    }
  }
  return NULL;
}

tEdgeAggregator* tEdgeAggregator::GetAggregator(const tAbstractPort& source)
{
  tFrameworkElement* current = source.GetParent();
  while (current)
  {
    if (current->GetFlag(tFlag::EDGE_AGGREGATOR) && (!current->GetFlag(tFlag::NETWORK_ELEMENT)))
    {
      return static_cast<tEdgeAggregator*>(current);
    }
    current = current->GetParent();
  }
  return NULL;
}

void tEdgeAggregator::UpdateEdgeStatistics(tAbstractPort& source, tAbstractPort& target, size_t estimated_data_size)
{
  tEdgeAggregator* src = GetAggregator(source);
  tEdgeAggregator* dest = GetAggregator(target);
  assert(src && dest);
  tAggregatedEdge* ar = src->FindAggregatedEdge(*dest);
  assert(ar);
  ar->publish_count.fetch_add(1);
  ar->publish_size.fetch_add(estimated_data_size);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
