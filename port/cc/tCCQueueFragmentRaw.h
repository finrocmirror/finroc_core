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

#ifndef core__port__cc__tCCQueueFragmentRaw_h__
#define core__port__cc__tCCQueueFragmentRaw_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/finroc_core_utils/container/tQueueFragment.h"


namespace finroc
{
namespace core
{
class tCCPortQueueElement;

/*!
 * \author Max Reichardt
 *
 * Fragment for dequeueing bunch of values
 */
class tCCQueueFragmentRaw : public util::tQueueFragment<tCCPortDataManager, tCCPortQueueElement>
{
public:

  tCCQueueFragmentRaw() {}

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline rrlib::rtti::tGenericObject* DequeueAutoLocked()
  {
    tCCPortDataManager* tmp = ::finroc::util::tQueueFragment<tCCPortDataManager, tCCPortQueueElement>::Dequeue();
    if (tmp == NULL)
    {
      return NULL;
    }
    tThreadLocalCache::Get()->AddAutoLock(tmp);
    return tmp->GetObject();
  }

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline tCCPortDataManager* DequeueUnsafe()
  {
    return ::finroc::util::tQueueFragment<tCCPortDataManager, tCCPortQueueElement>::Dequeue();
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCQueueFragmentRaw_h__
