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
#include "plugins/blackboard/tBlackboardPlugin.h"
#include "core/plugin/tPlugins.h"

#include "core/test/tRealPortTest5.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"
#include "core/tFrameworkElement.h"
#include "plugins/blackboard/tBlackboardManager.h"
#include "plugins/blackboard/tBlackboardBuffer.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/serialization/tInputStream.h"
#include "plugins/blackboard/tSingleBufferedBlackboardServer.h"
#include "rrlib/serialization/tMemoryBuffer.h"
#include "plugins/blackboard/tBlackboardClient.h"
#include "plugins/blackboard/tBlackboardWriteAccess.h"
#include "plugins/blackboard/tBBLockException.h"
#include "core/port/rpc/tMethodCallException.h"

namespace finroc
{
namespace core
{
using rrlib::serialization::tMemoryBuffer;

const int tRealPortTest5::cNUM_OF_PORTS;
const int tRealPortTest5::cCYCLE_TIME;
std::shared_ptr<tPort<int> > tRealPortTest5::input, tRealPortTest5::output, tRealPortTest5::p1, tRealPortTest5::p2, tRealPortTest5::p3;
tRuntimeEnvironment* tRealPortTest5::re;
const int tRealPortTest5::cCYCLES;

void tRealPortTest5::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  // set up
  //RuntimeEnvironment.initialInit(/*new ByteArrayInputStream(new byte[0])*/);
  re = tRuntimeEnvironment::GetInstance();
  output = std::shared_ptr<tPort<int> >(new tPort<int>("test1", tPortFlags::cOUTPUT_PORT));
  input = std::shared_ptr<tPort<int> >(new tPort<int>("test2", tPortFlags::cINPUT_PORT));
  output->ConnectToTarget(*input);
  p1 = std::shared_ptr<tPort<int> >(new tPort<int>("p1", tPortFlags::cINPUT_PORT));
  p2 = std::shared_ptr<tPort<int> >(new tPort<int>("p2", tPortFlags::cINPUT_PORT));
  p3 = std::shared_ptr<tPort<int> >(new tPort<int>("p3", tPortFlags::cINPUT_PORT));
  p3->GetWrapped()->Link(tRuntimeEnvironment::GetInstance(), "portlink");
  tFrameworkElement::InitAll();
  //output.std11CaseReceiver = input;

  //new RealPortTest5().start();
  TestSimpleEdge();
  TestSimpleEdge2();
  TestSimpleEdgeBB();

  input->GetWrapped()->ManagedDelete();
  output->GetWrapped()->ManagedDelete();

  util::tSystem::out.Println("waiting");
}

void tRealPortTest5::Run()
{
  //testSimpleSet();
  //testSimpleEdge();
  //testSimpleEdge2();
  //System.exit(0);
}

void tRealPortTest5::TestSimpleEdge()
{
  /*output = re.addNumberOutputPort("test1");
  input = re.addNumberInputPort("test2");
  output.connectToTarget(input);

  output.connectToTarget(input);*/

  output->Publish(42);

  std::cout << input->Get() << std::endl;

  try
  {
    util::tThread::Sleep(1000);
  }
  catch (const util::tInterruptedException& e)
  {
  }

  int64 start = util::tSystem::CurrentTimeMillis();
  int result = 0;
  for (int i = 0; i < cCYCLES; i++)
  {
    //for (int i = 0; i < 1000000; i++) {
    output->Publish(i);

    result = input->Get();
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(util::tLong::ToString(time) + " " + result);
}

void tRealPortTest5::TestSimpleEdge2()
{
  blackboard::tBlackboardManager::GetInstance();

  tPort<finroc::blackboard::tBlackboardBuffer> input("input", tPortFlags::cINPUT_PORT);
  tPort<finroc::blackboard::tBlackboardBuffer> output("output", tPortFlags::cOUTPUT_PORT);

  output.ConnectToTarget(input);
  tFrameworkElement::InitAll();

  tPortDataPtr<blackboard::tBlackboardBuffer> buf = output.GetUnusedBuffer();
  rrlib::serialization::tOutputStream co(buf.Get());
  co.WriteInt(42);
  co.Close();
  output.Publish(buf);

  const blackboard::tBlackboardBuffer* cbuf = input.GetAutoLocked();
  rrlib::serialization::tInputStream ci(cbuf);
  util::tSystem::out.Println(ci.ReadInt());
  input.ReleaseAutoLocks();

  try
  {
    util::tThread::Sleep(1000);
  }
  catch (const util::tInterruptedException& e)
  {
  }

  int64 start = util::tSystem::CurrentTimeMillis();
  int result = 0;
  for (int i = 0; i < cCYCLES; i++)
  {
    buf = output.GetUnusedBuffer();
    co.Reset(buf.get());
    co.WriteInt(i);
    output.Publish(buf);
    co.Close();

    cbuf = input.GetAutoLocked();
    ci.Reset(cbuf);
    result = ci.ReadInt();
    input.ReleaseAutoLocks();
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(util::tLong::ToString(time) + " " + result);
}

void tRealPortTest5::TestSimpleEdgeBB()
{
  blackboard::tBlackboardManager::GetInstance();
  __attribute__((unused))
  blackboard::tSingleBufferedBlackboardServer<rrlib::serialization::tMemoryBuffer>* server2 = new blackboard::tSingleBufferedBlackboardServer<rrlib::serialization::tMemoryBuffer>("testbb");
  blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer> client("testbb", NULL, false);
  //client.autoConnect();
  tFrameworkElement::InitAll();

  try
  {
    blackboard::tBlackboardWriteAccess<rrlib::serialization::tMemoryBuffer> bbw(client, 4000000);
    bbw.Resize(8u);

    rrlib::serialization::tOutputStream co(&(bbw[0]));
    co.WriteLong(0);
    co.Close();
  }
  catch (const blackboard::tBBLockException& ex)
  {
  }

  tPortDataPtr<std::vector<tMemoryBuffer> > buf = client.GetUnusedChangeBuffer();
  rrlib::serialization::sSerialization::ResizeVector(*buf, 1);
  rrlib::serialization::tOutputStream co(&buf->at(0));
  co.WriteInt(0x4BCDEF12);
  co.Close();
  try
  {
    client.CommitAsynchChange(buf, 0, 2);
  }
  catch (const tMethodCallException& e)
  {
    e.PrintStackTrace();
  }

  buf.reset();

  tPortDataPtr<const std::vector<tMemoryBuffer> > cbuf = client.Read();
  rrlib::serialization::tInputStream ci(&cbuf->at(0));
  util::tSystem::out.Println(ci.ReadInt());

  cbuf.reset();

  int64 start = util::tSystem::CurrentTimeMillis();
  int result = 0;
  int64 size = 0;
  for (int i = 0; i < cCYCLES; i++)
  {
    std::vector<tMemoryBuffer>* buf3 = client.WriteLock(300000000);

    co.Reset(&buf3->at(0));

    co.WriteInt(i);
    co.WriteInt(45);
    co.Close();

    size = buf3->at(0).GetSize();

    client.Unlock();

    /*cbuf = client.readPart(2, 4, 20000);
    cbuf.getManager().getCurrentRefCounter().releaseLock();*/
    cbuf = client.Read();

    ci.Reset(&cbuf->at(0));

    result = ci.ReadInt();

    cbuf.reset();
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(util::tLong::ToString(time) + " " + result + " " + size);
  tRuntimeEnvironment::GetInstance()->PrintStructure();
}

void tRealPortTest5::TestSimpleSet()
{
  try
  {
    util::tThread::Sleep(1000);
  }
  catch (const util::tInterruptedException& e)
  {
  }

  int64 start = util::tSystem::CurrentTimeMillis();
  for (int i = 1; i < cCYCLES + 1; i++)
  {
    output->Publish(i);
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;

  std::cout << time << " " << output->Get() << std::endl;
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
  ::finroc::core::tRealPortTest5::Main(sa);
}
