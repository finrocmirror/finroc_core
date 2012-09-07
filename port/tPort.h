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
#include "core/port/tPortTypeMap.h"
#include "core/port/tPortUtil.h"
#include "core/port/tPortListener.h"
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
 * It is a generic wrapper for the type-less PortBase.
 */
template<typename T>
class tPort : public tPortWrapperBase
{
  static_assert(rrlib::serialization::tIsBinarySerializable<T>::value, "Type T needs to be binary serializable for use in ports.");

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
  inline static tPortCreationInfo<T> ProcessPci(tPortCreationInfo<T> pci)
  {
    pci.data_type = rrlib::rtti::tDataType<typename tPortTypeMap<T>::tPortDataType>();
    return pci;
  }

  template < bool BOUNDABLE = tPortTypeMap<T>::boundable >
  typename tPortTypeMap<T>::tPortBaseType* CreateBoundedPort(const typename std::enable_if<BOUNDABLE, tPortCreationInfo<T>>::type& pci)
  {
    return new typename tPortTypeMap<T>::tBoundedPortBaseType(ProcessPci(pci));
  }

  template < bool BOUNDABLE = tPortTypeMap<T>::boundable >
  typename tPortTypeMap<T>::tPortBaseType* CreateBoundedPort(const typename std::enable_if < !BOUNDABLE, tPortCreationInfo<T >>::type& pci)
  {
    assert(false && "This should not be called. Bounds are not supported with non-cc types.");
    return NULL;
  }

public:

  // typedefs
  typedef tPortDataPtr<T> tDataPtr;
  typedef typename tPortTypeMap<T>::tPortBaseType tPortBaseType;

  /*!
   * Constructor takes variadic argument list... just any properties you want to assign to port.
   *
   * The first string is interpreted as port name, the second possibly as config entry (relevant for parameters only).
   * A framework element pointer is interpreted as parent.
   * unsigned int arguments are interpreted as flags.
   * int argument is interpreted as queue length.
   * tBounds<T> are port's bounds.
   * tUnit argument is port's unit.
   * int16/short argument is interpreted as minimum network update interval.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when port has numeric or string type.
   * There we have these rules:
   *
   * string type: The second string argument is interpreted as default_value. The third as config entry.
   * numeric type: The first numeric argument is interpreted as default_value. However, if it is the only unsigned int
   *               argument, it is interpreted as flags (because every port needs flags).
   */
  template <typename ... ARGS>
  tPort(const ARGS&... args)
  {
    tPortCreationInfo<T> pci(args...);
    if (pci.BoundsSet())
    {
      wrapped = CreateBoundedPort(pci);
    }
    else
    {
      wrapped = new tPortBaseType(ProcessPci(pci));
    }
    wrapped->SetWrapperDataType(rrlib::rtti::tDataType<T>());
    if (pci.DefaultValueSet())
    {
      T t = rrlib::rtti::sStaticTypeInfo<T>::CreateByValue();
      pci.GetDefault(t);
      SetDefault(t);
    }
  }

  /*!
   * Wraps raw port
   */
  tPort(tPortBaseType& wrap)
  {
    assert(wrap.GetDataType().GetRttiName() == typeid(typename tPortTypeMap<T>::tPortDataType).name());
    wrapped = &wrap;
  }

  /*!
   * \param listener Listener to add
   */
  void AddPortListener(tPortListener<tPortDataPtr<const T>>& listener)
  {
    static_cast<tPortBaseType*>(wrapped)->AddPortListenerRaw(listener);
  }

  void AddPortListener(tPortListener<>& listener)
  {
    static_cast<tPortBaseType*>(wrapped)->AddPortListenerRaw(listener);
  }
  inline void AddPortListener(tPortListener<T>& listener)
  {
    static_cast<tPortBaseType*>(wrapped)->AddPortListenerRaw(listener);
  }

  /*!
   * Dequeue all elements currently in queue
   *
   * \param fragment Fragment to store all dequeued values in
   */
  inline void DequeueAll(tPortQueueFragment<T>& fragment)
  {
    fragment.DequeueFromPort(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * (Use only with ports that have a input queue)
   *
   * \return Dequeued first/oldest element in queue (NULL if no element is left in queue)
   */
  inline tPortDataPtr<const T> Dequeue()
  {
    return tPortUtil<T>::DequeueSingle(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * Dequeue first/oldest element in queue.
   * Because queue is bounded, continuous dequeueing may skip some values.
   * Use dequeueAll if a continuous set of values is required.
   *
   * (Use only with ports that have a input queue)
   *
   * \param result Buffer to (deep) copy dequeued value to
   * \param timestamp Buffer to store time stamp of data in (optional)
   * (Using this dequeueSingle()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   * \return true if element was dequeued - false if queue was empty
   */
  inline bool Dequeue(T& result)
  {
    rrlib::time::tTimestamp unused;
    return Dequeue(result, unused);
  }
  inline bool Dequeue(T& result, rrlib::time::tTimestamp& timestamp)
  {
    return tPortUtil<T>::DequeueSingle(static_cast<tPortBaseType*>(wrapped), result, timestamp);
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
  inline const T* DequeueAutoLocked()
  {
    return tPortUtil<T>::DequeueSingleAutoLocked(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * Gets Port's current value.
   * (Only available for CC types)
   *
   * \param v unused dummy parameter for std::enable_if technique
   * \param timestamp Buffer to store time stamp of data in (optional)
   * \return Port's current value by value.
   */
  template < bool CC = typeutil::tIsCCType<T>::value >
  inline T Get(typename std::enable_if<CC, void>::type* v = NULL) const
  {
    T t;
    tPortUtil<T>::GetValue(static_cast<tPortBaseType*>(wrapped), t);
    return t;
  }
  template < bool CC = typeutil::tIsCCType<T>::value >
  inline T Get(typename std::enable_if<CC, rrlib::time::tTimestamp&>::type& timestamp) const
  {
    T t;
    Get(t, timestamp);
    return t;
  }

  /*!
   * Gets Port's current value
   *
   * (Note that numbers and "cheap copy" types also have a method: T GetValue();  (defined in tPortParent<T>))
   *
   * \param result Buffer to (deep) copy port's current value to
   * \param timestamp Buffer to store time stamp of data in (optional)
   * (Using this get()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   */
  inline void Get(T& result) const
  {
    tPortUtil<T>::GetValue(static_cast<tPortBaseType*>(wrapped), result);
  }
  inline void Get(T& result, rrlib::time::tTimestamp& timestamp) const
  {
    tPortUtil<T>::GetValue(static_cast<tPortBaseType*>(wrapped), result, timestamp);
  }

  /*!
   * Gets Port's current value
   *
   * \return current auto-locked Port data (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const T* GetAutoLocked() const
  {
    return tPortUtil<T>::GetAutoLocked(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * \return Bounds as they are currently set
   */
  template < bool BOUNDABLE = tPortTypeMap<T>::boundable >
  inline const typename std::enable_if<BOUNDABLE, tBounds<T>>::type GetBounds() const
  {
    return tPortUtil<T>::GetBounds(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * \return Buffer with default value. Can be used to change default value
   * for port. However, this should be done before the port is used.
   */
  inline T* GetDefaultBuffer()
  {
    rrlib::rtti::tGenericObject* go = static_cast<tPortBaseType*>(wrapped)->GetDefaultBufferRaw();
    return go->GetData<T>();
  }

  /*!
   * Gets Port's current value
   *
   * \return Port's current value with read lock.
   * (in Java lock will need to be released manually, in C++ tPortDataPtr takes care of this)
   * (Using get with parameter T& is more efficient when using CC types - shouldn't matter usually)
   */
  inline tPortDataPtr<const T> GetPointer() const
  {
    return tPortUtil<T>::GetValueWithLock(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * Pulls port data (regardless of strategy)
   * (careful: no auto-release of lock in Java)
   *
   * \param intermediate_assign Assign pulled value to ports in between?
   *
   * \return Pulled locked data
   */
  inline tPortDataPtr<const T> GetPull(bool intermediate_assign)
  {
    return tPortUtil<T>::GetPull(static_cast<tPortBaseType*>(wrapped), intermediate_assign);
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
    return tPortUtil<T>::GetUnusedBuffer(static_cast<tPortBaseType*>(wrapped));
  }

  /*!
   * \return Wrapped port. For rare case that someone really needs to access ports.
   */
  inline tPortBaseType* GetWrapped()
  {
    return static_cast<tPortBaseType*>(wrapped);
  }

  /*!
   * \return Does port have "cheap copy" (CC) type?
   */
  inline bool HasCCType() const
  {
    return typeutil::tIsCCType<T>::value; // compile-time constant
  }

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * It should not be modified thereafter.
   * Should only be called on output ports.
   *
   * \param data Data buffer acquired from a port using getUnusedBuffer (or locked data received from another port)
   */
  inline void Publish(tPortDataPtr<const T> && data)
  {
    tPortUtil<T>::Publish(static_cast<tPortBaseType*>(wrapped), data);
  }

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * Should only be called on output ports.
   *
   * (This pass-by-value Publish()-variant is efficient when using CC types, but can be extremely costly with large data types)
   *
   * \param data Data to publish. It will be deep-copied.
   * \param teimstamp Timestamp for attached data (optional)
   */
  inline void Publish(const T& data, const rrlib::time::tTimestamp& timestamp = rrlib::time::cNO_TIME)
  {
    tPortUtil<T>::CopyAndPublish(static_cast<tPortBaseType*>(wrapped), data, timestamp);
  }

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * Should only be called on output ports.
   *
   * \param data Data to publish. It will be deep-copied.
   */
  inline void Publish(tPortDataPtr<T> && data)
  {
    tPortUtil<T>::Publish(static_cast<tPortBaseType*>(wrapped), data);
  }
  inline void Publish(tPortDataPtr<T>& data)
  {
    tPortUtil<T>::Publish(static_cast<tPortBaseType*>(wrapped), data);
  }
  inline void Publish(tPortDataPtr<const T>& data)
  {
    tPortUtil<T>::Publish(static_cast<tPortBaseType*>(wrapped), data);
  }

  /*!
   * \param listener Listener to remove
   */
  void RemovePortListener(tPortListener<tPortDataPtr<const T>>& listener)
  {
    static_cast<tPortBaseType*>(wrapped)->RemovePortListenerRaw(listener);
  }
  inline void RemovePortListener(tPortListener<T>& listener)
  {
    static_cast<tPortBaseType*>(wrapped)->RemovePortListenerRaw(listener);
  }

  /*!
   * Set new bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param b New Bounds
   */
  template < bool BOUNDABLE = tPortTypeMap<T>::boundable >
  inline void SetBounds(const typename std::enable_if<BOUNDABLE, tBounds<T>>::type& b)
  {
    tPortUtil<T>::SetBounds(static_cast<tPortBaseType*>(wrapped), b);
  }

  /*!
   * Set default value
   * This must be done before the port is used/initialized.
   *
   * \param t new default
   */
  void SetDefault(const T& t)
  {
    rrlib::serialization::tStackMemoryBuffer<2048> buf;
    rrlib::serialization::tOutputStream os(&buf);
    os << t;
    os.Close();
    SetDefault(buf);
  }

  /*!
   * Set default value
   * This must be done before the port is used/initialized.
   *
   * \param source Source from which default value is deserialized
   */
  void SetDefault(const rrlib::serialization::tConstSource& source)
  {
    assert(!this->IsReady() && "please set default value _before_ initializing port");
    rrlib::serialization::tInputStream is(&source);
    tPortUtil<T>::SetDefault(static_cast<tPortBaseType*>(wrapped), is);
  }
};

extern template class tPort<int>;
extern template class tPort<long long int>;
extern template class tPort<float>;
extern template class tPort<double>;
extern template class tPort<tNumber>;
extern template class tPort<std::string>;
extern template class tPort<bool>;
extern template class tPort<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

#endif // core__port__tPort_h__
