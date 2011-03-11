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
#ifndef core__test__tRealPortTest5_h__
#define core__test__tRealPortTest5_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPort.h"

namespace finroc
{
namespace core
{
class tRuntimeEnvironment;

class tRealPortTest5 : public util::tUncopyableObject
{
private:
  /*extends CoreThreadBase*/

  static const int cNUM_OF_PORTS = 1000;

  static const int cCYCLE_TIME = 3;

  static const int cCYCLES = 10000000;

public:

  static ::std::shared_ptr<tPort<int> > input, output, p1, p2, p3;

  static tRuntimeEnvironment* re;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "test");

  tRealPortTest5() {}

  inline static util::tString GetLogDescription()
  {
    return "RealPortTest";
  }

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

};

} // namespace finroc
} // namespace core

#endif // core__test__tRealPortTest5_h__
