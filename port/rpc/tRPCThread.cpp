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
  tLoopThread(rrlib::time::tDuration(0)),
#ifdef NDEBUG  // only activate monitoring in debug mode
  tWatchDogTask(false),
#else
  tWatchDogTask(true),
#endif
  next_task(NULL),
  used(true),
  next_reusable_task()
{
  static int count = 0;
  std::ostringstream oss;
  oss << "RPC Thread " << count;
  SetName(oss.str());
  count++;
}

void tRPCThread::ExecuteTask(rrlib::thread::tTask& t)
{
  rrlib::thread::tLock lock1(*this);
  assert((next_task == NULL));
  next_task = &t;
  GetMonitor().NotifyAll(lock1);
}

void tRPCThread::ExecuteTask(tSerializableReusableTask::tPtr& t)
{
  rrlib::thread::tLock lock1(*this);
  assert(!next_reusable_task);
  next_reusable_task = std::move(t);
  GetMonitor().NotifyAll(lock1);
}

void tRPCThread::HandleWatchdogAlert()
{
  rrlib::thread::tLock lock2(current_task_string_mutex);
  FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Watchdoggy detected that execution of the following task got stuck: ", current_task_string);
  tWatchDogTask::Deactivate();
}

void tRPCThread::MainLoopCallback()
{
  {
    rrlib::thread::tLock lock2(*this);
    if (next_task == NULL && next_reusable_task.get() == NULL)
    {
      if (!used)
      {
        tRPCThreadPool::GetInstance().EnqueueThread(this);
      }

      if (!IsStopSignalSet())
      {
        GetMonitor().Wait(lock2);
      }
    }
  }
  while (next_task || next_reusable_task)
  {
    assert(GetDeadLine() == rrlib::time::cNO_TIME);
#ifndef NDEBUG
    SetDeadLine(rrlib::time::Now() + std::chrono::seconds(3)); // TODO: deadline should depend on type of call
#endif
    if (next_task)
    {
      rrlib::thread::tTask* tmp = next_task;
      next_task = NULL;
      tmp->ExecuteTask();
    }
    else
    {
      tSerializableReusableTask::tPtr tmp(std::move(next_reusable_task));
#ifndef NDEBUG
      {
        rrlib::thread::tLock lock2(current_task_string_mutex);
        current_task_string = tmp->ToString();
      }
#endif
      tmp->ExecuteTask(tmp);
    }
    tWatchDogTask::Deactivate();
    used = false;
  }
}

void tRPCThread::Run()
{
  tThreadLocalCache::Get();
  tLoopThread::Run();
}

void tRPCThread::StopThread()
{
  rrlib::thread::tLock lock2(*this);
  GetMonitor().Notify(lock2);
}

} // namespace finroc
} // namespace core

