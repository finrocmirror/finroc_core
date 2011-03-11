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

#ifndef core__thread__tPeriodicFrameworkElementTask_h__
#define core__thread__tPeriodicFrameworkElementTask_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataType.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/port/tEdgeAggregator.h"
#include "core/tFinrocAnnotation.h"

namespace finroc
{
namespace util
{
class tTask;
} // namespace finroc
} // namespace util

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This represents a periodic task on the annotated Framework element
 *
 * Such tasks are executed by a ThreadContainer - in the order of the graph.
 */
struct tPeriodicFrameworkElementTask : public tFinrocAnnotation
{
public:

  /*! Data Type */
  static rrlib::serialization::tDataType<tPeriodicFrameworkElementTask> cTYPE;

  /*! Task to execute */
  util::tTask* task;

  /*! Element containing incoming ports (relevant for execution order) */
  tEdgeAggregator* incoming;

  /*! Element containing outgoing ports (relevant for execution order) */
  tEdgeAggregator* outgoing;

  /*! Tasks to execute before this one (updated during scheduling) */
  util::tSimpleList<tPeriodicFrameworkElementTask*> previous_tasks;

  /*! Tasks to execute after this one (updated during scheduling) */
  util::tSimpleList<tPeriodicFrameworkElementTask*> next_tasks;

  /*!
   * \param incoming_ports Element containing incoming ports (relevant for execution order)
   * \param outgoing_ports Element containing outgoing ports (relevant for execution order)
   * \param task Task to execute
   */
  tPeriodicFrameworkElementTask(tEdgeAggregator* incoming_ports, tEdgeAggregator* outgoing_ports, util::tTask* task_);

  /*!
   * Dummy constructor. Generic instantiation is not supported.
   */
  tPeriodicFrameworkElementTask();

  /*!
   * \return Is this a sensor task?
   */
  inline bool IsSenseTask()
  {
    return outgoing->GetFlag(tEdgeAggregator::cSENSOR_DATA) || incoming->GetFlag(tEdgeAggregator::cSENSOR_DATA);
  }

};

} // namespace finroc
} // namespace core

#endif // core__thread__tPeriodicFrameworkElementTask_h__
