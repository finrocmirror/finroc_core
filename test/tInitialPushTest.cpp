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
#include "core/test/tInitialPushTest.h"
#include "core/tRuntimeEnvironment.h"
#include "plugins/blackboard/tBlackboardBuffer.h"
#include "core/port/tPortCreationInfo.h"
#include "rrlib/serialization/tMemoryBuffer.h"
#include "core/port/tPortFlags.h"
#include "core/tFrameworkElement.h"
#include "core/buffers/tCoreOutput.h"
#include "rrlib/serialization/tFixedBuffer.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
void tInitialPushTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  // setup and initialize ports
  //ThreadLocalCache.get();
  tRuntimeEnvironment::GetInstance();
  tPort<blackboard::tBlackboardBuffer> out(tPortCreationInfo("StdOut", rrlib::serialization::tMemoryBuffer::cTYPE, tPortFlags::cOUTPUT_PORT));
  tPort<blackboard::tBlackboardBuffer> in(tPortCreationInfo("StdIn", rrlib::serialization::tMemoryBuffer::cTYPE, tPortFlags::cINPUT_PORT));
  tPort<int> n_out(tPortCreationInfo("NumOut", tPortFlags::cOUTPUT_PORT));
  tPort<int> n_in(tPortCreationInfo("NumIn", tPortFlags::cINPUT_PORT));
  tPort<int> n_rev_out(tPortCreationInfo("NumRevOut", tPortFlags::cOUTPUT_PORT | tPortFlags::cPUSH_STRATEGY_REVERSE));
  tFrameworkElement::InitAll();

  // fill output ports with something
  n_out.Publish(23);
  tPortDataPtr<blackboard::tBlackboardBuffer> bb = out.GetUnusedBuffer();
  tCoreOutput co;
  co.Reset(bb.get());
  co.WriteInt(23);
  co.Close();
  out.Publish(bb);

  // connect input ports
  n_in.ConnectToSource(n_out);
  n_rev_out.ConnectToTarget(n_in);
  in.ConnectToSource(out);

  // print output
  util::tSystem::out.Println(util::tStringBuilder("NumIn (exp 23): ") + n_in.GetValue());
  util::tSystem::out.Println(util::tStringBuilder("NumRevOut (exp 23): ") + n_rev_out.GetValue());
  const blackboard::tBlackboardBuffer* bb2 = in.GetAutoLocked();
  util::tSystem::out.Println(util::tStringBuilder("StdIn (exp 23): ") + bb2->GetBuffer()->GetInt(0u));
  tThreadLocalCache::GetFast()->ReleaseAllLocks();

  // strategy changes...
  n_in.SetPushStrategy(false);
  n_out.Publish(42);
  //System.out.println("NumIn: " + nIn.getDoubleRaw());
  n_in.SetPushStrategy(true);
  util::tSystem::out.Println(util::tStringBuilder("NumIn (expected 23 - because we have two sources => no push): ") + n_in.GetValue());
  util::tSystem::out.Println(util::tStringBuilder("NumRevOut (exp 23): ") + n_rev_out.GetValue());
  n_rev_out.SetReversePushStrategy(false);
  n_out.Publish(12);
  util::tSystem::out.Println(util::tStringBuilder("NumRevOut (exp 23): ") + n_rev_out.GetValue());
  n_rev_out.SetReversePushStrategy(true);
  util::tSystem::out.Println(util::tStringBuilder("NumRevOut (exp 12): ") + n_rev_out.GetValue());

  // now for a complex net
  util::tSystem::out.Println("\nNow for a complex net...");

  // o1->o2
  tPort<int> o1(tPortCreationInfo("o1", tPortFlags::cOUTPUT_PROXY));
  tFrameworkElement::InitAll();
  o1.Publish(24);
  tPort<int> o2(tPortCreationInfo("o2", tPortFlags::cINPUT_PROXY | tPortFlags::cPUSH_STRATEGY));
  tFrameworkElement::InitAll();
  o1.ConnectToTarget(o2);
  Print(o2, 24);

  // o1->o2->o3
  tPort<int> o3(tPortCreationInfo("o3", tPortFlags::cINPUT_PORT));
  o2.ConnectToTarget(o3);
  tFrameworkElement::InitAll();
  o2.SetPushStrategy(false);
  o3.SetPushStrategy(false);
  o1.Publish(22);
  //print(o3, 24); ok pulled
  o3.SetPushStrategy(true);
  Print(o3, 22);

  // o0->o1->o2->o3
  tPort<int> o0(tPortCreationInfo("o0", tPortFlags::cOUTPUT_PROXY));
  tFrameworkElement::InitAll();
  o0.Publish(42);
  o0.ConnectToTarget(o1);
  Print(o3, 42);

  // o6->o0->o1->o2->o3
  //              \            .
  //               o4->o5
  tPort<int> o4(tPortCreationInfo("o4", tPortFlags::cINPUT_PROXY));
  tPort<int> o5(tPortCreationInfo("o5", tPortFlags::cINPUT_PORT));
  tFrameworkElement::InitAll();
  o4.ConnectToTarget(o5);
  o2.ConnectToTarget(o4);
  Print(o5, 42);
  tPort<int> o6(tPortCreationInfo("o6", tPortFlags::cOUTPUT_PORT));
  tFrameworkElement::InitAll();
  o6.Publish(44);
  o6.ConnectToTarget(o0);
  Print(o3, 44);
  Print(o5, 44);

  // o6->o0->o1->o2->o3
  //        /     \            .
  //      o7->o8   o4->o5
  tPort<int> o7(tPortCreationInfo("o7", tPortFlags::cOUTPUT_PROXY));
  tFrameworkElement::InitAll();
  o7.Publish(33);
  tPort<int> o8(tPortCreationInfo("o8", tPortFlags::cINPUT_PORT));
  tFrameworkElement::InitAll();
  o7.ConnectToTarget(o8);
  Print(o8, 33);
  o7.ConnectToTarget(o1);
  Print(o1, 44);

  // o6->o0->o1->o2->o3
  //        /     \            .
  //  o9->o7->o8   o4->o5
  tPort<int> o9(tPortCreationInfo("o9", tPortFlags::cOUTPUT_PORT));
  tFrameworkElement::InitAll();
  o9.Publish(88);
  o9.ConnectToTarget(o7);
  Print(o8, 88);
  Print(o1, 44);
  Print(o3, 44);
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
  ::finroc::core::tInitialPushTest::Main(sa);
}
