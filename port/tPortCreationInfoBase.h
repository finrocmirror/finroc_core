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

#ifndef core__port__tPortCreationInfoBase_h__
#define core__port__tPortCreationInfoBase_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/rtti/rtti.h"
#include "rrlib/serialization/serialization.h"

#include "core/datatype/tBounds.h"
#include "core/datatype/tUnit.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
template <typename T>
class tPortCreationInfo;

/*! Can be used to wrap lock order for tPortCreationInfoBase variadic template constructor */
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
class tPortCreationInfoBase
{
protected: // Used by derived class tPortCreationInfo also

  /*!
   * Class to store default values and bounds of arbitrary types (in serialized form)
   */
  template <size_t INITIAL_SIZE>
  class tStorage : public rrlib::serialization::tStackMemoryBuffer<INITIAL_SIZE>
  {
  public:
    tStorage() :
      rrlib::serialization::tStackMemoryBuffer<INITIAL_SIZE>(5, true)
    {}

    tStorage(const tStorage& o)
    {
      CopyFrom(o);
    }

    tStorage& operator=(const tStorage& o)
    {
      CopyFrom(o);
      return *this;
    }
  };

  /*! Storage for default value */
  tStorage<150> default_value;

  /*! Storage for bounds */
  tStorage<300> bounds;

  /*! Has name been set? (we do not check name for zero length, because ports without names may be created) */
  bool name_set;

public:

  /*! number of send buffers */
  int send_buffer_size, alt_send_buffer_size;

  /*! SI Unit of port. NULL for no unit = provides raw numbers */
  tUnit* unit;

  /*! Input Queue size; value <= 0 means flexible size */
  int max_queue_size;

  /*! Port flags */
  unsigned int flags;

  /*! Minimum Network update interval; value < 0 => default values */
  int16 min_net_update_interval;

  /*! Data type of port */
  rrlib::rtti::tDataTypeBase data_type;

  /*! Parent of port */
  tFrameworkElement* parent;

  /*!
   * Only relevant for Port sets: Does Port set manage ports?
   * Is port responsible itself for invalidating outdated port values? (usually not the case... only with some PortSets)
   * (currently not in use)
   */
  bool manages_ports;

  /*! Port name */
  util::tString name;

  /*! Lock order level */
  int lock_order;

  /*! config entry in config file */
  util::tString config_entry;

  /*!
   * Creates port creation info with default values
   * (Typically, at least flags and name should be set to something sensible)
   */
  tPortCreationInfoBase();

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
   * tPortCreationBase argument is copied. This is only allowed as first argument.
   */
  template <typename ARG1, typename ... TArgs>
  explicit tPortCreationInfoBase(const ARG1& arg1, const TArgs&... rest) :
    default_value(),
    bounds(),
    name_set(false),
    send_buffer_size(-1),
    alt_send_buffer_size(-1),
    unit(&tUnit::cNO_UNIT),
    max_queue_size(16),
    flags(0),
    min_net_update_interval(-1),
    data_type(NULL),
    parent(NULL),
    manages_ports(false),
    name(""),
    lock_order(-1)
  {
    ProcessFirstArg<ARG1>(arg1);
    ProcessArgs(rest...);
  }

  /*!
   * \return Have bounds for port been set?
   */
  bool BoundsSet() const
  {
    return bounds.GetSize() > 0;
  }

  /*!
   * \return Has a default value been set?
   */
  bool DefaultValueSet() const
  {
    return default_value.GetSize() > 0;
  }

  /*!
   * Derive method: Copy port creation info and change specified parameters
   * (Basically, the same arguments as in the constructor are possible)
   */
  template <typename ... TArgs>
  tPortCreationInfoBase Derive(const TArgs&... rest) const
  {
    ProcessArgs(rest...);
    return *this;
  }

  /*!
   * \return Bounds (when their exact type is not known at compile time)
   */
  const rrlib::serialization::tConstSource& GetBoundsGeneric() const
  {
    return bounds;
  }

  /*!
   * \return Bounds (when their exact type is not known at compile time)
   */
  const rrlib::serialization::tConstSource& GetDefaultGeneric() const
  {
    return default_value;
  }

  /*!
   * \param flag Flag to query
   * \return Value of port flag
   */
  inline bool GetFlag(uint flag) const
  {
    return (flags & flag) > 0;
  }

  /*!
   * Copy port creation info and modify lock order level
   *
   * \param lock_order Lock order to use in copy
   */
  tPortCreationInfoBase LockOrderDerive(int lock_order) const;

  /*! Various set methods for different port properties */
  void Set(tFrameworkElement* parent)
  {
    this->parent = parent;
  }

  void Set(const char* c)
  {
    SetString(c);
  }

  void Set(const util::tString& s)
  {
    SetString(s);
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

  void Set(const rrlib::rtti::tDataTypeBase& dt)
  {
    this->data_type = dt;
  }

  /*!
   * Set bounds when type is not known at compile time
   *
   * \param min Minimum value
   * \param max Maximum value
   * \param adjust_to_range Adjust values lying outside to range (or rather discard them)?
   */
  void SetBoundsGeneric(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max, bool adjust_to_range = true)
  {
    rrlib::serialization::tOutputStream os(&bounds);
    // critical: needs to be the same serialization as used in tBounds
    os << min << max << (adjust_to_range ? tBounds<int>::eADJUST_TO_RANGE : tBounds<int>::eDISCARD);
  }

  void SetDefaultGeneric(const rrlib::rtti::tGenericObject& default_val)
  {
    rrlib::serialization::tOutputStream os(&default_value);
    os << default_val;
  }

  /*!
   * Set Port flag
   *
   * \param flag Flag to set
   * \param value Value to set flag to
   */
  void SetFlag(uint flag, bool value);

protected:

  /*!
   * Processes next string argument
   */
  void SetString(const util::tString& s);

private:

  /*!
   * Processes first argument (only here tPortCreationInfoBase argument is allowed)
   */
  template <typename A>
  void ProcessFirstArg(const typename std::enable_if<std::is_base_of<tPortCreationInfoBase, A>::value, A>::type& a)
  {
    static_cast<tPortCreationInfoBase&>(*this) = a;
  }

  template <typename A>
  void ProcessFirstArg(const typename std::enable_if < !std::is_base_of<tPortCreationInfoBase, A>::value, A >::type& a)
  {
    ProcessArg<A>(a);
  }

  /*! Process constructor arguments */
  void ProcessArgs() {}

  template <typename A, typename ... ARest>
  void ProcessArgs(const A& arg, const ARest&... args)
  {
    ProcessArg<A>(arg);
    ProcessArgs(args...);
  }

  /*! Process single constructor argument */
  template <typename A>
  void ProcessArg(const A& arg)
  {
    // standard case
    Set(arg);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortCreationInfoBase_h__
