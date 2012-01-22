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
#include "core/port/rpc/tRPCThreadPool.h"

#include "core/port/rpc/tRPCThread.h"
#include "rrlib/finroc_core_utils/thread/tLoopThread.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"

namespace finroc
{
namespace core
{
tRPCThread::tRPCThread() :
  tCoreLoopThreadBase(0),
  next_task(NULL)
{
}

void tRPCThread::ExecuteTask(util::tTask* t)
{
  util::tLock lock1(this);
  assert((next_task == NULL));
  next_task = t;
  monitor.NotifyAll(lock1);
}

void tRPCThread::MainLoopCallback()
{
  {
    util::tLock lock2(this);
    if (next_task == NULL)
    {
      tRPCThreadPool::GetInstance()->EnqueueThread(this);

      if (!IsStopSignalSet())
      {
        monitor.Wait(lock2);
      }
    }
  }
  while (next_task != NULL)
  {
    util::tTask* tmp = next_task;
    next_task = NULL;
    tmp->ExecuteTask();
  }
}

} // namespace finroc
} // namespace core

