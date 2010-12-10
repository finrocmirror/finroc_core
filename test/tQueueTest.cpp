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
#include "core/test/tQueueTest.h"
#include "rrlib/finroc_core_utils/container/tWonderQueue.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueFast.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueFastCR.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueTL.h"

namespace finroc
{
namespace core
{
void tQueueTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  TestTL();
  Test();
  TestFast();
  TestFastCR();
  TestBounded();
}

void tQueueTest::Test()
{
  util::tSystem::out.Println("\nWonderQueue");
  util::tSystem::out.Println("expected: null q1 q2 q3 null q1 q2 q3 null q1 q2 q3 q1");
  util::tWonderQueue<tNamedQueueable> q;

  // common test
  tNamedQueueable* q1 = new tNamedQueueable("q1");
  tNamedQueueable* q2 = new tNamedQueueable("q2");
  tNamedQueueable* q3 = new tNamedQueueable("q3");
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());

}

void tQueueTest::TestBounded()
{
  util::tSystem::out.Println("\nWonderQueueBounded");
  util::tSystem::out.Println("expected: null q1 q2 q3 null q1 q2 q3 null q1 q2 q3 q1");
  tTestWonderQueueBounded q;
  q.Init();

  // common test
  tNamedQueueable* q1 = new tNamedQueueable("q1");
  tNamedQueueable* q2 = new tNamedQueueable("q2");
  tNamedQueueable* q3 = new tNamedQueueable("q3");
  util::tSystem::out.Println(*q.Dequeue());
  q.EnqueueWrapped(q1);
  q.EnqueueWrapped(q2);
  q.EnqueueWrapped(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.EnqueueWrapped(q1);
  q.EnqueueWrapped(q2);
  q.EnqueueWrapped(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.EnqueueWrapped(q1);
  q.EnqueueWrapped(q2);
  q.EnqueueWrapped(q3);
  util::tSystem::out.Println(*q.Dequeue());
  q.EnqueueWrapped(q1);
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());

}

void tQueueTest::TestFast()
{
  util::tSystem::out.Println("\nWonderQueueFast");
  util::tSystem::out.Println("expected: null q1 q2 null null q3 q1 q2 null q3 q1 q2 q3");
  util::tWonderQueueFast<tNamedQueueable> q;

  // common test
  tNamedQueueable* q1 = new tNamedQueueable("q1");
  tNamedQueueable* q2 = new tNamedQueueable("q2");
  tNamedQueueable* q3 = new tNamedQueueable("q3");
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());

}

void tQueueTest::TestFastCR()
{
  util::tSystem::out.Println("\nWonderQueueFastCR");
  util::tSystem::out.Println("expected: null q1 q2 null null q3 q1 q2 null q3 q1 q2 q3");
  util::tWonderQueueFastCR<tNamedQueueable> q;

  // common test
  tNamedQueueable* q1 = new tNamedQueueable("q1");
  tNamedQueueable* q2 = new tNamedQueueable("q2");
  tNamedQueueable* q3 = new tNamedQueueable("q3");
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());

}

void tQueueTest::TestTL()
{
  util::tSystem::out.Println("\nWonderQueueTL");
  util::tSystem::out.Println("expected: null q1 q2 q3 null q1 q2 q3 null q1 q2 q3 q1");
  util::tWonderQueueTL<tNamedQueueable> q;

  // common test
  tNamedQueueable* q1 = new tNamedQueueable("q1");
  tNamedQueueable* q2 = new tNamedQueueable("q2");
  tNamedQueueable* q3 = new tNamedQueueable("q3");
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  for (int i = 0; i < 4; i++)
  {
    util::tSystem::out.Println(*q.Dequeue());
  }

  q.Enqueue(q1);
  q.Enqueue(q2);
  q.Enqueue(q3);
  util::tSystem::out.Println(*q.Dequeue());
  q.Enqueue(q1);
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
  util::tSystem::out.Println(*q.Dequeue());
}

tQueueTest::tNamedQueueable::tNamedQueueable(const util::tString& name_) :
    name(name_)
{
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
  ::finroc::core::tQueueTest::Main(sa);
}
