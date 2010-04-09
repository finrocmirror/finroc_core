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
#include "core/port/stream/tStreamCommitThread.h"
#include "core/tRuntimeSettings.h"

namespace finroc
{
namespace core
{
::std::tr1::shared_ptr<tStreamCommitThread> tStreamCommitThread::instance;

tStreamCommitThread::tStreamCommitThread() :
    tCoreLoopThreadBase(tRuntimeSettings::cSTREAM_THREAD_CYCLE_TIME->Get(), false),
    callbacks(4u, 4u)
{
  SetName("StreamCommitThread");
}

void tStreamCommitThread::MainLoopCallback()
{
  int64 time = util::tSystem::CurrentTimeMillis();
  util::tArrayWrapper<tCallback*>* iterable = callbacks.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tCallback* cb = iterable->Get(i);
    if (cb != NULL)
    {
      cb->StreamThreadCallback(time);
    }
  }
}

void tStreamCommitThread::StopThread()
{
  util::tSystem::out.Println("Stopping StreamCommitThread\n");
  ::finroc::util::tLoopThread::StopThread();
  instance.reset();
}

} // namespace finroc
} // namespace core

