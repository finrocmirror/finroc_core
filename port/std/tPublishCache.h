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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STD__TPUBLISHCACHE_H
#define CORE__PORT__STD__TPUBLISHCACHE_H

#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
class tPortDataReference;

/*!
 * \author Max Reichardt
 *
 * Thread local cache for publishing operations in "ordinary" ports
 */
struct tPublishCache
{
public:

  /*! estimated number of locks for current publishing operation */
  int lock_estimate;

  /*! actual number of already set locks in current publishing operation */
  int set_locks;

  /*! Reference to port data used in current publishing operation */
  tPortDataReference* cur_ref;

  /*! Reference to port data's reference counter in current publishing operation */
  tPortDataManager::tRefCounter* cur_ref_counter;

  inline void ReleaseObsoleteLocks()
  {
    assert(((set_locks <= lock_estimate)) && "More locks set than estimated and set (=> not safe... please increase lockEstimate)");
    if (set_locks < lock_estimate)
    {
      cur_ref_counter->ReleaseLocks(static_cast<int8>((lock_estimate - set_locks)));
    }
  }

  tPublishCache() {}  // if possible, no initialization

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPUBLISHCACHE_H
