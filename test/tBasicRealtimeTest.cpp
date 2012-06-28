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
#include "core/test/tBasicRealtimeTest.h"
#include "core/tRuntimeEnvironment.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/time/time.h"

using namespace finroc::core;
using namespace finroc;
using namespace rrlib::logging;

rrlib::time::tDuration tBasicRealtimeTest::cINTERVAL = std::chrono::microseconds(500);

tBasicRealtimeTest::tBasicRealtimeTest(const util::tString& name) :
  tLoopThread(cINTERVAL),
  port(name + "-port", tPortFlags::cOUTPUT_PORT),
  max_latency(rrlib::time::tDuration(0)),
  total_latency(rrlib::time::tDuration(0)),
  cycles()
{
  port.Init();
  SetName(name);
}

void tBasicRealtimeTest::Run()
{
  tThreadLocalCache::Get();  // init ThreadLocalCache
  port.Publish(40);
  port.Publish(42);

  tLoopThread::Run();
}

void tBasicRealtimeTest::MainLoopCallback()
{
  rrlib::time::tDuration diff = rrlib::time::Now() - tLoopThread::GetCurrentCycleStartTime();
  if (max_latency.Load() < diff)
  {
    max_latency.Store(diff);
  }
  total_latency.Store(total_latency.Load() + diff);
  int c = cycles.IncrementAndGet();
  port.Publish(c);
}

int main(int argc__, char **argv__)
{
  tRuntimeEnvironment::GetInstance();

  std::shared_ptr<tBasicRealtimeTest> rt = util::sThreadUtil::GetThreadSharedPtr(new tBasicRealtimeTest("RT-Thread"));
  std::shared_ptr<tBasicRealtimeTest> t = util::sThreadUtil::GetThreadSharedPtr(new tBasicRealtimeTest("non-RT-Thread"));
  util::sThreadUtil::MakeThreadRealtime(rt);
  rt->Start();
  t->Start();

  while (true)
  {
    FINROC_LOG_PRINT(eLL_USER, rt->ToString() + "   " + t->ToString());
    util::tThread::Sleep(std::chrono::seconds(1), false);
  }
}
