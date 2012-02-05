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

#ifndef core__port__tEdgeAggregator_h__
#define core__port__tEdgeAggregator_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/port/tAggregatedEdge.h"
#include "core/tCoreFlags.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Framework element that aggregates edges to determine data dependencies
 * between higher level entities (and to collect usage data).
 * Modules, for instance, typically do this.
 *
 * This information will be valuable for efficient scheduling
 */
class tEdgeAggregator : public tFrameworkElement
{
private:

  /*! List of emerging aggregated edges */
  util::tSafeConcurrentlyIterableList<tAggregatedEdge*, 5, true> emerging_edges;

public:

  /*! Is this edge aggregator an interface of its parent (one of possibly many) */
  static const uint cIS_INTERFACE = tCoreFlags::cFIRST_CUSTOM_CONST_FLAG;

  /*! Hint for displaying in finstruct: Is this sensor data only? */
  static const uint cSENSOR_DATA = tCoreFlags::cFIRST_CUSTOM_CONST_FLAG << 1;

  /*! Hint for displaying in finstruct: Is this controller data only? */
  static const uint cCONTROLLER_DATA = tCoreFlags::cFIRST_CUSTOM_CONST_FLAG << 2;

  /*! All flags introduced by edge aggregator class */
  static const uint cALL_EDGE_AGGREGATOR_FLAGS = cIS_INTERFACE | cSENSOR_DATA | cCONTROLLER_DATA;

private:

  /*!
   * Called when edge has been added that is relevant for this element
   *
   * \param dest Destination aggregating element
   */
  void EdgeAdded(tEdgeAggregator* dest);

  /*!
   * Called when edge has been added that is relevant for this element
   *
   * \param dest Destination aggregating element
   */
  void EdgeRemoved(tEdgeAggregator* dest);

  /*!
   * \param source Port
   * \return EdgeAggregator parent - or null if there's none
   */
  static tEdgeAggregator* GetAggregator(tAbstractPort* source);

public:

  /*! see FrameworkElement for parameter description */
  tEdgeAggregator(tFrameworkElement* parent_ = NULL, const util::tString& name_ = "", uint flags_ = 0);

  /*!
   * (Should be called by abstract port only)
   * Notify parent aggregators that edge has been added
   *
   * \param source Source port
   * \param target Target port
   */
  static void EdgeAdded(tAbstractPort* source, tAbstractPort* target);

  /*!
   * (Should be called by abstract port only)
   * Notify parent aggregators that edge has been removed
   *
   * \param source Source port
   * \param target Target port
   */
  static void EdgeRemoved(tAbstractPort* source, tAbstractPort* target);

  /*!
   * \param dest Destination aggregating element
   * \return Edge that connects these elements - or null if such an edge does not yet exists
   */
  tAggregatedEdge* FindAggregatedEdge(tEdgeAggregator* dest);

  /*!
   * \return Array with emerging edges. Can be iterated over concurrently.
   */
  inline util::tArrayWrapper<tAggregatedEdge*>* GetEmergingEdges()
  {
    util::tArrayWrapper<tAggregatedEdge*>* iter = emerging_edges.GetIterable();
    assert(iter != NULL);
    return iter;
  }

  /*!
   * Update Edge Statistics: Called every time when data has been published
   *
   * \param source Source port
   * \param target Destination port
   * \param estimated_data_size Data Size of data
   */
  static void UpdateEdgeStatistics(tAbstractPort* source, tAbstractPort* target, size_t estimated_data_size);

};

} // namespace finroc
} // namespace core

#endif // core__port__tEdgeAggregator_h__
