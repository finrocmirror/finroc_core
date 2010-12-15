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

#ifndef CORE__TEST__TREALPORTQUEUETEST_H
#define CORE__TEST__TREALPORTQUEUETEST_H

#include "core/port/cc/tPortNumeric.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Test for ports with bounded queues
 */
class tRealPortQueueTest : public util::tThread
{
public:

  // Number of iterations
  static int cCYCLES;

  static ::std::tr1::shared_ptr<tPortNumeric> output;

  static volatile int cPUBLISH_LIMIT;

  /*! publish positive numbers? */
  bool positive_count;

  static util::tAtomicInt finished;

  tRealPortQueueTest(bool pos);

  static void Main(::finroc::util::tArrayWrapper<util::tString>& args);

  virtual void Run();

};

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TREALPORTQUEUETEST_H
