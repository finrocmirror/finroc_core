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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__TEST__TREALPORTTEST5_H
#define CORE__TEST__TREALPORTTEST5_H

#include "core/port/cc/tNumberPort.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
class tRealPortTest5 : public util::tUncopyableObject
{
private:
  /*extends CoreThreadBase*/

  static const int cNUM_OF_PORTS = 1000;

  static const int cCYCLE_TIME = 3;

  static const int cCYCLES = 100000;

public:

  static tNumberPort* input, * output, * p1, * p2, * p3;

  static tRuntimeEnvironment* re;

  tRealPortTest5() {}

  static void Main(::finroc::util::tArrayWrapper<util::tString>& args);

  void Run();

  // test 100.000.000 set and get operations with two CC ports over a simple edge
  static void TestSimpleEdge();

  // test 100.000.000 set and get operations with two ports over a simple edge
  static void TestSimpleEdge2();

  // test 100.000.000 set and get operations with two ports over a simple edge
  static void TestSimpleEdgeBB();

  // test 100.000.000 set operation without connection
  static void TestSimpleSet();

//
//
//  public static class TestModule extends Module {
//
//      DoublePort p1 = new DoublePort("p1", new PortCreationInfo(PortFlags.OUTPUT_PORT));
//      DoublePort p2 = new DoublePort("p2", new PortCreationInfo(PortFlags.OUTPUT_PORT));
//      //IntPort ip = new IntPort(new PortCreationInfo(PortFlags.INPUT_PORT, "ip"));
//      DoublePort ip = new DoublePort("ip", new PortCreationInfo(PortFlags.INPUT_PORT));
//
//
//      public TestModule(CoreEventThread ct) {
//          super(true);
//          setDescription("module");
//          //setEventThread(ct);
//          /*p1.setValuesBeforeAreUnused(Long.MAX_VALUE);
//          p2.setValuesBeforeAreUnused(Long.MAX_VALUE);
//          ip.setValuesBeforeAreUnused(Long.MAX_VALUE);*/
//          addChild(p1);
//          addChild(p2);
//          addChild(ip);
//      }
//
//      @Override
//      protected void update() {
//      }
//
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TREALPORTTEST5_H
