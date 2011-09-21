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

#ifndef core__port__tPortCreationInfo_h__
#define core__port__tPortCreationInfo_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortCreationInfoBase.h"
#include "core/port/tPortFlags.h"
#include "core/portdatabase/typeutil.h"
#include "core/datatype/tBounds.h"

namespace finroc
{
namespace core
{

template <typename T>
class tPortCreationInfo;

namespace internal
{
// only store bounds/default_value if we have cc type
template <typename T, bool CC>
class tStorage
{
  friend class tPortCreationInfo<T>;
  std::shared_ptr<T> default_value;
public:
  tStorage() :
      default_value()
  {}

  void SetDefault(const T& t)
  {
    default_value.reset(rrlib::serialization::tDataType<T>::CreateInstance());
    rrlib::serialization::sSerialization::DeepCopy(t, *this->default_value);
  }

  T* GetDefault()
  {
    return default_value.get();
  }

};

template <typename T>
class tStorage<T, true>
{
  friend class tPortCreationInfo<T>;
  tBounds<T> bounds;
  T default_value;
public:
  template < bool F = std::is_fundamental<T>::value >
  tStorage(typename std::enable_if<F, void>::type* v = 0) :
      bounds(),
      default_value(0)
  {}

  template < bool F = std::is_fundamental<T>::value >
  tStorage(typename std::enable_if < !F, void >::type* v = 0) :
      bounds(),
      default_value()
  {}

  void SetDefault(const T& t)
  {
    default_value = t;
  }

  T* GetDefault()
  {
    return &default_value;
  }
};

/*! type trait to determine whether a type is numeric */
template <typename T>
struct tIsNumeric
{
  enum { value = std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, tNumber>::value };
};

/*! type trait to determine whether a type is a string */
template <typename T>
struct tIsString
{
  enum { value = std::is_same<T, std::string>::value || std::is_same<T, util::tString>::value || std::is_same<T, char*>::value || std::is_same<T, const char*>::value || std::is_same<typename std::remove_extent<T>::type, char>::value || std::is_same<T, util::tStringBuilder>::value };
};

}

/*! Can be used to wrap lock order for tPortCreationInfo constructor */
struct tLockOrder
{
  int wrapped;

  tLockOrder(int i) : wrapped(i) {}
};

/*!
 * \author Max Reichardt
 *
 * This class contains various information for the creation of ports.
 * If ports require parameters in their constructor, they should take an
 * instance of this class.
 * This way, only one constructor is needed per Port class.
 */
template <typename T>
class tPortCreationInfo : public tPortCreationInfoBase
{
  enum { boundable = typeutil::tIsCCType<T>::value && (!std::is_same<bool, T>::value) };

public:

  /*! Has default value been set? */
  bool default_value_set;

  /*! Have bounds for port been set? */
  bool bounds_set;

  /*! Has description for port been set? */
  bool description_set;

  tPortCreationInfo() :
      tPortCreationInfoBase(),
      default_value_set(false),
      bounds_set(false),
      description_set(false),
      storage(),
      unsigned_int_arg_count(0)
  {
  }

  /*!
   * Constructor takes variadic argument list... just any properties you want to assign to port.
   *
   * The first string is interpreted as port name, the second possibly as config entry (relevant for parameters only).
   * A framework element pointer is interpreted as parent.
   * unsigned int arguments are interpreted as flags.
   * int argument is interpreted as queue length.
   * tBounds<T> are port's bounds.
   * tUnit argument is port's unit.
   * int16/short argument is interpreted as minimum network update interval.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when port has numeric or string type.
   * There we have these rules:
   *
   * string type: The second string argument is interpreted as default_value. The third as config entry.
   * numeric type: The first numeric argument is interpreted as default_value. However, if it is the only unsigned int
   *               argument, it is interpreted as flags (because every port needs flags).
   */
  template <typename ARG1, typename ... TArgs>
  tPortCreationInfo(const ARG1& arg1, const TArgs&... rest) :
      tPortCreationInfoBase(),
      default_value_set(),
      bounds_set(false),
      description_set(false),
      storage(),
      unsigned_int_arg_count(CountUnsignedIntArgs(arg1, rest...))
  {
    ProcessFirstArg<ARG1>(arg1);
    ProcessArgs(rest...);
  }

  /*! Various set methods for different port properties */
  void Set(tFrameworkElement* parent)
  {
    this->parent = parent;
  }

  void Set(const char* c)
  {
    SetString(c);
  }

  void Set(int queue_size)
  {
    max_queue_size = queue_size;
    flags |= tPortFlags::cHAS_AND_USES_QUEUE;
  }

  void Set(uint flags)
  {
    this->flags |= flags;
  }

  void Set(short min_net_update_interval)
  {
    this->min_net_update_interval = min_net_update_interval;
  }

  void Set(tUnit* unit)
  {
    this->unit = unit;
  }

  void Set(const tUnit& unit)
  {
    this->unit = const_cast<tUnit*>(&unit);
  }

  void Set(const tLockOrder& lo)
  {
    this->lock_order = lo.wrapped;
  }

  template < bool DISABLE = (std::is_integral<T>::value && sizeof(T) <= 4) || internal::tIsString<T>::value >
  void Set(const typename std::enable_if < !DISABLE, T >::type& default_value)
  {
    SetDefault(default_value);
  }

  template < bool BOUNDABLE = boundable >
  void Set(const typename std::enable_if<BOUNDABLE, tBounds<T>>::type& bounds)
  {
    storage.bounds = bounds;
    bounds_set = true;
  }

  /*!
   * \return Bounds for port
   */
  template < bool BOUNDABLE = boundable >
  typename std::enable_if<BOUNDABLE, tBounds<T>>::type GetBounds()
  {
    return storage.bounds;
  }

  /*!
   * \return Pointer to default value
   */
  T* GetDefault()
  {
    if (!default_value_set)
    {
      FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_DEBUG_WARNING, "Default value was not set");
    }
    return storage.GetDefault();
  }

private:

  /*! Container for bounds in case of cc type */
  internal::tStorage<T, typeutil::tIsCCType<T>::value> storage;

  /*! number of unsigned int arguments */
  int unsigned_int_arg_count;

  /*! Process first constructor argument (tPortCreationInfo allowed) */
  template <typename A>
  void ProcessFirstArg(const typename std::enable_if<std::is_same<A, tPortCreationInfo>::value, A>::type& a)
  {
    *this = a;
  }

  template <typename A>
  void ProcessFirstArg(const typename std::enable_if < !std::is_same<A, tPortCreationInfo>::value, A >::type& a)
  {
    ProcessArg<A>(a);
  }

  /*! Process constructor arguments */
  void ProcessArgs() {}

  template <typename A, typename ... TRest>
  void ProcessArgs(const A& arg, const TRest&... args)
  {
    ProcessArg<A>(arg);
    ProcessArgs(args...);
  }

  /*! Process single argument */
  template <typename A>
  void ProcessArg(const typename std::enable_if < !(internal::tIsString<A>::value || (internal::tIsNumeric<T>::value && internal::tIsNumeric<A>::value)), A >::type& arg)
  {
    // standard case
    Set(arg);
  }

  template <typename A>
  void ProcessArg(const typename std::enable_if<internal::tIsString<A>::value, A>::type& arg)
  {
    // string argument, handling it here (no method overloading), produces some nicer compiler error messages
    SetString(arg);
  }

  template <typename A>
  void ProcessArg(const typename std::enable_if<internal::tIsNumeric<T>::value && internal::tIsNumeric<A>::value, A>::type& arg)
  {
    // numeric type and numeric argument => first numeric argument is default value (or possibly flag)
    if (!default_value_set)
    {
      if (std::is_same<A, uint>::value && unsigned_int_arg_count <= 1)
      {
        flags |= (uint)arg;
      }
      else
      {
        SetDefault((T)arg);
      }
    }
    else
    {
      Set(arg);
    }
  }

  // various helper methods
  template < bool STRING = internal::tIsString<T>::value >
  void SetString(const typename std::enable_if < !STRING, util::tString >::type& s)
  {
    if (!description_set)
    {
      description = s;
      description_set = true;
    }
    else
    {
      config_entry = s;
    }
  }

  template < bool STRING = internal::tIsString<T>::value >
  void SetString(const typename std::enable_if<STRING, util::tString>::type& s)
  {
    if (!description_set)
    {
      description = s;
      description_set = true;
    }
    else if (!default_value_set)
    {
      SetDefault(s);
    }
    else
    {
      config_entry = s;
    }
  }

  void SetDefault(const T& default_val)
  {
    if (default_value_set)
    {
      FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_DEBUG_WARNING, "Default value already set");
    }
    storage.SetDefault(default_val);
    default_value_set = true;
  }

  /*! Helper to determine number of unsigned int arguments */
  int CountUnsignedIntArgs()
  {
    return 0;
  }

  template <typename ARG1, typename ... TArgs>
  int CountUnsignedIntArgs(const ARG1& arg1, const TArgs&... rest)
  {
    int args = CountUnsignedIntArgs(rest...);
    if (std::is_same<ARG1, uint>::value)
    {
      args++;
    }
    return args;
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortCreationInfo_h__
