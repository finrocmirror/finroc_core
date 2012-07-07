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
#include "core/port/tEdgeAggregator.h"
#include "core/tLockOrderLevels.h"
#include "core/port/tAbstractPort.h"
#include "core/portdatabase/tFinrocTypeInfo.h"

namespace finroc
{
namespace core
{
const uint tEdgeAggregator::cIS_INTERFACE;
const uint tEdgeAggregator::cSENSOR_DATA;
const uint tEdgeAggregator::cCONTROLLER_DATA;
const uint tEdgeAggregator::cALL_EDGE_AGGREGATOR_FLAGS;

tEdgeAggregator::tEdgeAggregator(tFrameworkElement* parent_, const util::tString& name_, uint flags_) :
  tFrameworkElement(parent_, name_, flags_ | tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cEDGE_AGGREGATOR, parent_ == NULL ? tLockOrderLevels::cLEAF_GROUP : -1),
  emerging_edges(0u)
{
}

void tEdgeAggregator::EdgeAdded(tAbstractPort& source, tAbstractPort& target)
{
  tEdgeAggregator* src = GetAggregator(source);
  tEdgeAggregator* dest = GetAggregator(target);
  if (src && dest && (!tFinrocTypeInfo::IsMethodType(source.GetDataType())))
  {
    //System.out.println("edgeAdded: " + src.getQualifiedName() + "->" + dest.getQualifiedName() + " (because of " + source.getQualifiedName() + "->" + target.getQualifiedName() + ")");
    src->EdgeAdded(*dest);
  }
}

void tEdgeAggregator::EdgeAdded(tEdgeAggregator& dest)
{
  tAggregatedEdge* ae = FindAggregatedEdge(dest);
  if (ae != NULL)
  {
    ae->edge_count++;
    return;
  }

  // not found
  ae = new tAggregatedEdge(*this, dest);
  ae->edge_count = 1u;
  emerging_edges.Add(ae, false);
}

void tEdgeAggregator::EdgeRemoved(tAbstractPort& source, tAbstractPort& target)
{
  tEdgeAggregator* src = GetAggregator(source);
  tEdgeAggregator* dest = GetAggregator(target);
  if (src && dest && (!tFinrocTypeInfo::IsMethodType(source.GetDataType())))
  {
    //System.out.println("edgeRemoved: " + src.getQualifiedName() + "->" + dest.getQualifiedName() + " (because of " + source.getQualifiedName() + "->" + target.getQualifiedName() + ")");
    src->EdgeRemoved(*dest);
  }
}

void tEdgeAggregator::EdgeRemoved(tEdgeAggregator& dest)
{
  tAggregatedEdge* ae = FindAggregatedEdge(dest);
  if (ae != NULL)
  {
    ae->edge_count--;
    if (ae->edge_count == 0)
    {
      //System.out.println("deleting edge");
      emerging_edges.Remove(ae);
      //ae.delete(); // obsolete: already deleted by command above
    }
    return;
  }
  throw util::tRuntimeException("Edge not found - this is inconsistent => programming error", CODE_LOCATION_MACRO);
}

tAggregatedEdge* tEdgeAggregator::FindAggregatedEdge(tEdgeAggregator& dest)
{
  util::tArrayWrapper<tAggregatedEdge*>* iterable = emerging_edges.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tAggregatedEdge* ae = iterable->Get(i);
    if (ae && &ae->destination == &dest)
    {
      return ae;
    }
  }
  return NULL;
}

tEdgeAggregator* tEdgeAggregator::GetAggregator(tAbstractPort& source)
{
  tFrameworkElement* current = source.GetParent();
  while (current)
  {
    if (current->GetFlag(tCoreFlags::cEDGE_AGGREGATOR) && (!current->GetFlag(tCoreFlags::cNETWORK_ELEMENT)))
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
  ar->publish_count.AddAndGet(1);
  ar->publish_size.AddAndGet(estimated_data_size);
}

} // namespace finroc
} // namespace core

