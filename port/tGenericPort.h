/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
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

#ifndef core__port__tGenericPort_h__
#define core__port__tGenericPort_h__

#include "core/port/tPort.h"
#include "core/port/tGenericPortImpl.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Wrapper class for ports whose type is not known at compile time.
 *
 * (note: Get and Publish are currently only implemented based on deep-copying data.
 *  Therefore, Generic ports are only efficient for data types that can be copied cheaply)
 */
class tGenericPort : public tPortWrapperBase
{
  std::shared_ptr<tGenericPortImpl> impl;

public:

  /*!
   * Empty constructor in case port is created later
   */
  tGenericPort() :
    impl()
  {}

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
  template <typename ... ARGS>
  tGenericPort(const ARGS&... args)
  {
    tPortCreationInfoBase pci(args...);
    impl.reset(tGenericPortImpl::CreatePortImpl(pci));
    wrapped = impl->GetWrapped();
  }

  /*!
   * Gets Port's current value
   *
   * (Note that numbers and "cheap copy" types also have a method: T GetValue();  (defined in tPortParent<T>))
   *
   * \param result Buffer to (deep) copy port's current value to
   * (Using this get()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   */
  inline const void Get(rrlib::rtti::tGenericObject& result)
  {
    impl->Get(result);
  }

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * Should only be called on output ports.
   *
   * \param data Data to publish. It will be deep-copied.
   * This publish()-variant is efficient when using CC types, but can be extremely costly with large data types)
   */
  inline void Publish(const rrlib::rtti::tGenericObject& data)
  {
    impl->Publish(data);
  }

  /*!
   * Set new bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param b New Bounds
   */
  inline void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max)
  {
    impl->SetBounds(min, max);
  }
};

} // namespace finroc
} // namespace core

#endif
