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

#ifndef core__thread__tThreadContainerElement_h__
#define core__thread__tThreadContainerElement_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tStructureParameter.h"
#include "core/datatype/tBounds.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/thread/tThreadContainerThread.h"
#include "core/finstructable/tGroup.h"
#include "core/thread/tStartAndPausable.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Contains thread that executes OrderedPeriodicTasks of all children.
 * Execution in performed in the order of the graph.
 *
 * This is the base class for tThreadContainer.
 * Is realized as template to allow creating things like FinstructableThreadContainers.
 */
template <typename BASE>
class tThreadContainerElement : public BASE, public tStartAndPausable
{
private:

  using BASE::log_domain;

  /*! Should this container contain a real-time thread? */
  tStructureParameter<bool> rt_thread;

  /*! Thread cycle time */
  tStructureParameter<int> cycle_time;

  /*! Warn on cycle time exceed */
  tStructureParameter<bool> warn_on_cycle_time_exceed;

  /*! Thread - while program is running - in pause mode null */
  std::shared_ptr<tThreadContainerThread> thread;

  /*!
   * Stop thread in thread container (does not block - call join thread to block until thread has terminated)
   */
  void StopThread();

public:

  /*!
   * \param description Name
   * \param parent parent
   */
  tThreadContainerElement(tFrameworkElement* parent, const util::tString& description, uint flags = 0);

  virtual ~tThreadContainerElement();

  /*!
   * \return Cycle time in milliseconds
   */
  inline int GetCycleTime()
  {
    return cycle_time.Get();
  }

  virtual bool IsExecuting();

  /*!
   * Block until thread has stopped
   */
  void JoinThread();

  virtual void PauseExecution()
  {
    StopThread();
    JoinThread();
  }

  /*!
   * \param period Cycle time in milliseconds
   */
  inline void SetCycleTime(int period)
  {
    cycle_time.Set(period);
  }

  virtual void StartExecution();

};

} // namespace finroc
} // namespace core

#include "core/thread/tThreadContainerElement.hpp"

#endif // core__thread__tThreadContainerElement_h__
