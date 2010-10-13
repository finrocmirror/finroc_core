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

#ifndef CORE__PORT__CC__TNUMBERPORT_H
#define CORE__PORT__CC__TNUMBERPORT_H

#include "core/port/tPortCreationInfo.h"
#include "core/datatype/tUnit.h"
#include "core/port/tPortFlags.h"
#include "core/datatype/tCoreNumber.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPortDataRef.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Port containing numbers.
 */
class tNumberPort : public tCCPort<tCoreNumber>
{
private:

  /*! Unit of numerical port */
  tUnit* unit;

  inline static tPortCreationInfo ProcessPciNP(tPortCreationInfo pci)
  {
    pci.data_type = tCoreNumber::GetDataType();
    return pci;
  }

public:

  tNumberPort(tPortCreationInfo pci) :
      tCCPort<tCoreNumber>(ProcessPciNP(pci)),
      unit(pci.unit != NULL ? pci.unit : &(tUnit::cNO_UNIT))
  {
  }

  tNumberPort(const util::tString& description, bool output_port) :
      tCCPort<tCoreNumber>(ProcessPciNP((tPortCreationInfo(description, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)))),
      unit(&(tUnit::cNO_UNIT))
  {
    // this(new PortCreationInfo(description,outputPort ? PortFlags.OUTPUT_PORT : PortFlags.INPUT_PORT));
  }

  template<typename T>
  inline T GetRaw()
  {
    if (PushStrategy())
    {
      for (;;)
      {
        tCCPortDataRef* val = value;
        tCoreNumber* cn = (tCoreNumber*)(val->GetData());
        T d = cn->Value<T>();
        if (val == value)
        {
          return d;
        }
      }
    }
    else
    {
      tCCPortDataContainer<tCoreNumber>* dc = (tCCPortDataContainer<tCoreNumber>*)PullValueRaw();
      T result = dc->GetData()->Value<T>();
      dc->ReleaseLock();
      return result;
    }
  }

  /*!
   * Get double value of port ignoring unit
   */
  inline double GetDoubleRaw()
  {
    return GetRaw<double>();
  }

  /*!
   * Get int value of port ignoring unit
   */
  inline int GetIntRaw()
  {
    return GetRaw<int>();
  }

  /*!
   * \return Unit of port
   */
  inline tUnit* GetUnit()
  {
    return unit;
  }

  /*!
   * Set/Change port value.
   * (usually only called on output ports)
   *
   * \param d New Value
   */
  inline void Publish(double d)
  {
    tCCPortDataRef* value = this->value;
    if (value->GetContainer()->IsOwnerThread() && (reinterpret_cast<tCoreNumber*>(value->GetData()))->IsDouble(d, unit))
    {
      return;
    }
    tThreadLocalCache* tc = tThreadLocalCache::Get();
    tCCPortDataContainer<>* ccdc = ::finroc::core::tCCPortBase::GetUnusedBuffer(tc);
    tCoreNumber* cnc = reinterpret_cast<tCoreNumber*>(ccdc->GetDataPtr());
    cnc->SetValue(d, unit);
    ::finroc::core::tCCPortBase::Publish(tc, ccdc);
  }

  /*!
   * Set/Change port value.
   * (usually only called on output ports)
   *
   * \param d New Value
   */
  inline void Publish(int d)
  {
    if (this->value->GetContainer()->IsOwnerThread() && (reinterpret_cast<tCoreNumber*>(this->value->GetData()))->IsInt(d, unit))
    {
      return;
    }
    tThreadLocalCache* tc = tThreadLocalCache::Get();
    tCCPortDataContainer<>* ccdc = ::finroc::core::tCCPortBase::GetUnusedBuffer(tc);
    tCoreNumber* cnc = reinterpret_cast<tCoreNumber*>(ccdc->GetDataPtr());
    cnc->SetValue(d, unit);
    ::finroc::core::tCCPortBase::Publish(tc, ccdc);
  }

  /*!
   * Set default value for port (default default value is zero)
   * (call before port is initialized)
   *
   * \param new_default New default value
   */
  inline void SetDefault(double new_default)
  {
    ::finroc::core::tCCPort<tCoreNumber>::GetDefaultBuffer()->SetValue(new_default, unit);
  }

  inline void SetDefault(int new_default)
  {
    ::finroc::core::tCCPort<tCoreNumber>::GetDefaultBuffer()->SetValue(new_default, unit);
  }

  inline void SetDefault(int64 new_default)
  {
    ::finroc::core::tCCPort<tCoreNumber>::GetDefaultBuffer()->SetValue(new_default, unit);
  }

  inline void SetDefault(float new_default)
  {
    ::finroc::core::tCCPort<tCoreNumber>::GetDefaultBuffer()->SetValue(new_default, unit);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TNUMBERPORT_H
