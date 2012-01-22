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
#include "core/test/tRealPortQueueTest.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/tFrameworkElement.h"
#include "core/datatype/tNumber.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"

namespace finroc
{
namespace core
{
int tRealPortQueueTest::cCYCLES = 10000000;
std::shared_ptr<tPort<int> > tRealPortQueueTest::output;
volatile int tRealPortQueueTest::cPUBLISH_LIMIT;
util::tAtomicInt tRealPortQueueTest::finished(0);

tRealPortQueueTest::tRealPortQueueTest(bool pos) :
  positive_count(pos)
{
}

void tRealPortQueueTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  // Create number output port and input port with queue
  tRuntimeEnvironment::GetInstance();
  tThreadLocalCache::Get();
  output = std::shared_ptr<tPort<int> >(new tPort<int>("output", tPortFlags::cOUTPUT_PORT));
  tPortCreationInfo<int> input_pCI("input", tPortFlags::cINPUT_PORT | tPortFlags::cHAS_AND_USES_QUEUE | tPortFlags::cPUSH_STRATEGY);
  input_pCI.max_queue_size = 10;
  tPort<int> input(input_pCI);
  input_pCI.max_queue_size = 0;
  tPort<int> unlimited_input(input_pCI);
  tPort<int> unlimited_input2(input_pCI);
  output->ConnectToTarget(input);
  tFrameworkElement::InitAll();
  tRuntimeEnvironment::GetInstance()->PrintStructure();

  util::tSystem::out.Println("test writing a lot to port...");
  int64 start = util::tSystem::CurrentTimeMillis();
  for (int i = 0; i < cCYCLES; i++)
  {
    output->Publish(i);
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(time);

  util::tSystem::out.Println("Reading contents of queue (single dq)...");
  int cn = 0;
  while ((input.Dequeue(cn)))
  {
    PrintNum(cn);
  }

  util::tSystem::out.Println("Writing two entries to queue...");
  for (int i = 0; i < 2; i++)
  {
    output->Publish(i);
  }

  util::tSystem::out.Println("Reading contents of queue (single dq)...");
  while ((input.Dequeue(cn)))
  {
    PrintNum(cn);
  }

  util::tSystem::out.Println("Writing 20 entries to queue...");
  for (int i = 0; i < 20; i++)
  {
    output->Publish(i);
  }

  util::tSystem::out.Println("Read contents of queue in fragment...");
  tPortQueueFragment<int> frag;
  input.DequeueAll(frag);
  while (frag.Dequeue(cn))
  {
    PrintNum(cn);
  }
  tThreadLocalCache::Get()->ReleaseAllLocks();

  util::tSystem::out.Println("Read contents of queue in fragment again...");
  input.DequeueAll(frag);
  while (frag.Dequeue(cn))
  {
    PrintNum(cn);
  }
  tThreadLocalCache::Get()->ReleaseAllLocks();

  util::tSystem::out.Println("Writing 3 entries to queue...");
  for (int i = 0; i < 3; i++)
  {
    output->Publish(i);
  }

  util::tSystem::out.Println("Read contents of queue in fragment...");
  input.DequeueAll(frag);
  while (frag.Dequeue(cn))
  {
    PrintNum(cn);
  }
  tThreadLocalCache::Get()->ReleaseAllLocks();

  // now concurrently :-)
  util::tSystem::out.Println("\nAnd now for Concurrency :-)  ...");

  // connect to unlimited input
  output->ConnectToTarget(unlimited_input);
  output->ConnectToTarget(unlimited_input2);

  // remove values from initial push
  unlimited_input.DequeueAutoLocked();
  unlimited_input2.DequeueAutoLocked();
  tThreadLocalCache::GetFast()->ReleaseAllLocks();

  // start writer threads
  std::shared_ptr<tRealPortQueueTest> thread1 = util::sThreadUtil::GetThreadSharedPtr(new tRealPortQueueTest(true));
  std::shared_ptr<tRealPortQueueTest> thread2 = util::sThreadUtil::GetThreadSharedPtr(new tRealPortQueueTest(false));
  printf("Created threads %p and %p\n", thread1.get(), thread2.get());
  thread1->Start();
  thread2->Start();

  int last_pos_limited = 0;
  int last_neg_limited = 0;
  int last_pos_unlimited = 0;
  int last_neg_unlimited = 0;
  int last_pos_unlimited_f = 0;
  int last_neg_unlimited_f = 0;

  int e = cCYCLES - 1;
  start = util::tSystem::CurrentTimeMillis();
  cPUBLISH_LIMIT = cCYCLES;
  int cc = 0;
  while (true)
  {
    if ((last_pos_unlimited & 0xFFFF) == 0)
    {
      cPUBLISH_LIMIT = last_pos_unlimited + 70000;
    }

    // Dequeue from bounded queue
    if (input.Dequeue(cc))
    {
      int val = cc;
      if (val >= 0)
      {
        assert((val > last_pos_limited));
        last_pos_limited = val;
      }
      else
      {
        assert((val < last_neg_limited));
        last_neg_limited = val;
      }
    }

    // Dequeue from unlimited queue (single dq)
    if (unlimited_input.Dequeue(cc))
    {
      int val = cc;
      if (val >= 0)
      {
        assert((val == last_pos_unlimited + 1));
        last_pos_unlimited = val;
      }
      else
      {
        assert((val == last_neg_unlimited - 1));
        last_neg_unlimited = val;
      }
    }

    //          if ((lastPosLimited == e || lastNegLimited == -e) && lastPosUnlimited == e /*&& lastNegUnlimited == -e*()*/) {
    //              System.out.println("Yeah! Check Completed");
    //              break;
    //          }

    // Dequeue from unlimited queue (fragment-wise)
    //System.out.println("Iteratorion");
    unlimited_input2.DequeueAll(frag);
    while (frag.Dequeue(cc))
    {
      int val = cc;
      if (val >= 0)
      {
        assert((val == last_pos_unlimited_f + 1));
        last_pos_unlimited_f = val;
      }
      else
      {
        assert((val == last_neg_unlimited_f - 1));
        last_neg_unlimited_f = val;
      }
    }

    if ((last_pos_limited == e || last_neg_limited == -e) && last_pos_unlimited == e && last_neg_unlimited == -e && last_pos_unlimited_f == e && last_neg_unlimited_f == -e)
    {
      util::tSystem::out.Println("Yeah! Check Completed");
      break;
    }
  }
  time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(time);
  finished.Set(1);
}

void tRealPortQueueTest::Run()
{
  tThreadLocalCache::Get();
  for (int i = 1; i < cCYCLES; i++)
  {
    while (i > cPUBLISH_LIMIT)
    {
    }
    output->Publish(positive_count ? i : -i);
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
  ::finroc::core::tRealPortQueueTest::Main(sa);
}
