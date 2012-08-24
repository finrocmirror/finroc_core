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
#include "core/test/tRealClientTest.h"
#include "core/port/tPort.h"
#include "plugins/tcp/tTCPPeer.h"
#include "plugins/tcp/tTCP.h"
#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
tRealClientTest::tRealClientTest(int stop_cycle) :
  tNetworkTestSuite("ClientBlackboard", "ServerBlackboard", stop_cycle)
{
}

void tRealClientTest::Main()
{
  // connect ports
  if (cCC_TESTS)
  {
    if (cPUSH_TESTS)
    {
      this->cc_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/CCPush Input");
      this->cc_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/CCPush Output");
    }
    if (cPULL_PUSH_TESTS)
    {
      this->cc_pull_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/CCPullPush Input");
      this->cc_pull_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/CCPullPush Output");
    }
    if (cREVERSE_PUSH_TESTS)
    {
      this->cc_rev_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/CCRevPush Input");
      this->cc_rev_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/CCRevPush Output");
    }
    if (cQ_TESTS)
    {
      this->cc_qOut->ConnectTo("/TCP/localhost:4444/Unrelated/CCPush Queue Input");
      this->cc_qIn->ConnectTo("/TCP/localhost:4444/Unrelated/CCPush Queue Output");
    }
  }

  // connect ports
  if (cSTD_TESTS)
  {
    if (cPUSH_TESTS)
    {
      this->std_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/StdPush Input");
      this->std_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/StdPush Output");
    }
    if (cPULL_PUSH_TESTS)
    {
      this->std_pull_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/StdPullPush Input");
      this->std_pull_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/StdPullPush Output");
    }
    if (cREVERSE_PUSH_TESTS)
    {
      this->std_rev_push_out->ConnectTo("/TCP/localhost:4444/Unrelated/StdRevPush Input");
      this->std_rev_push_in->ConnectTo("/TCP/localhost:4444/Unrelated/StdRevPush Output");
    }
    if (cQ_TESTS)
    {
      this->std_qOut->ConnectTo("/TCP/localhost:4444/Unrelated/StdPush Queue Input");
      this->std_qIn->ConnectTo("/TCP/localhost:4444/Unrelated/StdPush Queue Output");
    }
  }

  // create Client
  tcp::tTCPPeer* client = new tcp::tTCPPeer(tcp::tTCP::cDEFAULT_CONNECTION_NAME, tcp::tTCPPeer::cGUI_FILTER);
  tFrameworkElement::InitAll();
  tRuntimeEnvironment::GetInstance()->PrintStructure();
  try
  {
    //client.connect("rrlab-test-net");
    client->Connect();
    //PeerList fpl = AutoDeleter.addStatic(new PeerList());
    //fpl.addPeer(new IPSocketAddress(IPAddress.getByName("localhost"), 4444));
    rrlib::thread::tThread::Sleep(std::chrono::seconds(2), true);
  }
  catch (const util::tException& e1)
  {
    e1.PrintStackTrace();
    return;
  }
  tRuntimeEnvironment::GetInstance()->PrintStructure();
  ::finroc::core::tNetworkTestSuite::MainLoop();
}

} // namespace finroc
} // namespace core

int main(int argc, char **argv)
{
  finroc::core::tRealClientTest rct(argc == 2 ? atoi(argv[1]) : -1);
  rct.Main();
  return 0;
}
