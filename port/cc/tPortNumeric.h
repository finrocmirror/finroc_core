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

#ifndef CORE__PORT__CC__TPORTNUMERIC_H
#define CORE__PORT__CC__TPORTNUMERIC_H

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPortDataRef.h"
#include "core/datatype/tNumber.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPort.h"
#include "core/datatype/tUnit.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Port containing numbers.
 */
class tPortNumeric : public tCCPort<tNumber>
{
public:
  class tPortImplNum; // inner class forward declaration

protected:

  /*!
   * for subclasses
   */
  tPortNumeric()  {}

public:

  tPortNumeric(tPortCreationInfo pci)
  {
    this->wrapped = new tPortImplNum(pci, pci.unit);
  }

  tPortNumeric(const util::tString& description, bool output_port)
  {
    // this(new PortCreationInfo(description,outputPort ? PortFlags.OUTPUT_PORT : PortFlags.INPUT_PORT));
    this->wrapped = new tPortImplNum((tPortCreationInfo(description, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)), (tPortCreationInfo(description, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)).unit);
  }

  tPortNumeric(const util::tString& description, tFrameworkElement* parent, bool output_port)
  {
    // this(new PortCreationInfo(description,parent,outputPort ? PortFlags.OUTPUT_PORT : PortFlags.INPUT_PORT));
    this->wrapped = new tPortImplNum((tPortCreationInfo(description, parent, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)), (tPortCreationInfo(description, parent, output_port ? tPortFlags::cOUTPUT_PORT : tPortFlags::cINPUT_PORT)).unit);
  }

  template<typename T>
  inline T GetRaw()
  {
    if (PushStrategy())
    {
      for (;;)
      {
        tCCPortDataRef* val = wrapped->value;
        tNumber* cn = (tNumber*)(val->GetData());
        T d = cn->Value<T>();
        if (val == wrapped->value)
        {
          return d;
        }
      }
    }
    else
    {
      tCCPortDataContainer<tNumber>* dc = (tCCPortDataContainer<tNumber>*)wrapped->PullValueRaw();
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
    return (static_cast<tPortImplNum*>(this->wrapped))->GetUnit();
  }

  /*!
   * Set/Change port value.
   * (usually only called on output ports)
   *
   * \param d New Value
   */
  inline void Publish(double d)
  {
    tCCPortDataRef* value = this->wrapped->value;
    if (value->GetContainer()->IsOwnerThread() && (reinterpret_cast<tNumber*>(value->GetData()))->IsDouble(d, GetUnit()))
    {
      return;
    }
    tThreadLocalCache* tc = tThreadLocalCache::Get();
    tCCPortDataContainer<>* ccdc = this->wrapped->GetUnusedBuffer(tc);
    tNumber* cnc = reinterpret_cast<tNumber*>(ccdc->GetDataPtr());
    cnc->SetValue(d, GetUnit());
    this->wrapped->Publish(tc, ccdc);
  }

  /*!
   * Set/Change port value.
   * (usually only called on output ports)
   *
   * \param d New Value
   */
  inline void Publish(int d)
  {
    tCCPortDataRef* value = this->wrapped->value;
    if (value->GetContainer()->IsOwnerThread() && (reinterpret_cast<tNumber*>(value->GetData()))->IsInt(d, GetUnit()))
    {
      return;
    }
    tThreadLocalCache* tc = tThreadLocalCache::Get();
    tCCPortDataContainer<>* ccdc = this->wrapped->GetUnusedBuffer(tc);
    tNumber* cnc = reinterpret_cast<tNumber*>(ccdc->GetDataPtr());
    cnc->SetValue(d, GetUnit());
    this->wrapped->Publish(tc, ccdc);
  }

  /*!
   * Set default value for port (default default value is zero)
   * (call before port is initialized)
   *
   * \param new_default New default value
   */
  inline void SetDefault(double new_default)
  {
    ::finroc::core::tCCPort<tNumber>::SetDefault(tNumber(new_default, GetUnit()));
  }

  inline void SetDefault(int new_default)
  {
    ::finroc::core::tCCPort<tNumber>::SetDefault(tNumber(new_default, GetUnit()));
  }

  inline void SetDefault(int64 new_default)
  {
    ::finroc::core::tCCPort<tNumber>::SetDefault(tNumber(new_default, GetUnit()));
  }

  inline void SetDefault(float new_default)
  {
    ::finroc::core::tCCPort<tNumber>::SetDefault(tNumber(new_default, GetUnit()));
  }

  inline void SetDefault(const tNumber& new_default)
  {
    ::finroc::core::tCCPort<tNumber>::SetDefault(new_default);
  }

public:

  class tPortImplNum : public tCCPortBase
  {
  protected:

    /*! Unit of numerical port */
    tUnit* unit;

  public:

    tPortImplNum(tPortCreationInfo pci, tUnit* unit_) :
        tCCPortBase(pci.Derive(tNumber::cTYPE)),
        unit(unit_ != NULL ? unit_ : &(tUnit::cNO_UNIT))
    {
    }

    inline tUnit* GetUnit()
    {
      return unit;
    }

  };

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TPORTNUMERIC_H
