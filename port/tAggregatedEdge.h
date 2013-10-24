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
/*!\file    core/port/tAggregatedEdge.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tAggregatedEdge
 *
 * \b tAggregatedEdge
 *
 * Collection of edges between ports that have the same EdgeAggregator
 * framework element parents as start and destination.
 *
 * The start EdgeAggregator owns objects of this type.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tAggregatedEdge_h__
#define __core__port__tAggregatedEdge_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tAnnotatable.h"

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
class tEdgeAggregator;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Edge aggregating multiple single edges
/*!
 * Collection of edges between ports that have the same EdgeAggregator
 * framework element parents as start and destination.
 *
 * The start EdgeAggregator owns objects of this type.
 */
struct tAggregatedEdge : public tAnnotatable
{

  /*! Number of aggregated data flow edges */
  int data_flow_edge_count;

  /*! Number of aggregated control flow edges */
  int control_flow_edge_count;

  /*! Pointer to source and destination element */
  tEdgeAggregator& source, & destination;

  /*! Usage statistics: Time when edge was created */
  rrlib::time::tTimestamp creation_time;

  /*! Usage statistics: Number of published elements */
  std::atomic<int64_t> publish_count;

  /*! Usage statistics: Size of published elements */
  std::atomic<int64_t> publish_size;

  /*!
   * \param src Source aggregator
   * \param dest Destination aggregator
   */
  tAggregatedEdge(tEdgeAggregator& src, tEdgeAggregator& dest) :
    data_flow_edge_count(0),
    control_flow_edge_count(0),
    source(src),
    destination(dest),
    creation_time(rrlib::time::Now()),
    publish_count(0),
    publish_size(0)
  {
  }

  /*!
   * \return Variable for counting edge with such type
   */
  int& GetCountVariable(bool data_flow_type)
  {
    return data_flow_type ? data_flow_edge_count : control_flow_edge_count;
  }

  /*!
   * \return How much data (in bytes) is transferred over this edge per second (average)?
   */
  inline int GetDataRate()
  {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(rrlib::time::Now(false) - creation_time);
    return ms.count() == 0 ? 0 : static_cast<int>((publish_size.load() * 1000) / ms.count());
  }

  /*!
   * \return How many publishes are transferred over this edge per second (average)?
   */
  inline float GetPublishRate()
  {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(rrlib::time::Now(false) - creation_time);
    return ms.count() == 0 ? 0.f : (static_cast<float>(publish_count.load()) * 1000.0f) / (static_cast<float>(ms.count()));
  }

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
