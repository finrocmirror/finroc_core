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
#include "rrlib/rtti/rtti.h"
#include "core/port/tPortTypeMap.h"
#include "core/port/tPortDataPtr.h"

namespace finroc
{
namespace core
{

// is friend of tPortDatatPtr
class tPortUtilHelper
{
public:
  template <typename M, typename T>
  static M* ResetManager(tPortDataPtr<T>& data)
  {
    M* mgr = data.manager_tl;
    if (data.mode == tPortDataPtrBase::eUNUSED)
    {
      data.manager_tl = NULL;
      data.data = NULL;
    }
    return mgr;
  }
};

/*!
 * \author Max Reichardt
 *
 * Implementations of various methods in tPort class
 */
template <typename T, bool CC, bool NUM>
class tPortUtilBase
{

public:
  typedef tPortDataManager tManager;
  typedef tPortDataManager tManagerTL;
  typedef tPortBase tPortType;
  typedef tPortDataPtr<T> tDataPtr;
  typedef tPortDataPtr<const T> tConstDataPtr;

  static tDataPtr GetUnusedBuffer(tPortType* port)
  {
    return tDataPtr(port->GetUnusedBufferRaw(), tPortDataPtrBase::eUNUSED);
  }

  static tConstDataPtr GetValueWithLock(tPortType* port)
  {
    return tConstDataPtr(port->GetLockedUnsafeRaw());
  }

  static tConstDataPtr DequeueSingle(tPortType* port)
  {
    return tConstDataPtr(port->DequeueSingleUnsafeRaw());
  }

  static tConstDataPtr GetPull(tPortType* port, bool intermediate_assign)
  {
    return tConstDataPtr(port->GetPullLockedUnsafe(intermediate_assign, false));
  }

  static void GetValue(tPortType* port, T& result)
  {
    tManager* mgr = port->GetLockedUnsafeRaw();
    rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
    mgr->ReleaseLock();
  }

  static bool DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    if (mgr != NULL)
    {
      rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
      mgr->ReleaseLock();
      return true;
    }
    return false;
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    return port->GetAutoLockedRaw()->GetObject()->GetData<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    tManager* mgr = port->DequeueSingleAutoLockedRaw();
    if (mgr == NULL)
    {
      return NULL;
    }
    rrlib::rtti::tGenericObject* go = mgr->GetObject();
    return go->GetData<T>();
  }

  static void SetDefault(tPortType* port, rrlib::serialization::tInputStream& t)
  {
    rrlib::rtti::tGenericObject* go = port->GetDefaultBufferRaw();
    t >> (*go);
  }

  static void Publish(tPortType* port, tConstDataPtr& t)
  {
    port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
    t.reset();
  }

  static void Publish(tPortType* port, tDataPtr& t)
  {
    port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
    t.reset();
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    tDataPtr buf = GetUnusedBuffer(port);
    rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(t, *buf);
    Publish(port, buf);
  }

  static const tBounds<T> GetBounds(tPortType* port)
  {
    assert(false && "Bounds can only be used with CC types");
    return *((tBounds<T>*)NULL); // dummy statement to make compiler happy
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
  typedef tCCPortBase tPortType;
  typedef tPortDataPtr<T> tDataPtr;
  typedef tPortDataPtr<const T> tConstDataPtr;

  static tDataPtr GetUnusedBuffer(tPortType* port)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    return tDataPtr(mgr->GetObject()->GetData<tNumber>()->GetValuePtr<T>(), mgr);
  }

  static tConstDataPtr GetValueWithLock(tPortType* port)
  {

    // copy value and possibly convert number to correct type (a little inefficient, but should be used scarcely anyway)
    T val;
    GetValue(port, val);
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    new_num->SetValue(val, port->GetUnit());
    return tConstDataPtr(new_num->GetValuePtr<T>(), c);
  }

  static tConstDataPtr DequeueSingle(tPortType* port)
  {
    T val;
    if (DequeueSingle(port, val))
    {
      tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
      tNumber* new_num = c->GetObject()->GetData<tNumber>();
      new_num->SetValue(val, port->GetUnit());
      return tConstDataPtr(new_num->GetValuePtr<T>(), c);
    }
    else
    {
      return tConstDataPtr();
    }
  }

  static tConstDataPtr GetPull(tPortType* port, bool intermediate_assign)
  {
    tManager* mgr = port->GetPullInInterthreadContainerRaw(intermediate_assign, false);
    tNumber* num = mgr->GetObject()->GetData<tNumber>();
    if (port->GetUnit() != num->GetUnit() && port->GetUnit() != &tUnit::cNO_UNIT && num->GetUnit() != &tUnit::cNO_UNIT)
    {
      num->SetValue(static_cast<T>(num->GetUnit()->ConvertTo(num->Value<double>(), port->GetUnit())));
    }
    return tConstDataPtr(num->GetValuePtr<T>(), mgr);
  }

  static void GetValue(tPortType* port, T& result)
  {
    tNumber num;
    port->GetRawT(num);
    if (port->GetUnit() != num.GetUnit() && port->GetUnit() != &tUnit::cNO_UNIT && num.GetUnit() != &tUnit::cNO_UNIT)
    {
      result = static_cast<T>(num.GetUnit()->ConvertTo(num.Value<double>(), port->GetUnit()));
    }
    else
    {
      result = num.Value<T>();
    }
  }

  static bool DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    if (mgr != NULL)
    {
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
      return true;
    }
    return false;
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    T val;
    GetValue(port, val);
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    new_num->SetValue(val, port->GetUnit());
    tThreadLocalCache::GetFast()->AddAutoLock(c);
    return new_num->GetValuePtr<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    T val;
    if (DequeueSingle(port, val))
    {
      tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
      tNumber* new_num = c->GetObject()->GetData<tNumber>();
      new_num->SetValue(val, port->GetUnit());
      tThreadLocalCache::GetFast()->AddAutoLock(c);
      return new_num->GetValuePtr<T>();
    }
    else
    {
      return NULL;
    }
  }

  static void SetDefault(tPortType* port, rrlib::serialization::tInputStream& is)
  {
    rrlib::rtti::tGenericObject* go = port->GetDefaultBufferRaw();
    tNumber* n = go->GetData<tNumber>();
    T t;
    is >> t;
    n->SetValue(t, port->GetUnit());

    // publish for value caching in Parameter classes
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(tNumber::cTYPE);
    mgr->GetObject()->DeepCopyFrom(go);
    port->BrowserPublishRaw(mgr);
  }

  static void Publish(tPortType* port, tConstDataPtr& t)
  {
    if (typeid(*t.GetManager()) == typeid(tManagerTL))
    {
      port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
      t.reset();
    }
    else
    {
      assert(typeid(*t.GetManager()) == typeid(tManager));
      CopyAndPublish(port, *t);
    }
  }

  static void Publish(tPortType* port, tDataPtr& t)
  {
    if (typeid(*t.GetManager()) == typeid(tManagerTL))
    {
      port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
      t.reset();
    }
    else
    {
      assert(typeid(*t.GetManager()) == typeid(tManager));
      CopyAndPublish(port, *t);
    }
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    tThreadLocalCache* tc = tThreadLocalCache::GetFast();
    tManagerTL* mgr = tc->GetUnusedBuffer(port->GetDataTypeCCIndex());
    mgr->GetObject()->GetData<tNumber>()->SetValue(t, port->GetUnit());
    port->Publish(tc, mgr);
  }

  static const tBounds<T> GetBounds(const tPortType* port)
  {
    assert(typeid(*port).name() == typeid(tCCPortBoundedNumeric<T>).name());
    return static_cast<const tCCPortBoundedNumeric<T>*>(port)->GetBounds();
  }

  static void SetBounds(tPortType* port, const tBounds<T>& b)
  {
    assert(typeid(*port).name() == typeid(tCCPortBoundedNumeric<T>).name());
    static_cast<tCCPortBoundedNumeric<T>*>(port)->SetBounds(b);
  }

};

template <typename T>
class tPortUtilBase<T, true, true> : public tPortUtilBaseNumeric<T>
{
};

template <typename T, bool NUM>
class tPortUtilBase<T, true, NUM>
{

public:
  typedef tCCPortDataManager tManager;
  typedef tCCPortDataManagerTL tManagerTL;
  typedef tCCPortBase tPortType;
  typedef tPortDataPtr<T> tDataPtr;
  typedef tPortDataPtr<const T> tConstDataPtr;

  static tDataPtr GetUnusedBuffer(tPortType* port)
  {
    return tDataPtr(tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType()));
  }

  static tConstDataPtr GetValueWithLock(tPortType* port)
  {
    return tConstDataPtr(port->GetInInterThreadContainer());
  }

  static tConstDataPtr DequeueSingle(tPortType* port)
  {
    return tConstDataPtr(port->DequeueSingleUnsafeRaw());
  }

  static tConstDataPtr GetPull(tPortType* port, bool intermediate_assign)
  {
    return tConstDataPtr(port->GetPullInInterthreadContainerRaw(intermediate_assign, false));
  }

  static void GetValue(tPortType* port, T& result)
  {
    port->GetRawT(result);
  }

  static bool DequeueSingle(tPortType* port, T& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    if (mgr != NULL)
    {
      rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(*(mgr->GetObject()->GetData<T>()), result);
      mgr->Recycle2();
      return true;
    }
    return false;
  }

  static const T* GetAutoLocked(tPortType* port)
  {
    return port->GetAutoLockedRaw()->GetData<T>();
  }

  static const T* DequeueSingleAutoLocked(tPortType* port)
  {
    rrlib::rtti::tGenericObject* go = port->DequeueSingleAutoLockedRaw();
    if (go == NULL)
    {
      return NULL;
    }
    return go->GetData<T>();
  }

  static void SetDefault(tPortType* port, rrlib::serialization::tInputStream& t)
  {
    rrlib::rtti::tGenericObject* go = port->GetDefaultBufferRaw();
    t >> (*go);

    // publish for value caching in Parameter classes
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    mgr->GetObject()->DeepCopyFrom(go);
    port->BrowserPublishRaw(mgr);
  }

  static void Publish(tPortType* port, tConstDataPtr& t)
  {
    if (typeid(*t.GetManager()) == typeid(tManagerTL))
    {
      port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
      t.reset();
    }
    else
    {
      assert(typeid(*t.GetManager()) == typeid(tManager));
      CopyAndPublish(port, *t);
    }
  }

  static void Publish(tPortType* port, tDataPtr& t)
  {
    if (typeid(*t.GetManager()) == typeid(tManagerTL))
    {
      port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
      t.reset();
    }
    else
    {
      assert(typeid(*t.GetManager()) == typeid(tManager));
      CopyAndPublish(port, *t);
    }
  }

  static void CopyAndPublish(tPortType* port, const T& t)
  {
    tManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
    rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(t, *(mgr->GetObject()->GetData<T>()), NULL);
    port->Publish(mgr);
  }

  static const tBounds<T> GetBounds(const tPortType* port)
  {
    assert(typeid(*port).name() == typeid(typename tPortTypeMap<T>::tBoundedPortBaseType).name());
    return static_cast<const typename tPortTypeMap<T>::tBoundedPortBaseType*>(port)->GetBounds();
  }

  static void SetBounds(tPortType* port, const tBounds<T>& b)
  {
    assert(typeid(*port).name() == typeid(typename tPortTypeMap<T>::tBoundedPortBaseType).name());
    static_cast<typename tPortTypeMap<T>::tBoundedPortBaseType*>(port)->SetBounds(b);
  }
};

template <>
class tPortUtilBaseNumeric<bool> : public tPortUtilBase<bool, true, false>
{

public:

  static const tBounds<bool> GetBounds(tPortType* port)
  {
    assert(false && "Bounds don't make sense with boolean values");
    return *((tBounds<bool>*)NULL); // dummy statement to make compiler happy
  }

  static void SetBounds(tPortType* port, const tBounds<bool>& b)
  {
    assert(false && "Bounds don't make sense with boolean values");
  }

};

template <>
class tPortUtilBase<std::string, false, false>
{

public:
  typedef tPortDataManager tManager;
  typedef tPortDataManager tManagerTL;
  typedef tPortBase tPortType;
  typedef tPortDataPtr<std::string> tDataPtr;
  typedef tPortDataPtr<const std::string> tConstDataPtr;

  static tDataPtr GetUnusedBuffer(tPortType* port)
  {
    tManagerTL* mgr = port->GetUnusedBufferRaw();
    return tDataPtr(&const_cast<std::string&>(mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef()), mgr, tPortDataPtrBase::eUNUSED);
  }

  static tConstDataPtr GetValueWithLock(tPortType* port)
  {
    tManager* mgr = port->GetLockedUnsafeRaw();
    return tConstDataPtr(&mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef(), mgr);
  }

  static tConstDataPtr DequeueSingle(tPortType* port)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    return tConstDataPtr(&mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef(), mgr);
  }

  static tConstDataPtr GetPull(tPortType* port, bool intermediate_assign)
  {
    tManager* mgr = port->GetPullLockedUnsafe(intermediate_assign, false);
    return tConstDataPtr(&mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef(), mgr);
  }

  static void GetValue(tPortType* port, std::string& result)
  {
    tManager* mgr = port->GetLockedUnsafeRaw();
    result.assign(mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef().c_str()); // hopefully, this won't destroy our tCoreString buffer (in contras to normal std::string assigment)
    mgr->ReleaseLock();
  }

  static bool DequeueSingle(tPortType* port, std::string& result)
  {
    tManager* mgr = port->DequeueSingleUnsafeRaw();
    if (mgr != NULL)
    {
      result.assign(mgr->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef().c_str()); // hopefully, this won't destroy our tCoreString buffer (in contras to normal std::string assigment)
      mgr->ReleaseLock();
      return true;
    }
    return false;
  }

  static const std::string* GetAutoLocked(tPortType* port)
  {
    return &port->GetAutoLockedRaw()->GetObject()->GetData<tCoreString>()->GetBuffer().GetStdStringRef();
  }

  static const std::string* DequeueSingleAutoLocked(tPortType* port)
  {
    tManager* mgr = port->DequeueSingleAutoLockedRaw();
    if (mgr == NULL)
    {
      return NULL;
    }
    rrlib::rtti::tGenericObject* go = mgr->GetObject();
    return &go->GetData<tCoreString>()->GetBuffer().GetStdStringRef();
  }

  static void SetDefault(tPortType* port, rrlib::serialization::tInputStream& t)
  {
    t >> (*port->GetDefaultBufferRaw()->GetData<tCoreString>());
  }

  static void Publish(tPortType* port, tConstDataPtr& t)
  {
    port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
    t.reset();
  }

  static void Publish(tPortType* port, tDataPtr& t)
  {
    port->Publish(tPortUtilHelper::ResetManager<tManagerTL>(t));
    t.reset();
  }

  static void CopyAndPublish(tPortType* port, const std::string& t)
  {
    tDataPtr buf = GetUnusedBuffer(port);
    buf->assign(t.c_str());
    Publish(port, buf);
  }

  static const tBounds<std::string> GetBounds(tPortType* port)
  {
    assert(false && "Bounds can only be used with CC types");
    return *((tBounds<std::string>*)NULL); // dummy statement to make compiler happy
  }

  static void SetBounds(tPortType* port, const tBounds<std::string>& b)
  {
    assert(false && "Bounds can only be used with CC types");
  }
};

template<typename T>
class tPortUtil : public tPortUtilBase < T, typeutil::tIsCCType<T>::value, std::is_integral<T>::value || std::is_floating_point<T>::value >
{

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortUtil_h__
