/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2011 Max Reichardt,
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

#ifndef core__port__tPort_h__
#define core__port__tPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortWrapperBase.h"
#include "core/port/tPortCreationInfo.h"
#include "core/datatype/tBounds.h"
#include "core/port/tPortListener.h"

#include "core/port/tPortTypeMap.h"
#include "core/port/tPortUtil.h"
#include "core/port/tPortQueueFragment.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * This Port class is used in applications.
 * It kind of provides the API for PortBase backend, which it wraps.
 * and is a generic wrapper for the type-less PortBase.
 *
 * In C++ code for correct casting is generated.
 */
template<typename T>
class tPort : public tPortParent<T>
{
protected:

  /*!
   * (Constructor for derived classes)
   * (wrapped must be set in constructor!)
   */
  tPort()  {}

  /*!
   * (in C++: puts data type T in port creation info)
   *
   * \param pci Input PortCreationInfo
   * \return Modified PortCreationInfo
   */
  inline static tPortCreationInfo ProcessPci(tPortCreationInfo pci)
  {
    pci.data_type = rrlib::serialization::tDataType<typename tPortTypeMap<T>::tPortDataType>();
    return pci;
  }

public:

  // typedefs
  typedef tPortDataPtr<T> tDataPtr;
  typedef typename tPortTypeMap<T>::tPortBaseType tPortBaseType;
  using tPortWrapperBase<tPortBaseType>::wrapped;

  /*!
   * \param pci Construction parameters in Port Creation Info Object
   */
  tPort(tPortCreationInfo pci)
  {
    wrapped = new tPortBaseType(ProcessPci(pci));
  }

  /*!
   * \param pci Construction parameters in Port Creation Info Object
   */
  tPort(tPortCreationInfo pci, const tBounds<T>& bounds)
  {
    wrapped = new typename tPortTypeMap<T>::tBoundedPortBaseType(ProcessPci(pci), bounds);
  }

  /*!
   * \param description Port description
   * \param parent Parent
   * \param output_port Output port? (or rather input port)
   */
  tPort(const util::tString& description, tFrameworkElement* parent, bool output_port)
  {
    wrapped = new tPortBaseType(ProcessPci(tPortCreationInfo(description, parent, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)));
  }

  void AddPortListener(tPortListener<tPortDataPtr<const T> >* listener)
  {
    wrapped->AddPortListenerRaw(listener);
  }

  void AddPortListener(tPortListener<void*>* listener)
  {
    wrapped->AddPortListenerRaw(listener);
  }

  /*!
   * \param listener Listener to add
   */
  inline void AddPortListener(tPortListener<T>* listener)
  {
    wrapped->AddPortListenerRaw(listener);
  }

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  inline void DequeueAll(tPortQueueFragment<T>& fragment)
  {
    fragment.DequeueFromPort(wrapped);
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * (Use only with ports that have a input queue)
   * (in Java lock will need to be released manually, in C++ tPortDataPtr takes care of this)
   *
   * \return Dequeued first/oldest element in queue
   */
  inline tPortDataPtr<const T> DequeueSingle()
  {
    return tPortUtil<T>::DequeueSingle(wrapped);
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * (Use only with ports that have a input queue)
   *
   * \param result Buffer to (deep) copy dequeued value to
   * (Using this dequeueSingle()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   * \return true if element was dequeued - false if queue was empty
   */
  inline bool DequeueSingle(T& result)
  {
    return tPortUtil<T>::DequeueSingle(wrapped, result);
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
  inline const T* DequeueSingleAutoLocked()
  {
    return tPortUtil<T>::DequeueSingleAutoLocked(wrapped);
  }

  /*!
   * Gets Port's current value
   *
   * \return Port's current value with read lock.
   * (in Java lock will need to be released manually, in C++ tPortDataPtr takes care of this)
   * (Using get with parameter T& is more efficient when using CC types - shouldn't matter usually)
   */
  inline tPortDataPtr<const T> Get()
  {
    return tPortUtil<T>::GetValueWithLock(wrapped);
  }

  /*!
   * Gets Port's current value
   *
   * (Note that numbers and "cheap copy" types also have a method: T GetValue();  (defined in tPortParent<T>))
   *
   * \param result Buffer to (deep) copy port's current value to
   * (Using this get()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   */
  inline const void Get(T& result)
  {
    return tPortUtil<T>::GetValue(wrapped, result);
  }

  /*!
   * Gets Port's current value
   *
   * \return current auto-locked Port data (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const T* GetAutoLocked()
  {
    return tPortUtil<T>::GetAutoLocked(wrapped);
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline T* GetDefaultBuffer()
  {
    rrlib::serialization::tGenericObject* go = wrapped->GetDefaultBufferRaw();
    return go->GetData<T>();
  }

  /*!
   * Pulls port data (regardless of strategy)
   * (careful: no auto-release of lock in Java)
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  inline const T GetPull(bool intermediate_assign)
  {
    return tPortUtil<T>::GetPull(wrapped, intermediate_assign);
  }

  /*!
   * \return Unused buffer of type T.
   * Buffers to be published using this port (non-CC-types),
   * should be acquired using this function. The buffer might contain old data, so it should
   * be cleared prior to using. Using this method with CC-types is not required and less
   * efficient than publishing values directly (factor 2, shouldn't matter usually).
   */
  inline tPortDataPtr<T> GetUnusedBuffer()
  {
    return tPortUtil<T>::GetUnusedBuffer(wrapped);
  }

  /*!
   * \return Does port have "cheap copy" (CC) type?
   */
  inline bool HasCCType()
  {
    return typeutil::tIsCCType<T>::value; // compile-time constant
  }

  // Publish Data Buffer. This data will be forwarded to any connected ports.
  // Should only be called on output ports.
  //
  // \param data Data to publish. It will be deep-copied.
  // This publish()-variant is efficient when using CC types, but can be extremely costly with large data types)
  void Publish(const T& data)
  {
    tPortUtil<T>::CopyAndPublish(wrapped, data);
  }

  inline void Publish(tPortDataPtr<T>& data)
  {
    tPortUtil<T>::Publish(wrapped, data);
  }

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * It should not be modified thereafter.
   * Should only be called on output ports.
   *
   * \param data Data buffer acquired from a port using getUnusedBuffer (or locked data received from another port)
   */
  inline void Publish(tPortDataPtr<const T>& data)
  {
    tPortUtil<T>::Publish(wrapped, data);
  }

  void RemovePortListener(tPortListener<tPortDataPtr<const T> >* listener)
  {
    wrapped->RemovePortListenerRaw(listener);
  }

  /*!
   * \param listener Listener to add
   */
  inline void RemovePortListener(tPortListener<T>* listener)
  {
    wrapped->RemovePortListenerRaw(listener);
  }

  //    /**
  //     * \param pullRequestHandler Object that handles pull requests - null if there is none (typical case)
  //     */
  //    public void setPullRequestHandler(PullRequestHandler pullRequestHandler) {
  //        wrapped.setPullRequestHandler(pullRequestHandler);
  //    }

  //    /**
  //     * Does port (still) have this value?
  //     * (calling this is only safe, when pd is locked)
  //     *
  //     * \param pd Port value
  //     * \return Answer
  //     */
  //    @InCpp("return getHelper(PortUtil<T>::publish(wrapped));")
  //    @ConstMethod public boolean valueIs(@Const @Ptr T pd) {
  //        if (hasCCType()) {
  //            return ((CCPortBase)wrapped).valueIs(pd);
  //        } else {
  //            return ((PortBase)wrapped).valueIs(pd);
  //        }
  //    }
  //
  //
  //    boolean valueIs(std::shared_ptr<const T>& pd) const {
  //        return valueIs(pd._get());
  //    }
  //
  //    boolean valueIs(const T& pd) const {
  //        return value
  //    }
  //

  /*!
   * Set new bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param b New Bounds
   */
  inline void SetBounds(tBounds<T> b)
  {
    tPortUtil<T>::SetBounds(wrapped, b);
  }

  /*!
   * Set default value
   * This must be done before the port is used/initialized.
   *
   * \param t new default
   */
  virtual void SetDefault(const T& t)
  {
    assert(!this->IsReady() && "please set default value _before_ initializing port");
    tPortUtil<T>::SetDefault(wrapped, t);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tPort_h__
