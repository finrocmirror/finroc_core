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

#ifndef CORE__PORT__STD__TPORTQUEUE_H
#define CORE__PORT__STD__TPORTQUEUE_H

#include "core/port/std/tPortQueueElement.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueBounded.h"

namespace finroc
{
namespace core
{
class tPortDataReference;

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
template < typename T = tPortData >
class tPortQueue : public util::tWonderQueueBounded<tPortDataReference, tPortQueueElement>
{
  inline tPortQueueElement* GetEmptyContainer2()
  {
    return tThreadLocalCache::GetFast()->GetUnusedPortQueueFragment();
  }

protected:

  virtual tPortQueueElement* GetEmptyContainer()
  {
    return GetEmptyContainer2();
  }

public:

  tPortQueue(int max_length) :
      util::tWonderQueueBounded<tPortDataReference, tPortQueueElement>(max_length)
  {
  }

  virtual ~tPortQueue()
  {
    ::finroc::util::tWonderQueueBounded<tPortDataReference, tPortQueueElement>::Clear(true);
    ;
  }

  inline void Enqueue(tPortDataReference* pdr)
  {
    tPortQueueElement* pqe = GetEmptyContainer2();
    assert((pqe->GetElement() == NULL));
    assert((!pqe->IsDummy()));
    pqe->SetElement(pdr);
    ::finroc::util::tWonderQueueBounded<tPortDataReference, tPortQueueElement>::EnqueueDirect(pqe);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTQUEUE_H
