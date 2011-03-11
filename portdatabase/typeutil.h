/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2011 Max Reichardt,
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
#ifndef core__portdatabase__typeutil_h__
#define core__portdatabase__typeutil_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include <boost/type_traits/has_virtual_destructor.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include "core/portdatabase/tCCType.h"
#include "rrlib/serialization/tGenericChangeable.h"
#include "rrlib/serialization/deepcopy.h"

namespace rrlib
{
namespace math
{
class tPose3D;
class tPose2D;
}
namespace util
{
class tTime;
}
}

namespace finroc
{
namespace core
{

class tMethodCall;
class tTransaction;

/**
 * Contains various (C++ -specific) helper constructs
 * for data type management
 */
namespace typeutil
{
template <bool CC, typename T>
struct tIsCCTypeBase
{
  enum { value = false };
};

template <typename T>
struct tIsCCTypeBase<true, T>
{
  enum { value = true };
};

template <bool B>
struct tIsCCTypeBase<B, rrlib::math::tPose3D>
{
  enum { value = true };
};

template <bool B>
struct tIsCCTypeBase<B, rrlib::math::tPose2D>
{
  enum { value = true };
};

template <bool B>
struct tIsCCTypeBase<B, rrlib::util::tTime>
{
  enum { value = true };
};

/*!
 * This struct is used to determine whether a type is a "cheap copy" type.
 * In this case 'value' is true.
 */
template <typename T>
struct tIsCCType : tIsCCTypeBase<boost::is_base_of<tCCType, T>::value, T> {};

/*!
 * This struct is used to determine the transaction-based change class for a type
 */
template <typename T>
struct tGenericChangeType
{

  template<typename C>
  static C* Change(rrlib::serialization::tGenericChangeable<C>* tc)
  {
    return NULL;
  }

  static T* Change(void* x)
  {
    return NULL;
  }

  typedef typename boost::remove_pointer < typeof(Change((T*)1)) >::type type;
};

// Helper methods for determining whether a type has a virtual table
template <typename T>
inline static bool HasVTable(T* t)
{
  return HasVTableHelper(boost::has_virtual_destructor<T>());
}
inline static bool HasVTableHelper(const boost::true_type&)
{
  return true;
}
inline static bool HasVTableHelper(const boost::false_type&)
{
  return false;
}

// Helper methods for determining whether a type is a transaction type
/*static bool GetTransactionType(tTransaction* x)
{
  return true;
}*/
inline static bool GetTransactionType(void* x)
{
  return false;
}

// Helper method to apply generic change to object
template <typename C>
inline static void ApplyChange(rrlib::serialization::tGenericChangeable<C>& obj, const C& transaction, int64_t param1, int64_t param2)
{
  obj.ApplyChange(transaction, param1, param2);
}

template <typename T>
inline static void ApplyChange(std::vector<T>& obj, const std::vector<T>& transaction, int64_t offset, int64_t param2)
{
  size_t end_index = std::min(obj.size(), transaction.size() + offset);
  for (size_t i = offset; i < end_index; i++)
  {
    ApplyChange(obj[i], transaction[i - offset], param2, 0);
  }
}

template <typename T>
inline static void ApplyChange(T& obj, const T& transaction, int64_t param1, int64_t param2)
{
  //obj = transaction;
  rrlib::serialization::deepcopy::Copy(transaction, obj, NULL);
}

void InitCCTypes();
} // namespace typeutil

} // namespace finroc
} // namespace core

#endif // core__portdatabase__typeutil_h__
