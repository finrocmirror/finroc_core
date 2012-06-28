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

#ifndef core__test__tBasicRealtimeTest_h__
#define core__test__tBasicRealtimeTest_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPort.h"

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
class tBasicRealtimeTest : public util::tLoopThread
{
public:

  tPort<int> port;

  rrlib::time::tAtomicDuration max_latency;

  rrlib::time::tAtomicDuration total_latency;

  util::tAtomicInt cycles;

  static rrlib::time::tDuration cINTERVAL;

  tBasicRealtimeTest(const util::tString& name);

  virtual void MainLoopCallback();

  virtual void Run();

  virtual const util::tString ToString() const
  {
    std::ostringstream os;
    os << GetName() << " - Cycles: " << cycles.Get() << "; Max Latency: " << rrlib::time::ToString(max_latency.Load()) << "; Average Latency: " << rrlib::time::ToString(std::chrono::nanoseconds(total_latency.Load()) / std::max(1, cycles.Get()));
    return os.str();
  }

};

} // namespace finroc
} // namespace core

#endif // core__test__tBasicRealtimeTest_h__
