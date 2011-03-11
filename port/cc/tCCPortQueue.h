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

#ifndef core__port__cc__tCCPortQueue_h__
#define core__port__cc__tCCPortQueue_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tThreadLocalCache.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueBounded.h"

namespace finroc
{
namespace core
{
class tCCPortQueueElement;
class tCCPortDataManager;

/*!
 * \author Max Reichardt
 *
 * FIFO Queue that is used in ports.
 *
 * Thread-safe, non-blocking and very efficient for writing.
 * Anything can be enqueued - typically PortData.
 *
 * Use concurrentDequeue, with threads reading from this queue concurrently.
 */
class tCCPortQueue : public util::tWonderQueueBounded<tCCPortDataManager, tCCPortQueueElement>
{
  inline tCCPortQueueElement* GetEmptyContainer2()
  {
    return tThreadLocalCache::GetFast()->GetUnusedCCPortQueueFragment();
  }

protected:

  virtual tCCPortQueueElement* GetEmptyContainer()
  {
    return GetEmptyContainer2();
  }

public:

  tCCPortQueue(int max_length) :
      util::tWonderQueueBounded<tCCPortDataManager, tCCPortQueueElement>(max_length)
  {
  }

  virtual ~tCCPortQueue()
  {
    ::finroc::util::tWonderQueueBounded<tCCPortDataManager, tCCPortQueueElement>::Clear(true);
    ;
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortQueue_h__
