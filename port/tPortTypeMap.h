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

#ifndef core__port__tPortTypeMap_h__
#define core__port__tPortTypeMap_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortQueueFragmentRaw.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortBounded.h"
#include "core/port/cc/tCCPortBoundedNumeric.h"
#include "core/port/cc/tCCQueueFragmentRaw.h"
#include "core/datatype/tCoreString.h"

#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>

/*!
 * In Finroc, it is sometimes necessary to handle ports of very different types in a generic way.
 *
 * For every type T that could be used as port data, the class tPortTypeMap provides a set of types that
 * can be used for this purpose, when the type T is known at compile time.
 *
 * T:              Type that should be transferred over port (anything from float to tImage)
 * tPortBaseType:  Port class to use as backend (derived from tAbstractPort)
 * tPortDataType:  Data type to use in port backend
 * tBoundedPortBaseType: Port class to use as backend when bounds are specified (derived from tAbstractPort)
 * tManagerType:   Type of port data manager to handle this type (possibly in between threads)
 * tListType:      Type for list of T objects
 * tGenericChange: Type that can be used for generic (transaction-like) changes (e.g. in blackboards)
 * tQueueFragment: Type for port queue fragment (to dequeue several elements at once)
 *
 * //tPortType:      Type of port to handle this type
 * //tValueVar:      Type to use for function and class variables operating with non-const port values.
 * //tValueVarConst: Const Type to use for function and class variables when getting values from port.
 */
namespace finroc
{
namespace core
{

class tDummyBoundedPortBaseType : public tPortBase
{
public:
  template <typename T>
  tDummyBoundedPortBaseType(tPortCreationInfo pci, tBounds<T> t)
  {
    assert(false && "Setting bounds is only possible with cc types");
  }
};

// Standard port of type T
template <typename T>
struct tStdPortTypeMap
{

  typedef tPortBase tPortBaseType;
  typedef T tPortDataType;
  typedef tDummyBoundedPortBaseType tBoundedPortBaseType;
  typedef tPortDataManager tManagerType;
  typedef tPortQueueFragmentRaw tQueueFragment;

//  typedef tPort<T> tPortType;
//  typedef shared_ptr<T> tValueVar;
//  typedef shared_ptr<const T> tValueVarConst;
};


// CC port of type T
template <typename T>
struct tCCPortTypeMap
{

  typedef tCCPortBase tPortBaseType;
  typedef T tPortDataType;
  typedef tCCPortBounded<T> tBoundedPortBaseType;
  typedef tCCPortDataManager tManagerType;
  typedef tCCQueueFragmentRaw tQueueFragment;

//  typedef tCCPort<T> tPortType;
//  typedef T tValueVar;
//  typedef const T tValueVarConst;
};

// empty dummy
template<typename T, bool CC, bool ENUM, bool INT>
struct tPortTypeMapBase
{
};

// Numeric
template<bool ENUM, bool INT>
struct tPortTypeMapBase<tNumber, true, ENUM, INT>
{

  typedef tCCPortBase tPortBaseType;
  typedef tNumber tPortDataType;
  typedef tCCPortBoundedNumeric<double> tBoundedPortBaseType;
  typedef tCCPortDataManager tManagerType;
  typedef tCCQueueFragmentRaw tQueueFragment;

//  typedef tPortNumeric tPortType;
//  typedef T tValueVar;
//  typedef const T tValueVarConst;
};


// Numeric
template<typename T, bool CC, bool ENUM>
struct tPortTypeMapBase<T, CC, ENUM, true>
{

  typedef tCCPortBase tPortBaseType;
  typedef tNumber tPortDataType;
  typedef tCCPortBoundedNumeric<T> tBoundedPortBaseType;
  typedef tCCPortDataManager tManagerType;
  typedef tCCQueueFragmentRaw tQueueFragment;

//  typedef tPortNumeric tPortType;
//  typedef T tValueVar;
//  typedef const T tValueVarConst;
};

// Enum
template<typename T, bool CC, bool INT>
struct tPortTypeMapBase<T, CC, true, INT>
{

  typedef tCCPortBase tPortBaseType;
  typedef tNumber tPortDataType;
  typedef tCCPortBoundedNumeric<int> tBoundedPortBaseType;
  typedef tCCPortDataManager tManagerType;
  typedef tCCQueueFragmentRaw tQueueFragment;

//  typedef tPortNumeric tPortType;
//  typedef T tValueVar;
//  typedef const T tValueVarConst;
};

// String
template<>
struct tPortTypeMapBase<std::string, false, false, false> : tStdPortTypeMap<tCoreString> {};

// Finroc String
template<>
struct tPortTypeMapBase<util::tString, false, false, false> : tStdPortTypeMap<tCoreString> {};

// CC type
template<typename T, bool ENUM, bool INT>
struct tPortTypeMapBase<T, true, ENUM, INT> : tCCPortTypeMap<T> {};

// Standard type
template<typename T>
struct tPortTypeMapBase<T, false, false, false> : tStdPortTypeMap<T> {};

// Actual class
template<typename T>
struct tPortTypeMap : tPortTypeMapBase < T, typeutil::tIsCCType<T>::value, boost::is_enum<T>::value, boost::is_integral<T>::value || boost::is_floating_point<T>::value >
{
  typedef typename typeutil::tGenericChangeType<T>::type tGenericChange;
  typedef std::vector<T> tListType;
};

} // namespace finroc
} // namespace core

#endif // core__port__tPortTypeMap_h__
