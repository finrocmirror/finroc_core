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

#ifndef CORE__TEST__TBASICREALTIMETEST_H
#define CORE__TEST__TBASICREALTIMETEST_H

#include "core/port/cc/tPortNumeric.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * A basic real-time test.
 *
 * Apart from using the finroc_core_utils API it doesn't have that
 * much to do with finroc (yet).
 */
class tBasicRealtimeTest : public util::tThread
{
public:

  tPortNumeric port;

  util::tAtomicInt64 max_latency;

  util::tAtomicInt64 total_latency;

  util::tAtomicInt cycles;

  static const int cINTERVAL = 500000;

  tBasicRealtimeTest(const util::tString& name);

  // ns

  static void Main(::finroc::util::tArrayWrapper<util::tString>& args);

  virtual void Run();

  virtual const util::tString ToString() const
  {
    return GetName() + " - Cycles: " + cycles.Get() + "; Max Latency: " + (max_latency.Get() / 1000) + " us; Average Latency: " + (total_latency.Get() / (1000 * std::max(1, cycles.Get()))) + " us";
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TBASICREALTIMETEST_H
