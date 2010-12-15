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
#include "core/port/cc/tCCPortBase.h"
#include "core/tFrameworkElement.h"
#include "plugins/blackboard/tBlackboardManager.h"
#include "core/port/std/tPort.h"
#include "plugins/blackboard/tBlackboardBuffer.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/buffers/tCoreOutput.h"
#include "core/buffers/tCoreInput.h"
#include "core/port/std/tPortDataManager.h"
#include "plugins/blackboard/tBlackboardServer.h"
#include "plugins/blackboard/tRawBlackboardClient.h"
#include "core/port/rpc/tMethodCallException.h"

namespace finroc
{
namespace core
{
const int tRealPortTest5::cNUM_OF_PORTS;
const int tRealPortTest5::cCYCLE_TIME;
::std::tr1::shared_ptr<tPortNumeric> tRealPortTest5::input, tRealPortTest5::output, tRealPortTest5::p1, tRealPortTest5::p2, tRealPortTest5::p3;
tRuntimeEnvironment* tRealPortTest5::re;
const int tRealPortTest5::cCYCLES;

void tRealPortTest5::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  // set up
  //RuntimeEnvironment.initialInit(/*new ByteArrayInputStream(new byte[0])*/);
  re = tRuntimeEnvironment::GetInstance();
  output = ::std::tr1::shared_ptr<tPortNumeric>(new tPortNumeric("test1", true));
  input = ::std::tr1::shared_ptr<tPortNumeric>(new tPortNumeric("test2", false));
  output->ConnectToTarget(*input);
  p1 = ::std::tr1::shared_ptr<tPortNumeric>(new tPortNumeric("p1", false));
  p2 = ::std::tr1::shared_ptr<tPortNumeric>(new tPortNumeric("p2", false));
  p3 = ::std::tr1::shared_ptr<tPortNumeric>(new tPortNumeric("p3", false));
  p3->GetWrapped()->Link(tRuntimeEnvironment::GetInstance(), "portlink");
  tFrameworkElement::InitAll();
  //output.std11CaseReceiver = input;

  //new RealPortTest5().start();
  TestSimpleEdge();
  TestSimpleEdge2();
  TestSimpleEdgeBB();

  input.reset();
  output.reset();

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
  util::tSystem::out.Println(input->GetDoubleRaw());

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
    result = input->GetIntRaw();
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(util::tLong::ToString(time) + " " + result);
}

void tRealPortTest5::TestSimpleEdge2()
{
  blackboard::tBlackboardManager::GetInstance();

  tPort<finroc::blackboard::tBlackboardBuffer> input(tPortCreationInfo("input", tPortFlags::cINPUT_PORT));
  tPort<finroc::blackboard::tBlackboardBuffer> output(tPortCreationInfo("output", tPortFlags::cOUTPUT_PORT));

  output.ConnectToTarget(input);
  tFrameworkElement::InitAll();

  blackboard::tBlackboardBuffer* buf = output.GetUnusedBuffer();
  tCoreOutput co(buf);
  co.WriteInt(42);
  co.Close();
  output.Publish(buf);

  const blackboard::tBlackboardBuffer* cbuf = input.GetLockedUnsafe();
  tCoreInput ci(cbuf);
  util::tSystem::out.Println(ci.ReadInt());
  cbuf->GetManager()->GetCurrentRefCounter()->ReleaseLock();

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
    co.Reset(buf);
    co.WriteInt(i);
    output.Publish(buf);
    co.Close();

    cbuf = input.GetLockedUnsafe();
    ci.Reset(cbuf);
    result = ci.ReadInt();
    cbuf->GetManager()->GetCurrentRefCounter()->ReleaseLock();
  }
  int64 time = util::tSystem::CurrentTimeMillis() - start;
  util::tSystem::out.Println(util::tLong::ToString(time) + " " + result);
}

void tRealPortTest5::TestSimpleEdgeBB()
{
  blackboard::tBlackboardManager::GetInstance();
  __attribute__((unused))
  blackboard::tBlackboardServer* server = new blackboard::tBlackboardServer("testbb");
  //SingleBufferedBlackboardServer server2 = new SingleBufferedBlackboardServer("testbb");
  blackboard::tRawBlackboardClient* client = new blackboard::tRawBlackboardClient(blackboard::tRawBlackboardClient::GetDefaultPci().Derive("testbb"));
  //client.autoConnect();
  tFrameworkElement::InitAll();

  blackboard::tBlackboardBuffer* buf = client->WriteLock(4000000);
  buf->Resize(8, 8, 8, false);
  client->Unlock();

  buf = client->GetUnusedBuffer();
  tCoreOutput co(buf);
  co.WriteInt(0x4BCDEF12);
  co.Close();
  try
  {
    client->CommitAsynchChange(2, buf);
  }
  catch (const tMethodCallException& e)
  {
    e.PrintStackTrace();
  }

  const blackboard::tBlackboardBuffer* cbuf = client->Read();
  tCoreInput ci(cbuf);
  util::tSystem::out.Println(ci.ReadInt());
  cbuf->GetManager()->ReleaseLock();

  int64 start = util::tSystem::CurrentTimeMillis();
  int result = 0;
  int64 size = 0;
  for (int i = 0; i < cCYCLES; i++)
  {
    buf = client->WriteLock(3000000000LL);
    co.Reset(buf);
    co.WriteInt(i);
    co.WriteInt(45);
    co.Close();
    size = buf->GetSize();
    client->Unlock();

    /*cbuf = client.readPart(2, 4, 20000);
    cbuf.getManager().getCurrentRefCounter().releaseLock();*/
    cbuf = client->Read();
    ci.Reset(cbuf);
    result = ci.ReadInt();
    cbuf->GetManager()->ReleaseLock();
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
  util::tSystem::out.Println(time + " " + output->GetIntRaw());
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
