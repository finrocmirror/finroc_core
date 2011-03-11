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
#ifndef core__port__tPortListenerAdapter_h__
#define core__port__tPortListenerAdapter_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/tListenerManager.h"
#include "core/port/tPortListenerRaw.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/datatype/tUnit.h"
#include "core/datatype/tNumber.h"
#include "rrlib/serialization/tGenericObjectManager.h"

namespace finroc
{
namespace core
{
class tAbstractPort;

namespace detail
{
/**
 * \return Unit of cc port
 */
tUnit* GetUnit(tAbstractPort* cc_port);
}

/*!
 * \author Max Reichardt
 *
 * (Internal class)
 * Converts Managed data to type T that user wants to have
 */
template < typename T, bool CC, bool NUM >
class tPortListenerAdapter : public tPortListenerRaw
{
public:

  /*!
   * Called whenever port's value has changed
   *
   * \param origin Port that value comes from
   * \param value Port's new value (locked for duration of method call)
   */
  virtual void PortChanged(tAbstractPort* origin, const T& value) = 0;

  virtual void PortChangedRaw(tAbstractPort* origin, const tGenericObjectManager* value)
  {
    PortChanged(origin, *value->GetObject()->GetData<T>());
  }
};

/*! variant for shared_ptr with auto-lock-release */
template < typename T >
class tPortListenerAdapter<std::shared_ptr<const T>, false, false> : public tPortListenerRaw
{
public:

  virtual void PortChanged(tAbstractPort* origin, const std::shared_ptr<const T>& value) = 0;

  virtual void PortChangedRaw(tAbstractPort* origin, const tGenericObjectManager* value)
  {
    assert(typeid(*value).name() == typeid(tPortDataManager).name());
    tPortDataManager* mgr = static_cast<tPortDataManager*>(value);
    mgr->AddLock();
    PortChanged(origin, std::shared_ptr<const T>(mgr->GetObject()->GetData<T>(), tSharedPtrDeleteHandler<tPortDataManager>(mgr)));
  }
};

/*! variant for shared_ptr with auto-lock-release (CC type - slightly inefficient) */
template < typename T >
class tPortListenerAdapter<std::shared_ptr<const T>, true, false> : public tPortListenerRaw
{
public:

  virtual void PortChanged(tAbstractPort* origin, const std::shared_ptr<const T>& value) = 0;

  virtual void PortChangedRaw(tAbstractPort* origin, const tGenericObjectManager* value)
  {
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(value->GetObject()->GetType());
    c->GetObject()->DeepCopyFrom(value->GetObject());
    PortChanged(origin, std::shared_ptr<const T>(c->GetObject()->GetData<T>(), tSharedPtrDeleteHandler<tCCPortDataManager>(c)));
  }
};

/*! variant for numbers */
template < typename T, bool CC>
class tPortListenerAdapter<T, CC, true> : public tPortListenerRaw
{
public:

  virtual void PortChanged(tAbstractPort* origin, const T& value) = 0;

  virtual void PortChangedRaw(tAbstractPort* origin, const tGenericObjectManager* value)
  {
    const tNumber* num = value->GetObject()->GetData<tNumber>();
    tUnit* port_unit = detail::GetUnit(origin);
    if (port_unit != num->GetUnit() && port_unit != &tUnit::cNO_UNIT && num->GetUnit() != &tUnit::cNO_UNIT)
    {
      PortChanged(origin, static_cast<T>(num->GetUnit()->ConvertTo(num->Value<double>(), port_unit)));
    }
    else
    {
      PortChanged(origin, num->Value<T>());
    }
  }
};

/*! variant for numbers in shared_pointers (possibly required for weird templates ;-) ) */
template < typename T, bool CC>
class tPortListenerAdapter<std::shared_ptr<const T>, CC, true> : public tPortListenerRaw
{
public:

  virtual void PortChanged(tAbstractPort* origin, const std::shared_ptr<const T>& value) = 0;

  virtual void PortChangedRaw(tAbstractPort* origin, const tGenericObjectManager* value)
  {
    const tNumber* num = value->GetObject()->GetData<tNumber>();
    tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
    tNumber* new_num = c->GetObject()->GetData<tNumber>();
    tUnit* port_unit = detail::GetUnit(origin);
    if (port_unit != num->GetUnit() && port_unit != &tUnit::cNO_UNIT && num->GetUnit() != &tUnit::cNO_UNIT)
    {
      new_num->SetValue(static_cast<T>(num->GetUnit()->ConvertTo(num->Value<double>(), port_unit)), port_unit);
    }
    else
    {
      new_num->SetValue(num->Value<T>(), num->GetUnit());
    }
    PortChanged(origin, std::shared_ptr<const T>(new_num->GetValuePtr<T>(), tSharedPtrDeleteHandler<tCCPortDataManager>(c)));
  }
};

} // namespace finroc
} // namespace core

#endif // core__port__tPortListenerAdapter_h__