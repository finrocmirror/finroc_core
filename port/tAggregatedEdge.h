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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__TAGGREGATEDEDGE_H
#define CORE__PORT__TAGGREGATEDEDGE_H

#include "rrlib/finroc_core_utils/tTime.h"
#include "core/tAnnotatable.h"

namespace finroc
{
namespace core
{
class tEdgeAggregator;

/*!
 * \author Max Reichardt
 *
 * Collection of edges between ports that have the same EdgeAggregator
 * framework element parents as start and destination.
 *
 * The start EdgeAggregator owns objects of this type.
 */
class tAggregatedEdge : public tAnnotatable
{
public:

  /*! Number of aggregated edges */
  size_t edge_count;

  /*! Pointer to source and destination element */
  tEdgeAggregator* source, * destination;

  /*! Usage statistics: Time when edge was created */
  int64 creation_time;

  /*! Usage statistics: Number of published elements */
  util::tAtomicInt64 publish_count;

  /*! Usage statistics: Size of published elements */
  util::tAtomicInt64 publish_size;

  /*!
   * \param src Source aggregator
   * \param dest Destination aggregator
   */
  tAggregatedEdge(tEdgeAggregator* src, tEdgeAggregator* dest) :
      edge_count(0),
      source(src),
      destination(dest),
      creation_time(util::tTime::GetPrecise()),
      publish_count(0),
      publish_size(0)
  {
  }

  /*!
   * \return How much data (in bytes) is transferred over this edge per second (average)?
   */
  inline int GetDataRate()
  {
    return static_cast<int>(((publish_size.Get() * 1000) / std::max(static_cast<int64>(1), util::tTime::GetCoarse() - creation_time)));
  }

  /*!
   * \return How many publishes are transferred over this edge per second (average)?
   */
  inline float GetPublishRate()
  {
    return ((static_cast<float>(publish_count.Get())) * 1000.0f) / (static_cast<float>(std::max(static_cast<int64>(1), util::tTime::GetCoarse() - creation_time)));
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__TAGGREGATEDEDGE_H
