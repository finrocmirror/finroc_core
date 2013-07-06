//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
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
/*!\file    core/port/tEdgeAggregator.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tEdgeAggregator
 *
 * \b tEdgeAggregator
 *
 * Framework element that aggregates edges to determine data dependencies
 * between higher level entities (and to collect usage data).
 * Module and group interfaces, for instance, typically do this.
 *
 * This information is valuable for efficient scheduling.
 */
//----------------------------------------------------------------------
#ifndef __core__port__tEdgeAggregator_h__
#define __core__port__tEdgeAggregator_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

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
struct tAggregatedEdge;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Edge aggregating framework element
/*!
 * Framework element that aggregates edges to determine data dependencies
 * between higher level entities (and to collect usage data).
 * Module and group interfaces, for instance, typically do this.
 *
 * This information is valuable for efficient scheduling.
 */
class tEdgeAggregator : public tFrameworkElement
{
  typedef rrlib::concurrent_containers::tSet < tAggregatedEdge*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<5, 15, definitions::cSINGLE_THREADED >> tOutgoingConnectionSet;
  typedef tOutgoingConnectionSet::tConstIterator tOutgoingConnectionIterator;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! see FrameworkElement for parameter description */
  explicit tEdgeAggregator(tFrameworkElement* parent = NULL, const tString& name = "", tFlags flags = tFlags());

  /*!
   * (Should be called by abstract port only - with runtime registry locked)
   * Notify parent aggregators that edge has been added
   *
   * \param source Source port
   * \param target Target port
   */
  static void EdgeAdded(tAbstractPort& source, tAbstractPort& target);

  /*!
   * (Should be called by abstract port only - with runtime registry locked)
   * Notify parent aggregators that edge has been removed
   *
   * \param source Source port
   * \param target Target port
   */
  static void EdgeRemoved(tAbstractPort& source, tAbstractPort& target);

  /*!
   * \param dest Destination aggregating element
   * \return Edge that connects these elements - or NULL if such an edge does not yet exists
   */
  tAggregatedEdge* FindAggregatedEdge(tEdgeAggregator& dest);

  /*!
   * \param source Port
   * \return EdgeAggregator parent - or null if there's none
   */
  static tEdgeAggregator* GetAggregator(const tAbstractPort& source);

  /*!
   * \return True, if the provided type is a data flow type
   */
  inline static bool IsDataFlowType(const rrlib::rtti::tType& type)
  {
    return type.GetSize() > 0;
  }

  /*!
   * \return An iterator to iterate over all edge aggregators that this edge aggregators has outgoing connections to.
   */
  tOutgoingConnectionIterator OutgoingConnectionsBegin() const
  {
    return emerging_edges.Begin();
  }

  /*!
   * \return An iterator to iterate over all ports that this port has outgoing connections to pointing to the past-the-end element.
   */
  tOutgoingConnectionIterator OutgoingConnectionsEnd() const
  {
    return emerging_edges.End();
  }

  /*!
   * Update Edge Statistics: Called every time when data has been published
   *
   * \param source Source port
   * \param target Destination port
   * \param estimated_data_size Data Size of data
   */
  static void UpdateEdgeStatistics(tAbstractPort& source, tAbstractPort& target, size_t estimated_data_size);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Set of emerging aggregated edges */
  tOutgoingConnectionSet emerging_edges;

  /*!
   * Called when edge has been added that is relevant for this element
   *
   * \param dest Destination aggregating element
   * \param data_flow_type Was a data flow edge added?
   */
  void EdgeAdded(tEdgeAggregator& dest, bool data_flow_type);

  /*!
   * Called when edge has been added that is relevant for this element
   *
   * \param dest Destination aggregating element
   * \param data_flow_type Was a data flow edge added?
   */
  void EdgeRemoved(tEdgeAggregator& dest, bool data_flow_type);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
