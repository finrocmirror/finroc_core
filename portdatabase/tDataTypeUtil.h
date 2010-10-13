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
#ifndef CORE__PORTDATABASE__TDATATYPEUTIL_H
#define CORE__PORTDATABASE__TDATATYPEUTIL_H

#include "rrlib/finroc_core_utils/tJCBase.h"
#include <boost/type_traits/has_virtual_destructor.hpp>

namespace finroc
{
namespace core
{

class tDataType;
class tPortData;
class tMethodCall;
class tTransaction;

/**
 * Contains various (C++ -specific) helper constructs
 * for data type management
 */
class tDataTypeUtil
{

public:
  // Helper methods for determining whether a type should be considered a "cheap copy" type
  static bool GetCCType(tPortData* x)
  {
    return false;
  }
  static bool GetCCType(tMethodCall* x)
  {
    return false;
  }
  static bool GetCCType(void* x)
  {
    return true;
  }

  // Helper methods for determining whether a type has a virtual table
  template <typename T>
  static bool HasVTable(T* t)
  {
    return HasVTableHelper(boost::has_virtual_destructor<T>());
  }
  static bool HasVTableHelper(const boost::true_type&)
  {
    return true;
  }
  static bool HasVTableHelper(const boost::false_type&)
  {
    return true;
  }

  // Helper methods for determining whether a type is a transaction type
  static bool GetTransactionType(tTransaction* x)
  {
    return true;
  }
  static bool GetTransactionType(void* x)
  {
    return false;
  }
};

template <typename T>
class tDataTypeLookup
{
public:
  static tDataType* type;
};

} // namespace finroc
} // namespace core

#include "core/portdatabase/tDataTypeUtil.hpp"


#endif // CORE__PORTDATABASE__TDATATYPEUTIL_H
