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
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortDataBufferPool.h"

namespace finroc
{
namespace core
{
const size_t tCCPortDataManagerTL::cREF_INDEX_MASK;

tCCPortDataManagerTL::tCCPortDataManagerTL() :
    reuse_counter(0),
    owner_thread(util::sThreadUtil::GetCurrentThreadId()),
    ref_counter(0)
{
}

void tCCPortDataManagerTL::NonOwnerLockRelease(tCCPortDataBufferPool* owner)
{
  ::finroc::util::tObject* owner2 = this->owner;
  if (owner_thread == util::sThreadUtil::GetCurrentThreadId())
  {
    ReleaseLock();
  }
  else if (owner2 != NULL)
  {
    owner->ReleaseLock(this);
  }
  else
  {
    PostThreadReleaseLock();
  }
}

void tCCPortDataManagerTL::PostThreadReleaseLock()
{
  ref_counter--;
  assert((ref_counter >= 0));
  if (ref_counter == 0)
  {
    delete this;  // my favourite statement :-)
  }
}

} // namespace finroc
} // namespace core

