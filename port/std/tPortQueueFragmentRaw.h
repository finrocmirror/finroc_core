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

#ifndef core__port__std__tPortQueueFragmentRaw_h__
#define core__port__std__tPortQueueFragmentRaw_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/std/tPortDataReference.h"
#include "rrlib/finroc_core_utils/container/tQueueFragment.h"


namespace finroc
{
namespace core
{
class tPortQueueElement;

/*!
 * \author Max Reichardt
 *
 * Fragment for dequeueing bunch of values
 */
class tPortQueueFragmentRaw : public util::tQueueFragment<tPortDataReference, tPortQueueElement>
{
public:

  tPortQueueFragmentRaw() {}

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline rrlib::rtti::tGenericObject* DequeueAutoLocked()
  {
    tPortDataManager* tmp = DequeueUnsafe();
    tThreadLocalCache::Get()->AddAutoLock(tmp);
    return tmp->GetObject();
  }

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline tPortDataManager* DequeueUnsafe()
  {
    return Dequeue()->GetManager();
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__std__tPortQueueFragmentRaw_h__
