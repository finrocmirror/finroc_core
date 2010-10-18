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

#ifndef CORE__PORT__CC__TCCPORT_H
#define CORE__PORT__CC__TCCPORT_H

#include "core/portdatabase/tDataTypeRegister.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/cc/tCCPortListener.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCQueueFragment.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/tFrameworkElement.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
class tCCPortData;

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
class tCCPort : public tCCPortBase
{
public:

  /*!
   * \param pci Construction parameters in Port Creation Info Object
   */
  tCCPort(tPortCreationInfo pci) :
      tCCPortBase(ProcessPci(pci))
  {
  }

  /*!
   * \param listener Listener to add
   */
  inline void AddPortListener(tCCPortListener<T>* listener)
  {
    AddPortListenerRaw(reinterpret_cast<tCCPortListener<>*>(listener));
  }

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  inline void DequeueAll(tCCQueueFragment<T>& fragment)
  {
    ::finroc::core::tCCPortBase::DequeueAllRaw(reinterpret_cast<tCCQueueFragment<tCCPortData>&>(fragment));
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
    return reinterpret_cast<T*>(::finroc::core::tCCPortBase::DequeueSingleAutoLockedRaw());
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
  inline tCCInterThreadContainer<T>* DequeueSingleUnsafe()
  {
    return reinterpret_cast<tCCInterThreadContainer<T>*>(::finroc::core::tCCPortBase::DequeueSingleUnsafeRaw());
  }

  /*!
   * \return current auto-locked Port data (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const T* GetAutoLocked()
  {
    return reinterpret_cast<const T*>(GetAutoLockedRaw());
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline T* GetDefaultBuffer()
  {
    assert(((!IsReady())) && "please set default value _before_ initializing port");

    return reinterpret_cast<T*>(this->default_value->GetData());
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

  inline tCCPortDataContainer<T>* GetUnusedBuffer()
  {
    return reinterpret_cast<tCCPortDataContainer<T>*>(tThreadLocalCache::Get()->GetUnusedBuffer(this->data_type));
  }

  inline static tPortCreationInfo& ProcessPci(tPortCreationInfo& pci)
  {
    pci.data_type = tDataTypeRegister::GetInstance()->GetDataType<T>();
    return pci;
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListener(tCCPortListener<T>* listener)
  {
    RemovePortListenerRaw(reinterpret_cast<tCCPortListener<>*>(listener));
  }

  /*!
   * Set default value
   * This must be done before the port is used/initialized.
   *
   * \param t new default
   */
  inline void SetDefault(const T& t)
  {
    this->default_value->Assign(&(t));
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCPORT_H
