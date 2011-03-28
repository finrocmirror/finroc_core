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
#include "rrlib/finroc_core_utils/tTime.h"

namespace finroc
{
namespace core
{
const int tBasicRealtimeTest::cINTERVAL;

tBasicRealtimeTest::tBasicRealtimeTest(const util::tString& name) :
    port(name + "-port", NULL, true),
    max_latency(),
    total_latency(),
    cycles()
{
  port.Init();
  SetName(name);
}

void tBasicRealtimeTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  tRuntimeEnvironment::GetInstance();

  std::shared_ptr<tBasicRealtimeTest> rt = util::sThreadUtil::GetThreadSharedPtr(new tBasicRealtimeTest("RT-Thread"));
  std::shared_ptr<tBasicRealtimeTest> t = util::sThreadUtil::GetThreadSharedPtr(new tBasicRealtimeTest("non-RT-Thread"));
  util::sThreadUtil::MakeThreadRealtime(rt);
  rt->Start();
  t->Start();

  while (true)
  {
    util::tSystem::out.Println(rt->ToString() + "   " + t->ToString());
    try
    {
      ::finroc::util::tThread::Sleep(1000);
    }
    catch (const util::tInterruptedException& e)
    {
      break;
    }
  }
}

void tBasicRealtimeTest::Run()
{
  tThreadLocalCache::Get();  // init ThreadLocalCache
  port.Publish(40);
  port.Publish(42);

  int64 next = util::tTime::NanoTime() + util::tTime::cNSEC_PER_SEC;
  while (true)
  {
    util::tTime::SleepUntilNano(next);
    int64 diff = util::tTime::NanoTime() - next;
    if (max_latency.Get() < diff)
    {
      max_latency.Set(diff);
    }
    total_latency.Set(total_latency.Get() + diff);
    int c = cycles.IncrementAndGet();
    port.Publish(c);
    next += cINTERVAL;
  }
}

} // namespace finroc
} // namespace core

int main(int argc__, char **argv__)
{
  ::finroc::util::tArrayWrapper< ::finroc::util::tString> sa(argc__ <= 0 ? 0 : (argc__ - 1));
  for (int i = 1; i < argc__; i++)
  {
    sa[i - 1] = ::finroc::util::tString(argv__[i]);
  }
  ::finroc::core::tBasicRealtimeTest::Main(sa);
}
