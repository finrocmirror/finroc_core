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

#ifndef core__port__rpc__tRPCThread_h__
#define core__port__rpc__tRPCThread_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/thread/tLoopThread.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"
#include "rrlib/finroc_core_utils/container/tReusable.h"
#include "rrlib/watchdog/tWatchDogTask.h"

#include "core/portdatabase/tSerializableReusable.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * (Helper) thread for remote procedure calls
 */
class tRPCThread : public finroc::util::tLoopThread, public util::tReusable, public rrlib::watchdog::tWatchDogTask
{
private:

  /*! Task to execute next */
  util::tTask* volatile next_task;

  /*! Next reusable task */
  tSerializableReusableTask::tPtr next_reusable_task;

  /*! String representation of task that is currently executed */
  std::string current_task_string;

  /*! Mutex String representation of task that is currently executed */
  util::tMutex current_task_string_mutex;

public:

  tRPCThread();

  /*!
   * Execute task using this thread
   *
   * \param t Task to execute
   */
  void ExecuteTask(util::tTask& t);
  void ExecuteTask(tSerializableReusableTask::tPtr& t);

  virtual void HandleWatchdogAlert();

  virtual void MainLoopCallback();

  virtual void Run();

  virtual void StopThread();

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tRPCThread_h__
