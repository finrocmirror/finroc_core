/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#ifndef core__plugin__tParamType_h__
#define core__plugin__tParamType_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "core/parameter/tStructureParameterBool.h"
#include "core/parameter/tStructureParameterNumeric.h"
#include "core/parameter/tStructureParameterString.h"
#include "core/parameter/tStructureParameterEnum.h"

#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_enum.hpp>

namespace finroc
{
namespace core
{

struct tEmpty
{
public:
  tEmpty() {}
  tEmpty(const util::tString& name) {}
};

/**
 * Helper template that maps constructor parameter types on StructureParameter classes
 */

template <typename T, bool ENUM, bool PTR>
struct tParamTypeBase
{
  typedef tStructureParameter<T> t;
  static T Get(tStructureParameterBase* x)
  {
    return *(static_cast<t*>(x)->GetValue());
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

template <typename T>
struct tParamType : tParamTypeBase<T, boost::is_enum<T>::value, boost::is_pointer<T>::value> {};

template <typename T>
struct tParamTypeBase<T, false, true>
{
  typedef tStructureParameter<typename boost::remove_pointer<T>::type> t;
  static T Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->GetValue();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

template <typename T>
struct tNumParamType
{
  typedef tStructureParameterNumeric<T> t;
  static T Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->Get();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, 0, true);
  }
};

template <> struct tParamType<int8> : tNumParamType<int8> {};
template <> struct tParamType<int16> : tNumParamType<int16> {};
template <> struct tParamType<int> : tNumParamType<int> {};
template <> struct tParamType<int64> : tNumParamType<int64> {};
template <> struct tParamType<uint8_t> : tNumParamType<uint8_t> {};
template <> struct tParamType<uint16_t> : tNumParamType<uint16_t> {};
template <> struct tParamType<uint32_t> : tNumParamType<uint32_t> {};
template <> struct tParamType<uint64_t> : tNumParamType<uint64_t> {};

template <>
struct tParamType<const char*>
{
  typedef tStructureParameterString t;
  static const char* Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->GetValue()->GetBuffer().GetCString();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

template <>
struct tParamType<std::string>
{
  typedef tStructureParameterString t;
  static std::string Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->GetValue()->GetBuffer().GetStdString();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

template <>
struct tParamType<util::tString>
{
  typedef tStructureParameterString t;
  static util::tString Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->Get();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

template <>
struct tParamType<bool>
{
  typedef tStructureParameterBool t;
  static bool Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->Get();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, false, true);
  }
};

template <>
struct tParamType<tEmpty>
{
  typedef tEmpty t;
  static tEmpty Get(tStructureParameterBase* x)
  {
    assert(false);
    return tEmpty();
  }
  static t* Create(const util::tString& name)
  {
    return NULL;
  }
};

template <typename T>
struct tParamTypeBase<T, true, false>
{
  typedef tStructureParameterEnum<T> t;
  static T Get(tStructureParameterBase* x)
  {
    return static_cast<t*>(x)->Get();
  }
  static t* Create(const util::tString& name)
  {
    return new t(name, true);
  }
};

} // namespace finroc
} // namespace core

#endif // core__plugin__tParamType_h__
