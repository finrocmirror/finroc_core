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
#include "core/port/tThreadLocalCache.h"
#include "core/port/rpc/tRPCThread.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
tRPCThread::tRPCThread() :
  tLoopThread(0),
#ifdef NDEBUG  // only activate monitoring in debug mode
  tWatchDogTask(false),
#else
  tWatchDogTask(true),
#endif
  next_task(NULL),
  next_reusable_task()
{
}

void tRPCThread::ExecuteTask(util::tTask& t)
{
  util::tLock lock1(this);
  assert((next_task == NULL));
  next_task = &t;
  monitor.NotifyAll(lock1);
}

void tRPCThread::ExecuteTask(tSerializableReusableTask::tPtr& t)
{
  util::tLock lock1(this);
  assert(!next_reusable_task);
  next_reusable_task = std::move(t);
  monitor.NotifyAll(lock1);
}

void tRPCThread::HandleWatchdogAlert()
{
  util::tLock lock2(this);
  RRLIB_LOG_PRINT(rrlib::logging::eLL_WARNING, "Watchdoggy detected that execution of the following task got stuck: ", current_task_string);
  tWatchDogTask::Deactivate();
}

void tRPCThread::MainLoopCallback()
{
  {
    util::tLock lock2(this);
    if (next_task == NULL && next_reusable_task.get() == NULL)
    {
      tRPCThreadPool::GetInstance().EnqueueThread(this);

      if (!IsStopSignalSet())
      {
        monitor.Wait(lock2);
      }
    }
  }
  while (next_task || next_reusable_task)
  {
    assert(GetDeadLine() == cTASK_DEACTIVED);
#ifndef NDEBUG
    SetDeadLine(util::tTime::GetPrecise() + 3000); // TODO: deadline should depend on type of call
#endif
    if (next_task)
    {
      util::tTask* tmp = next_task;
      next_task = NULL;
      tmp->ExecuteTask();
    }
    else
    {
      tSerializableReusableTask::tPtr tmp(std::move(next_reusable_task));
#ifndef NDEBUG
      {
        util::tLock lock2(this);
        current_task_string = tmp->ToString();
      }
#endif
      tmp->ExecuteTask(tmp);
    }
    tWatchDogTask::Deactivate();
  }
}

void tRPCThread::Run()
{
  tThreadLocalCache::Get();
  finroc::util::tLoopThread::Run();
}

void tRPCThread::StopThread()
{
  util::tLock lock2(this);
  monitor.Notify(lock2);
}

} // namespace finroc
} // namespace core

