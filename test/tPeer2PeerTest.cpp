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
#include "core/test/tPeer2PeerTest.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/tFrameworkElement.h"
#include "core/port/tPort.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/port/tAbstractPort.h"
#include "core/tCoreFlags.h"
#include "plugins/tcp/tTCPPeer.h"

using namespace rrlib::logging;

namespace finroc
{
namespace core
{
void tPeer2PeerTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  tThreadLocalCache::Get();

  // Create two ports
  tFrameworkElement* link_test = new tFrameworkElement(NULL, "linkTest");
  tPort<int> output("testOut", tPortFlags::cSHARED_OUTPUT_PORT);
  output.GetWrapped()->Link(link_test, "linkTestPort");
  tPort<int> output2("testOutGlobal", tPortFlags::cSHARED_OUTPUT_PORT | tCoreFlags::cGLOBALLY_UNIQUE_LINK);
  tPort<int> input("testIn", tPortFlags::cINPUT_PORT);
  input.ConnectToSource("/TCP/localhost:4444/Unrelated/testOut");
  input.ConnectToSource("/Unrelated/testOutGlobal");

  // Create TCP peer
  util::tString addr = "localhost:4444";
  if (args.length > 0)
  {
    addr = args[0];
  }
  tcp::tTCPPeer* peer = new tcp::tTCPPeer(addr, "", tcp::tTCPPeer::eFULL, 4444, tcp::tTCPPeer::cDEFAULT_FILTER, false);
  tFrameworkElement::InitAll();
  output.Publish(4);
  output2.Publish(5);
  try
  {
    peer->Connect();
  }
  catch (const util::tException& e1)
  {
    e1.PrintStackTrace();
  }

  // Print output every 10 seconds for one minute
  for (int i = 0; i < 1; i++)
  {
    try
    {
      util::tThread::Sleep(10000);
    }
    catch (const util::tException& e)
    {
      e.PrintStackTrace();
    }
    re->PrintStructure();
    FINROC_LOG_PRINT(eLL_USER, util::tStringBuilder("Input connections: ") + input.GetConnectionCount());
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
  ::finroc::core::tPeer2PeerTest::Main(sa);
}
