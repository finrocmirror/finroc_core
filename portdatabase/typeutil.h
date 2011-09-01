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
#include <type_traits>
#include "rrlib/serialization/deepcopy.h"
#include "rrlib/serialization/tTypeTraitsVector.h"

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

/*!
 * This struct is used to determine whether a type is a "cheap copy" type.
 *
 * In this case 'value' is true.
 *
 * Cheap copy types never block or allocate memory during copying.
 * As a rule of thumb, all types that could be copied by using memcpy and that are not too big (maybe 256 bytes)
 * are cheap copy types.
 *
 * Interestingly, std::has_trivial_destructor is a pretty good heuristic whether a type is a cheap copy type.
 */
template <typename T>
struct tIsCCType
{
  enum { value = std::has_trivial_destructor<T>::value && (sizeof(T) <= 256) };
};

/*!
 * Equivalent for runtime "cheap copy" type identification
 */
inline bool IsCCType(const rrlib::serialization::tDataTypeBase& dt)
{
  return dt.GetSize() <= 256 && ((dt.GetTypeTraits() & rrlib::serialization::trait_flags::cHAS_TRIVIAL_DESTRUCTOR) != 0);
}

template <typename Q, typename C>
inline C* GenericChangeTypeHelper(void (Q::*tFunc)(const C& t, int64_t i1, int64_t i2))
{
  return NULL;
}

/*!
 * This struct is used to determine the transaction-based change class for a type
 */
template <typename T>
struct tGenericChangeType
{

  template < typename Q = T >
  static decltype(GenericChangeTypeHelper(&Q::ApplyChange)) Change(Q* tc/*, decltype(HelperBasic(&Q::ApplyChange)) = NULL*/)
  {
    return NULL;
  }

  static T* Change(...)
  {
    return NULL;
  }

  typedef typename std::remove_pointer < typeof(Change((T*)1)) >::type type;
};

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
template <typename T, typename C>
inline static void ApplyChange(T& obj, const C& transaction, int64_t param1, int64_t param2, decltype(((T*)NULL)->ApplyChange(*((C*)NULL), 0, 0))* = NULL)
{
  obj.ApplyChange(transaction, param1, param2);
}

template <typename T>
inline static void ApplyChange(T& obj, const T& transaction, ...)
{
  //obj = transaction;
  rrlib::serialization::sSerialization::DeepCopy(transaction, obj, NULL);
}

template <typename T>
inline static void ApplyChange(std::vector<T>& obj, const std::vector<T>& transaction, int64_t offset, int64_t param2)
{
  size_t end_index = std::min<size_t>(obj.size(), transaction.size() + offset);
  for (size_t i = offset; i < end_index; i++)
  {
    ApplyChange(obj[i], transaction[i - offset], param2, (int64_t)0);
  }
}

} // namespace typeutil

} // namespace finroc
} // namespace core

#endif // core__portdatabase__typeutil_h__
