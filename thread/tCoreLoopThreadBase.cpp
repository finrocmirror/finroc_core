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
#include "core/thread/tCoreLoopThreadBase.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
tCoreLoopThreadBase::tCoreLoopThreadBase(int64 default_cycle_time, bool warn_on_cycle_time_exceed, bool pause_on_startup) :
  util::tLoopThread(default_cycle_time, warn_on_cycle_time_exceed, pause_on_startup),
  tc(NULL)
{
}

tCoreLoopThreadBase::tCoreLoopThreadBase(int64 default_cycle_time, bool warn_on_cycle_time_exceed) :
  util::tLoopThread(default_cycle_time, warn_on_cycle_time_exceed),
  tc(NULL)
{
}

tCoreLoopThreadBase::tCoreLoopThreadBase(int64 default_cycle_time) :
  util::tLoopThread(default_cycle_time),
  tc(NULL)
{
}

void tCoreLoopThreadBase::InitThreadLocalCache()
{
  tc = tThreadLocalCache::Get();
}

} // namespace finroc
} // namespace core

