/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "core/finstructable/pFinstructed.h"
#include "core/tRuntimeEnvironment.h"
#include "tcp/tTCPServer.h"
#include "core/finstructable/tFinstructableGroup.h"

namespace finroc
{
namespace core
{
void pFinstructed::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  tcp::tTCPServer* server = new tcp::tTCPServer(4444, true, NULL);
  server->Init();   // TODO: remove

  tFinstructableGroup* fg = new tFinstructableGroup("finstructed", re, "etc/finstructed_test.xml");
  fg->Init();

  while (true)
  {
    try
    {
      util::tThread::Sleep(10000);
    }
    catch (const util::tInterruptedException& e)
    {
    }
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
  ::finroc::core::pFinstructed::Main(sa);
}
