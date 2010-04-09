/**
J * You received this file as part of an advanced experimental
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
#include "core/port/cc/tCCPortDataRef.h"
#include "core/port/cc/tCCPortDataBufferPool.h"

namespace finroc
{
namespace core
{
template<typename T>
const size_t tCCPortDataContainer<T>::cREF_INDEX_MASK;

template<typename T>
tCCPortDataContainer<T>::tCCPortDataContainer(tDataType* type, util::tObject* object) :
    reuse_counter(0u),
    owner_thread(util::sThreadUtil::GetCurrentThreadId()),
    ref_counter(0),
    obj_synch(),
    port_data(type, object)
{
  //portData.setContainer(this);

  this->type = type;
  assert((GetDataPtr() == (reinterpret_cast<tCCPortDataContainer<>*>(this))->GetDataPtr()));  // for C++ class layout safety
}

template<typename T>
void tCCPortDataContainer<T>::NonOwnerLockRelease(tCCPortDataBufferPool* owner)
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

template<typename T>
void tCCPortDataContainer<T>::PostThreadReleaseLock()
{
  util::tLock lock1(obj_synch);
  ref_counter--;
  assert((ref_counter >= 0));
  if (ref_counter == 0)
  {
    lock1.unlock();
    delete this;  // my favourite statement :-)

  }
}

} // namespace finroc
} // namespace core

