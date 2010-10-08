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

#ifndef CORE__PORT__STD__TPORT_H
#define CORE__PORT__STD__TPORT_H

#include "core/portdatabase/tDataTypeRegister.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPortListener.h"
#include "core/port/std/tPortQueueFragment.h"
#include "core/port/std/tPortBase.h"

namespace finroc
{
namespace core
{
class tPortData;

/*!
 * \author Max Reichardt
 *
 * This Port class is used in applications.
 * It kind of provides the API for PortBase
 * and is a generic wrapper for the type-less PortBase.
 *
 * In C++ code for correct casting is generated.
 */
template<typename T>
class tPort : public tPortBase
{
public:

  /*!
   * \param pci Construction parameters in Port Creation Info Object
   */
  tPort(tPortCreationInfo pci) :
      tPortBase(ProcessPci(pci))
  {
  }

  /*!
   * \param listener Listener to add
   */
  inline void AddPortListener(tPortListener<T>* listener)
  {
    ::finroc::core::tPortBase::AddPortListenerRaw(reinterpret_cast<tPortListener<>*>(listener));
  }

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  inline void DequeueAll(tPortQueueFragment<T>& fragment)
  {
    ::finroc::core::tPortBase::DequeueAllRaw(reinterpret_cast<tPortQueueFragment<tPortData>&>(fragment));
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * Container is autoLocked and is recycled with next ThreadLocalCache.get().releaseAllLocks()
   * (Use only with ports that have a input queue)
   *
   * \return Dequeued first/oldest element in queue
   */
  inline T* DequeueSingleAutoLocked()
  {
    return static_cast<T*>(::finroc::core::tPortBase::DequeueSingleAutoLockedRaw());
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * Container needs to be recycled manually by caller!
   * (Use only with ports that have a input queue)
   *
   * \return Dequeued first/oldest element in queue
   */
  inline T* DequeueSingleUnsafe()
  {
    return static_cast<T*>(::finroc::core::tPortBase::DequeueSingleUnsafeRaw());
  }

  /*!
   * \return current auto-locked Port data (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const T* GetAutoLocked()
  {
    return static_cast<const T*>(GetAutoLockedRaw());
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline T* GetDefaultBuffer()
  {
    return static_cast<T*>(::finroc::core::tPortBase::GetDefaultBufferRaw());
  }

  /*!
   * (careful: no auto-release of lock)
   *
   * \return current locked port data
   */
  inline const T* GetLockedUnsafe()
  {
    return static_cast<const T*>(::finroc::core::tPortBase::GetLockedUnsafeRaw());
  }

  /*!
   * Pulls port data (regardless of strategy)
   * (careful: no auto-release of lock)
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  virtual const T* GetPullLockedUnsafe(bool intermediate_assign)
  {
    return static_cast<const T*>(PullValueRaw(intermediate_assign));
  }

  //
  //
  //  ::std::tr1::shared_ptr<T> getValue() {
  //        return shared_ptr<T>(((PortDataContainer<T>)lockContainer())->data), **valueUnlocker**);
  //  }
  //
  //  ::std::tr1::<PortDataContainer<T>> getValueContainer() {
  //        return shared_ptr<T>((PortDataContainer<T>)lockContainer(), **valueUnlocker**);
  //  }
  //

  inline T* GetUnusedBuffer()
  {
    return static_cast<T*>(::finroc::core::tPortBase::GetUnusedBufferRaw());
  }

  inline static tPortCreationInfo& ProcessPci(tPortCreationInfo& pci)
  {
    pci.data_type = pci.data_type != NULL ? pci.data_type : tDataTypeRegister::GetInstance()->GetDataType<T>();
    return pci;
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListener(tPortListener<T>* listener)
  {
    ::finroc::core::tPortBase::RemovePortListenerRaw(reinterpret_cast<tPortListener<>*>(listener));
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORT_H
