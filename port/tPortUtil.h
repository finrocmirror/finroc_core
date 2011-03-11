/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#ifndef core__port__tPortUtil_h__
#define core__port__tPortUtil_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/serialization/tGenericObjectWrapper.h"
#include "core/portdatabase/tSharedPtrDeleteHandler.h"
#include "core/port/tPortTypeMap.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Implementations of various methods in tPort class
 */
template <typename T, bool CC, bool ENUM, bool NUM>
class tPortUtilBase
{

public:
  typedef tPortDataManager tManager;
  typedef tPortDataManager tManagerTL;
  typedef tSharedPtrDeleteHandler<tManagerTL> tDeleteHandlerTL;
  typedef tSharedPtrDeleteHandler<tManager> tDeleteHandler;
  typedef tPortBase tPortType;

  static std::shared_ptr<T> GetUnusedBuffer(tPortType* port)
  {
    tManagerTL* mgr = port->GetUnusedBufferRaw();
    return std::shared_ptr<T>(mgr->GetObject()->GetData<T>(), tDeleteHandlerTL(mgr));
  }

  static std::shared_ptr<const T> GetValueWithLock(tPortType* port)
  {
    tManager* mgr = port->GetLockedUnsafeRaw();
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static std::shared_ptr<const T> DequeueSingle(tPortType* port)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static std::shared_ptr<const T> GetPull(tPortType* port, bool intermediate_assign)
  {
    tManager* mgr = port->GetPullLockedUnsafe(intermediate_assign);
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static void GetValue(tPortType* port, T& result)
  {
    tManager* mgr = port->GetLockedUnsafeRaw();
    rrlib::serialization::sSerialization::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
    mgr->ReleaseLock();
  }

  static void DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    rrlib::serialization::sSerialization::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
    mgr->ReleaseLock();
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    return port->GetAutoLockedRaw()->GetObject()->GetData<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    rrlib::serialization::tGenericObject* go = port->DequeueSingleAutoLockedRaw();
    return go->GetData<T>();
  }

  static void SetDefault(tPortType* port, const T& t)
  {
    rrlib::serialization::tGenericObject* go = port->GetDefaultBufferRaw();
    rrlib::serialization::sSerialization::DeepCopy(t, *(go->GetData<T>()), NULL);
  }

  static void Publish(tPortType* port, std::shared_ptr<const T>& t)
  {
    tManagerTL* mgr = tDeleteHandlerTL::GetManager(t, true);
    assert(mgr != NULL && "You should acquire buffers to publish large data with getUnusedBuffer()");
    port->Publish(mgr);
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    std::shared_ptr<T> buf = GetUnusedBuffer(port);
    rrlib::serialization::sSerialization::DeepCopy(t, *buf);
    Publish(port, buf);
  }

  static void SetBounds(tPortType* port, const tBounds<T>& b)
  {
    assert(false && "Bounds can only be used with CC types");
  }
};

template <typename T>
class tPortUtilBaseNumeric
{

public:
  typedef tCCPortDataManager tManager;
  typedef tCCPortDataManagerTL tManagerTL;
  typedef tSharedPtrDeleteHandler<tManagerTL> tDeleteHandlerTL;
  typedef tSharedPtrDeleteHandler<tManager> tDeleteHandler;
  typedef tCCPortBase tPortType;

  static std::shared_ptr<T> GetUnusedBuffer(tPortType* port)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    return std::shared_ptr<T>(mgr->GetObject()->GetData<tNumber>()->GetValuePtr<T>(), tDeleteHandlerTL(mgr));
  }

  static std::shared_ptr<const T> GetValueWithLock(tPortType* port)
  {

    // copy value and possibly convert number to correct type (a little inefficient, but should be used scarcely anyway)
    T val;
    GetValue(port, val);
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    new_num->SetValue(val, port->GetUnit());
    return std::shared_ptr<const T>(new_num->GetValuePtr<T>(), tDeleteHandler(c));
  }

  static std::shared_ptr<const T> DequeueSingle(tPortType* port)
  {
    T val;
    DequeueSingle(port, val);
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    new_num->SetValue(val, port->GetUnit());
    return std::shared_ptr<const T>(new_num->GetValuePtr<T>(), tDeleteHandler(c));
  }

  static std::shared_ptr<const T> GetPull(tPortType* port, bool intermediate_assign)
  {
    tManager* mgr = port->GetPullInInterthreadContainerRaw(intermediate_assign);
    tNumber* num = mgr->GetObject()->GetData<tNumber>();
    if (port->GetUnit() != num->GetUnit() && port->GetUnit() != &tUnit::cNO_UNIT && num->GetUnit() != &tUnit::cNO_UNIT)
    {
      num->SetValue(static_cast<T>(num->GetUnit()->ConvertTo(num->Value<double>(), port->GetUnit())));
    }
    return std::shared_ptr<const T>(num->GetValuePtr<T>(), tDeleteHandler(mgr));
  }

  static void GetValue(tPortType* port, T& result)
  {
    tNumber num;
    rrlib::serialization::tGenericObjectWrapper<tNumber> tmp(&num);
    port->GetRaw(&tmp);
    if (port->GetUnit() != num.GetUnit() && port->GetUnit() != &tUnit::cNO_UNIT && num.GetUnit() != &tUnit::cNO_UNIT)
    {
      result = static_cast<T>(num.GetUnit()->ConvertTo(num.Value<double>(), port->GetUnit()));
    }
    else
    {
      result = num.Value<T>();
    }
  }

  static void DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    tNumber* num = mgr->GetObject()->GetData<tNumber>();
    if (port->GetUnit() != num->GetUnit() && port->GetUnit() != &tUnit::cNO_UNIT && num->GetUnit() != &tUnit::cNO_UNIT)
    {
      result = static_cast<T>(num->GetUnit()->ConvertTo(num->Value<double>(), port->GetUnit()));
    }
    else
    {
      result = num->Value<T>();
    }
    mgr->Recycle2();
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    port->GetAutoLockedRaw()->GetData<tNumber>()->GetValuePtr<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    T val;
    DequeueSingle(port, val);
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    new_num->SetValue(val, port->GetUnit());
    tThreadLocalCache::GetFast()->AddAutoLock(c);
    return new_num;
  }

  static void SetDefault(tPortType* port, const T& t)
  {
    rrlib::serialization::tGenericObject* go = port->GetDefaultBufferRaw();
    go->GetData<tNumber>()->SetValue(t, port->GetUnit());

    // publish for value caching in Parameter classes
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(tNumber::cTYPE);
    mgr->GetObject()->GetData<tNumber>()->SetValue(t, port->GetUnit());
    port->BrowserPublishRaw(mgr);
  }

  static void Publish(tPortType* port, std::shared_ptr<const T>& t)
  {
    tManagerTL* mgr = tDeleteHandlerTL::GetManager(t, true);
    assert(mgr != NULL && "You should acquire buffers to publish large data with getUnusedBuffer()");
    port->Publish(mgr);
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    mgr->GetObject()->GetData<tNumber>()->SetValue(t, port->GetUnit());
    port->Publish(mgr);
  }

  static void SetBounds(tPortType* port, const tBounds<T>& b)
  {
    static_cast<tCCPortBoundedNumeric<T>*>(port)->SetBounds(b);
  }

};

template <typename T, bool CC, bool ENUM>
class tPortUtilBase<T, CC, ENUM, true> : public tPortUtilBaseNumeric<T>
{
};

template <typename T, bool CC, bool NUM>
class tPortUtilBase<T, CC, true, NUM> : public tPortUtilBaseNumeric<T>
{
};

template <typename T, bool ENUM, bool NUM>
class tPortUtilBase<T, true, ENUM, NUM>
{

public:
  typedef tCCPortDataManager tManager;
  typedef tCCPortDataManagerTL tManagerTL;
  typedef tSharedPtrDeleteHandler<tManagerTL> tDeleteHandlerTL;
  typedef tSharedPtrDeleteHandler<tManager> tDeleteHandler;
  typedef tCCPortBase tPortType;

  static std::shared_ptr<T> GetUnusedBuffer(tPortType* port)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    return std::shared_ptr<T>(mgr->GetObject()->GetData<T>(), tDeleteHandlerTL(mgr));
  }

  static std::shared_ptr<const T> GetValueWithLock(tPortType* port)
  {
    tManager* mgr = port->GetInInterThreadContainer();
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static std::shared_ptr<const T> DequeueSingle(tPortType* port)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static std::shared_ptr<const T> GetPull(tPortType* port, bool intermediate_assign)
  {
    tManager* mgr = port->GetPullInInterthreadContainerRaw(intermediate_assign);
    return std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tDeleteHandler(mgr));
  }

  static void GetValue(tPortType* port, T& result)
  {
    rrlib::serialization::tGenericObjectWrapper<T> tmp(&result);
    port->GetRaw(&tmp);
  }

  static void DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    rrlib::serialization::sSerialization::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
    mgr->Recycle2();
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    return port->GetAutoLockedRaw()->GetData<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    return port->DequeueSingleAutoLockedRaw()->GetData<T>();
  }

  static void SetDefault(tPortType* port, const T& t)
  {
    rrlib::serialization::tGenericObject* go = port->GetDefaultBufferRaw();
    rrlib::serialization::sSerialization::DeepCopy(t, *(go->GetData<T>()), NULL);

    // publish for value caching in Parameter classes
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    rrlib::serialization::sSerialization::DeepCopy(t, *(mgr->GetObject()->GetData<T>()), NULL);
    port->BrowserPublishRaw(mgr);
  }

  static void Publish(tPortType* port, std::shared_ptr<const T>& t)
  {
    tManagerTL* mgr = tDeleteHandlerTL::GetManager(t, true);
    assert(mgr != NULL && "You should acquire buffers to publish large data with getUnusedBuffer()");
    port->Publish(mgr);
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    rrlib::serialization::sSerialization::DeepCopy(t, *(mgr->GetObject()->GetData<T>()), NULL);
    port->Publish(mgr);
  }

  static void SetBounds(tPortType* port, const tBounds<T>& b)
  {
    static_cast<typename tPortTypeMap<T>::tBoundedPortBaseType*>(port)->SetBounds(b);
  }
};


template<typename T>
class tPortUtil : public tPortUtilBase < T, typeutil::tIsCCType<T>::value, boost::is_enum<T>::value, boost::is_integral<T>::value || boost::is_floating_point<T>::value >
{

};

template<typename T, bool GETVALUE>
class tPortParentBase : public tPortWrapperBase<typename tPortTypeMap<T>::tPortBaseType>
  {};

template<typename T>
class tPortParentBase<T, true> : public tPortWrapperBase<typename tPortTypeMap<T>::tPortBaseType>
{
public:
  /*!
   * \return Port's current value
   */
  T GetValue()
  {
    T t;
    tPortUtil<T>::GetValue(this->wrapped, t);
    return t;
  }

};

template<typename T>
class tPortParent : public tPortParentBase < T, typeutil::tIsCCType<T>::value || boost::is_enum<T>::value || boost::is_integral<T>::value || boost::is_floating_point<T>::value >
  {};

} // namespace finroc
} // namespace core

#endif // core__port__tPortUtil_h__