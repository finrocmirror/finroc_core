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
#include "core/tRuntimeEnvironment.h"
#include "plugins/tcp/tTCPServer.h"
#include "core/thread/tThreadContainer.h"
#include "core/test/mTestModule.h"
#include "core/port/cc/tNumberPort.h"

using namespace finroc::core;
using namespace finroc;

int main(int argc__, char **argv__)
{
  // Create TCP server
  tcp::tTCPServer* server = new tcp::tTCPServer(4444, true, NULL);
  server->Init();

  // Create Thread Container
  tThreadContainer* tc = new tThreadContainer(tRuntimeEnvironment::GetInstance(), "tc");
  tc->SetCycleTime(500);

  // Create Modules
  mTestModule* tm1 = new mTestModule(tc, "tm1");
  mTestModule* tm2 = new mTestModule(tc, "tm2");
  mTestModule* tm3 = new mTestModule(tc, "tm3");

  // Add Edges
  tm3->so->ConnectToTarget(tm2->si);
  tm2->so->ConnectToTarget(tm1->si);
  tm1->co->ConnectToTarget(tm2->ci);
  tm2->co->ConnectToTarget(tm3->ci);

  // Initialize created elements
  tc->Init();

  // Start Thread
  tc->StartExecution();

  try
  {
    util::tThread::Sleep(1000000);
  }
  catch (const util::tInterruptedException& e) {}
}
