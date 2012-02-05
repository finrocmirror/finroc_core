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

#ifndef core__port__rpc__tInterfaceServerPort_h__
#define core__port__rpc__tInterfaceServerPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tInterfacePort.h"


namespace finroc
{
namespace core
{
class tAbstractMethodCallHandler;

/*! Base class for server implementation of interface */
class tInterfaceServerPort : public tInterfacePort
{
private:

  /*! Handler that will handle method calls */
  tAbstractMethodCallHandler* handler;

protected:

  inline void SetCallHandler(tAbstractMethodCallHandler* handler_)
  {
    this->handler = handler_;
  }

public:

  tInterfaceServerPort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& type) :
    tInterfacePort(name, parent, type, ::finroc::core::tInterfacePort::eServer),
    handler(NULL)
  {
  }

  tInterfaceServerPort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& type, tAbstractMethodCallHandler* ch, uint custom_flags = 0) :
    tInterfacePort(name, parent, type, ::finroc::core::tInterfacePort::eServer, custom_flags),
    handler(NULL)
  {
    SetCallHandler(ch);
  }

  tInterfaceServerPort(const util::tString& name, tFrameworkElement* parent, const rrlib::rtti::tDataTypeBase& type, tAbstractMethodCallHandler* ch, uint custom_flags, int lock_level) :
    tInterfacePort(name, parent, type, ::finroc::core::tInterfacePort::eServer, custom_flags, lock_level),
    handler(NULL)
  {
    SetCallHandler(ch);
  }

  /*!
   * Get buffer to use in method return (has one lock)
   *
   * (for non-cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  template <typename T>
  inline tPortDataPtr<T> GetBufferForReturn(const rrlib::rtti::tDataTypeBase& dt = NULL)
  {
    return GetBufferForCall<T>(dt);
  }

  /*!
   * \return Handler that will handle method calls
   */
  inline tAbstractMethodCallHandler* GetHandler()
  {
    return handler;
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tInterfaceServerPort_h__
