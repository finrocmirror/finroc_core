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
#include "core/test/tRealServerTest.h"
#include "tcp/tTCPServer.h"
#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
tRealServerTest::tRealServerTest(int stop_cycle) :
    tNetworkTestSuite("ServerBlackboard", "ClientBlackboard", stop_cycle)
{
}

void tRealServerTest::Main()
{
  // create Server
  tcp::tTCPServer* server = new tcp::tTCPServer(4444, true, NULL);
  server->GetDescription();  // dummy instruction... avoids warning
  tFrameworkElement::InitAll();
  tRuntimeEnvironment::GetInstance()->PrintStructure();

  ::finroc::core::tNetworkTestSuite::MainLoop();
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
  ::finroc::core::tRealServerTest::Main(sa);
}
