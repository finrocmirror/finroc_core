/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef core__thread__tThreadContainerThread_h__
#define core__thread__tThreadContainerThread_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/watchdog/tWatchDogTask.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElement.h"
#include "core/tCoreFlags.h"
#include "core/port/tEdgeAggregator.h"
#include "core/thread/tCoreLoopThreadBase.h"
#include "core/tRuntimeListener.h"
#include "core/port/tPort.h"

namespace finroc
{
namespace core
{
class tThreadContainer;
class tPeriodicFrameworkElementTask;
class tAbstractPort;

/*! ThreadContainer thread class */
class tThreadContainerThread : public tCoreLoopThreadBase, public tRuntimeListener, public rrlib::watchdog::tWatchDogTask
{
private:

  /*! Thread container that thread belongs to */
  tFrameworkElement& thread_container;

  /*! true, when thread needs to make a new schedule before next run */
  std::atomic<bool> reschedule;

  /*! simple schedule: Tasks will be executed in specified order */
  util::tSimpleList<tPeriodicFrameworkElementTask*> schedule;

  /*! temporary list of tasks that need to be scheduled */
  util::tSimpleList<tPeriodicFrameworkElementTask*> tasks;

  /*! temporary list of tasks that need to be scheduled - which are not sensor tasks */
  util::tSimpleList<tPeriodicFrameworkElementTask*> non_sensor_tasks;

  /*! temporary variable for scheduling algorithm: trace we're currently following */
  util::tSimpleList<tEdgeAggregator*> trace;

  /*! temporary variable: trace back */
  util::tSimpleList<tPeriodicFrameworkElementTask*> trace_back;

  /*! Port to publish time spent in last call to MainLoopCallback() */
  tPort<rrlib::time::tDuration> last_cycle_execution_time;

  /*!
   * Thread sets this to the task it is currently executing (for error message, should it get stuck)
   * NULL if not executing any task
   */
  tPeriodicFrameworkElementTask* current_task;

  virtual void HandleWatchdogAlert();

  /*!
   * Trace outgoing connection
   *
   * \param task Task we're tracing from
   * \param outgoing edge aggregator with outgoing connections to follow
   */
  void TraceOutgoing(tPeriodicFrameworkElementTask& task, tEdgeAggregator& outgoing);

public:

  tThreadContainerThread(tFrameworkElement& thread_container, rrlib::time::tDuration default_cycle_time, bool warn_on_cycle_time_exceed, tPort<rrlib::time::tDuration> last_cycle_execution_time);

  /*!
   * \return Returns pointer to current thread if it is a tThreadContainerThread - NULL otherwise
   */
  static tThreadContainerThread* CurrentThread()
  {
    rrlib::thread::tThread& thread = rrlib::thread::tThread::CurrentThread();
    return (typeid(thread) == typeid(tThreadContainerThread)) ? static_cast<tThreadContainerThread*>(&thread) : NULL;
  }

  /*!
   * \return Shared Pointer to thread container thread
   */
  std::shared_ptr<tThreadContainerThread> GetSharedPtr()
  {
    return std::static_pointer_cast<core::tThreadContainerThread>(tThread::GetSharedPtr());
  }

  /*!
   * \param fe Framework element
   * \return Is framework element an interface?
   */
  inline bool IsInterface(tFrameworkElement& fe)
  {
    return fe.GetFlag(tCoreFlags::cEDGE_AGGREGATOR | tEdgeAggregator::cIS_INTERFACE);
  }

  virtual void MainLoopCallback();

  virtual void Run();

  virtual void RuntimeChange(int8 change_type, tFrameworkElement& element);

  virtual void RuntimeEdgeChange(int8 change_type, tAbstractPort& source, tAbstractPort& target);

  virtual void StopThread();
};

} // namespace finroc
} // namespace core

#endif // core__thread__tThreadContainerThread_h__
