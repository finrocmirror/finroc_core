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

#ifndef core__port__tPortQueueFragment_h__
#define core__port__tPortQueueFragment_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortTypeMap.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Port queue fragment.
 * Can be used to dequeue all values in port queue at once.
 */
template <typename T>
class tPortQueueFragment;


template<typename T, bool NUM>
class tPortQueueFragmentBase : public util::tUncopyableObject
{
  typedef typename tPortTypeMap<T>::tQueueFragment tQueueFragment;
  typedef typename tPortTypeMap<T>::tManagerType tManager;
  typedef typename tPortTypeMap<T>::tPortBaseType tPortType;
  friend class tPort<T>;

  tQueueFragment wrapped;

  void DequeueFromPort(tPortType* port)
  {
    port->dequeueAllRaw(wrapped);
  }

public:

  tPortQueueFragmentBase() : wrapped()
  {}

  /*!
   * Dequeue one queue element.
   * Returned object needs to be unlocked manually.
   *
   * \return Next element in QueueFragment
   */
  inline tPortDataPtr<const T> Dequeue()
  {
    tManager* mgr = wrapped.DequeueUnsafe();
    return tPortDataPtr<T>(mgr);
  }

  /*!
   * Dequeue one queue element.
   *
   * \param result Buffer to (deep) copy dequeued value to
   * (Using this dequeueSingle()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   * \return true if element was dequeued - false if queue was empty
   */
  inline bool Dequeue(T& result)
  {
    tManager* mgr = wrapped.DequeueUnsafe();
    if (mgr != NULL)
    {
      rrlib::serialization::tGenericObject* go = mgr->GetObject();
      rrlib::serialization::sSerialization::DeepCopy(*(go->GetData<T>()), result);
      mgr->HandlePointerRelease();
      return true;
    }
    return false;
  }

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline const T* DequeueAutoLocked()
  {
    rrlib::serialization::tGenericObject* go = wrapped.DequeueAutoLocked();
    if (go == NULL)
    {
      return NULL;
    }
    return go->GetData<T>();
  }
};


template<typename T>
class tPortQueueFragmentBase<T, true> : public util::tUncopyableObject
{
  typedef typename tPortTypeMap<T>::tQueueFragment tQueueFragment;
  typedef typename tPortTypeMap<T>::tManagerType tManager;
  typedef typename tPortTypeMap<T>::tPortBaseType tPortType;
  friend class tPort<T>;

  tQueueFragment wrapped;
  tUnit* unit;

  void DequeueFromPort(tPortType* port)
  {
    port->DequeueAllRaw(wrapped);
    unit = port->GetUnit();
  }

public:

  tPortQueueFragmentBase() : wrapped(), unit(&tUnit::cNO_UNIT)
  {}


  /*!
   * Dequeue one queue element.
   * Returned object needs to be unlocked manually.
   *
   * \return Next element in QueueFragment
   */
  inline tPortDataPtr<const T> Dequeue()
  {
    // copy value and possibly convert number to correct type (a little inefficient, but should be used scarcely anyway)
    T val;
    if (Dequeue(val))
    {
      tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
      tNumber* new_num = c->GetObject()->GetData<tNumber>();
      new_num->SetValue(val, unit);
      return tPortDataPtr<const T>(new_num->GetValuePtr<T>(), c);
    }
    return tPortDataPtr<const T>();
  }

  /*!
   * Dequeue one queue element.
   *
   * \param result Buffer to (deep) copy dequeued value to
   * (Using this dequeueSingle()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   * \return true if element was dequeued - false if queue was empty
   */
  inline bool Dequeue(T& result)
  {
    tManager* mgr = wrapped.DequeueUnsafe();
    if (mgr != NULL)
    {
      tNumber num;
      rrlib::serialization::tGenericObject* go = mgr->GetObject();
      rrlib::serialization::sSerialization::DeepCopy(*(go->GetData<tNumber>()), num);
      mgr->HandlePointerRelease();
      if (unit != num.GetUnit() && unit != &tUnit::cNO_UNIT && num.GetUnit() != &tUnit::cNO_UNIT)
      {
        result = static_cast<T>(num.GetUnit()->ConvertTo(num.Value<double>(), unit));
      }
      else
      {
        result = num.Value<T>();
      }
      return true;
    }
    return false;
  }

  /*!
   * Dequeue one queue element.
   * Returned element will be automatically unlocked
   *
   * \return Next element in QueueFragment
   */
  inline const T* DequeueAutoLocked()
  {
    // copy value and possibly convert number to correct type (a little inefficient, but should be used scarcely anyway)
    T val;
    if (Dequeue(val))
    {
      tCCPortDataManager* c = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(tNumber::cTYPE);
      tNumber* new_num = c->GetObject()->GetData<tNumber>();
      new_num->SetValue(val, unit);
      tThreadLocalCache::GetFast()->AddAutoLock(c);
      return new_num->GetValuePtr<T>;
    }
    return NULL;
  }
};

template <typename T>
class tPortQueueFragment : public tPortQueueFragmentBase < T, boost::is_integral<T>::value || boost::is_floating_point<T>::value || boost::is_enum<T>::value > {};


} // namespace finroc
} // namespace core

#endif // core__port__tPortQueueFragment_h__
