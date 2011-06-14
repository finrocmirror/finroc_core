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
#ifndef core__port__rpc__tParameterUtil_h__
#define core__port__rpc__tParameterUtil_h__

#include "core/port/rpc/tCallParameter.h"
#include "core/portdatabase/typeutil.h"
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/cc/tCCPortDataManagerTL.h"

/*!
 * This class contains classes with static helper functions
 * that allow handling parameters of "tAbstractCall"s in a
 * uniform way.
 */

namespace finroc
{
namespace core
{

namespace tParamaterUtilHelper
{
tCCPortDataManager* GetInterThreadBuffer(rrlib::serialization::tDataTypeBase dt);
}

// Pass-By-Value variant
template<typename T, bool NUM>
class tParameterUtilBase
{
public:

  static void Cleanup(const T& t)
  {
    // do nothing
  }

  static bool HasLock(const T& t)
  {
    return true;
  }

  static void GetParam(tCallParameter* p, T& val)
  {
    val = p->value->GetData<T>();
    p->Clear();
  }

  static void AddParam(tCallParameter* p, const T& val)
  {
    p->type = tCallParameter::cOBJECT;
    static_assert(typeutil::tIsCCType<T>::value, "Only CC types may be passed by value");
    tCCPortDataManager* mgr = tParamaterUtilHelper::GetInterThreadBuffer(rrlib::serialization::tDataType<T>());
    p->value = tPortDataPtr<rrlib::serialization::tGenericObject>(mgr->GetObject(), mgr);
  }
};

// Variant for shared_pointers
template <typename T, bool NUM>
class tParameterUtilBase<tPortDataPtr<T>, NUM>
{
public:
  static void Cleanup(const tPortDataPtr<T>& t)
  {
    // do nothing
  }

  static bool HasLock(const tPortDataPtr<T>& t)
  {
    return true;
  }

  static void GetParam(tCallParameter* p, tPortDataPtr<T>& val)
  {
    //val.reset(p->value, p->value->GetData<T>());
    val = std::move(p->value);
    p->Clear();
  }

  static void AddParam(tCallParameter* p, tPortDataPtr<T>& val)
  {
    typename tPortDataPtr<T>::tManager* mgr = val.GetManager();
    if (mgr != NULL)
    {
      p->type = tCallParameter::cOBJECT;
      p->value = std::move(val);
    }
    else
    {
      p->type = tCallParameter::cNULLPARAM;
    }
  }
};

// Variant for numbers
template <typename T>
class tParameterUtilBase<T, true>
{
public:
  static void Cleanup(const T& t)
  {
    // do nothing
  }

  static bool HasLock(const T& t)
  {
    return true;
  }

  static void GetParam(tCallParameter* p, T& val)
  {
    val = p->number.Value<T>();
    p->Clear();
  }

  static void AddParam(tCallParameter* p, const T& val)
  {
    p->type = tCallParameter::cNUMBER;
    p->number.SetValue(val);
  }
};

template <typename T>
class tParameterUtil : public tParameterUtilBase < T, boost::is_enum<T>::value || boost::is_integral<T>::value || boost::is_floating_point<T>::value >
{
};

} // namespace finroc
} // namespace core


#endif // core__port__rpc__tParameterUtil_h__
