/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
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
#include "core/port/tGenericPortImpl.h"

#include "core/port/tPort.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/std/tPortBase.h"

using namespace rrlib::logging;

namespace finroc
{
namespace core
{
namespace internal
{

template <typename T>
class tGenericPortImplTyped : public tGenericPortImpl
{
public:
  tPort<T> port;

  tGenericPortImplTyped(const tPortCreationInfoBase& pci) :
    port(pci)
  {}

  virtual void Get(rrlib::rtti::tGenericObject& result, rrlib::time::tTimestamp& timestamp)
  {
    port.Get(*result.GetData<T>(), timestamp);
  }

  virtual tAbstractPort* GetWrapped()
  {
    return port.GetWrapped();
  }

  virtual void Publish(const rrlib::rtti::tGenericObject& data, const rrlib::time::tTimestamp& timestamp)
  {
    port.Publish(*data.GetData<T>(), timestamp);
  }

  virtual void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max)
  {
    FINROC_LOG_PRINT(eLL_ERROR, "Cannot set bounds for type ", rrlib::rtti::tDataType<T>().GetName());
  }
};

template <typename T>
class tGenericPortImplNumeric : public tGenericPortImplTyped<T>
{
public:

  tGenericPortImplNumeric(const tPortCreationInfoBase& pci) :
    tGenericPortImplTyped<T>(pci)
  {}

  virtual void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max)
  {
    tGenericPortImplTyped<T>::port.SetBounds(tBounds<T>(*min.GetData<T>(), *max.GetData<T>()));
  }
};

class tGenericPortImplCC : public tGenericPortImpl
{
public:
  tCCPortBase* port;

  tGenericPortImplCC(const tPortCreationInfoBase& pci) :
    port(new tCCPortBase(pci))
  {
    if (pci.DefaultValueSet())
    {
      rrlib::serialization::tInputStream is(&pci.GetDefaultGeneric());
      is >> (*port->GetDefaultBufferRaw());

      // publish for value caching in Parameter classes
      tCCPortDataManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(port->GetDataType());
      mgr->GetObject()->DeepCopyFrom(port->GetDefaultBufferRaw());
      util::tString error = port->BrowserPublishRaw(mgr);
      if (error.size() > 0)
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Could not set default value: ", error);
      }
    }
  }

  virtual void Get(rrlib::rtti::tGenericObject& result, rrlib::time::tTimestamp& timestamp)
  {
    port->GetRaw(result, timestamp);
  }

  virtual tAbstractPort* GetWrapped()
  {
    return port;
  }

  virtual void Publish(const rrlib::rtti::tGenericObject& data, const rrlib::time::tTimestamp& timestamp)
  {
    assert(data.GetType() == port->GetDataType());
    tCCPortDataManagerTL* mgr = tThreadLocalCache::GetFast()->GetUnusedBuffer(data.GetType());
    mgr->GetObject()->DeepCopyFrom(&data);
    mgr->SetTimestamp(timestamp);
    port->Publish(mgr);
  }

  virtual void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max)
  {
    FINROC_LOG_PRINT(eLL_ERROR, "Cannot set bounds for type ", port->GetDataType().GetName());
  }
};

class tGenericPortImplStd : public tGenericPortImpl
{
public:
  tPortBase* port;

  tGenericPortImplStd(const tPortCreationInfoBase& pci) :
    port(new tPortBase(pci))
  {
    if (pci.DefaultValueSet())
    {
      rrlib::serialization::tInputStream is(&pci.GetDefaultGeneric());
      is >> (*port->GetDefaultBufferRaw());
    }
  }

  virtual void Get(rrlib::rtti::tGenericObject& result, rrlib::time::tTimestamp& timestamp)
  {
    tPortDataManager* mgr = port->GetLockedUnsafeRaw();
    rrlib::rtti::tGenericObject* buf = mgr->GetObject();
    result.DeepCopyFrom(buf);
    timestamp = mgr->GetTimestamp();
    mgr->ReleaseLock();
  }

  virtual tAbstractPort* GetWrapped()
  {
    return port;
  }

  virtual void Publish(const rrlib::rtti::tGenericObject& data, const rrlib::time::tTimestamp& timestamp)
  {
    assert(data.GetType() == port->GetDataType());
    tPortDataManager* mgr = port->GetUnusedBufferRaw();
    mgr->GetObject()->DeepCopyFrom(&data);
    mgr->SetTimestamp(timestamp);
    port->Publish(mgr);
  }

  virtual void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max)
  {
    FINROC_LOG_PRINT(eLL_ERROR, "Cannot set bounds for type ", port->GetDataType().GetName());
  }
};

} // namespace internal

tGenericPortImpl* tGenericPortImpl::CreatePortImpl(const tPortCreationInfoBase& pci)
{
  assert(pci.data_type != NULL);
  int t = pci.data_type.GetTypeTraits();
  if ((t & rrlib::rtti::trait_flags::cIS_INTEGRAL) || (t & rrlib::rtti::trait_flags::cIS_FLOATING_POINT))
  {
    if (pci.data_type.GetRttiName() == typeid(int8_t).name())
    {
      return new internal::tGenericPortImplNumeric<int8_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(int16_t).name())
    {
      return new internal::tGenericPortImplNumeric<int16_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(int32_t).name())
    {
      return new internal::tGenericPortImplNumeric<int32_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(int64_t).name())
    {
      return new internal::tGenericPortImplNumeric<int64_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(uint8_t).name())
    {
      return new internal::tGenericPortImplNumeric<uint8_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(uint16_t).name())
    {
      return new internal::tGenericPortImplNumeric<uint16_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(uint32_t).name())
    {
      return new internal::tGenericPortImplNumeric<uint32_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(uint64_t).name())
    {
      return new internal::tGenericPortImplNumeric<uint64_t>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(float).name())
    {
      return new internal::tGenericPortImplNumeric<float>(pci);
    }
    else if (pci.data_type.GetRttiName() == typeid(double).name())
    {
      return new internal::tGenericPortImplNumeric<double>(pci);
    }
  }
  else if (pci.data_type.GetRttiName() == typeid(std::string).name())
  {
    return new internal::tGenericPortImplTyped<std::string>(pci);
  }
  else if (pci.data_type.GetRttiName() == typeid(util::tString).name())
  {
    return new internal::tGenericPortImplTyped<util::tString>(pci);
  }
  else if (pci.data_type.GetRttiName() == typeid(bool).name())
  {
    return new internal::tGenericPortImplTyped<bool>(pci);
  }
  else if (tFinrocTypeInfo::IsCCType(pci.data_type))
  {
    return new internal::tGenericPortImplCC(pci);
  }
  else if (tFinrocTypeInfo::IsStdType(pci.data_type))
  {
    return new internal::tGenericPortImplStd(pci);
  }
  FINROC_LOG_PRINT(eLL_ERROR, "Cannot create port for type ", pci.data_type.GetName());
  return NULL;
}

} // namespace finroc
} // namespace core

