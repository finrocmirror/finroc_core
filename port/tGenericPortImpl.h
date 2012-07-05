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

#ifndef core__port__tGenericPortImpl_h__
#define core__port__tGenericPortImpl_h__

#include "core/port/tPortCreationInfoBase.h"
#include "core/port/tPortDataPtr.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Implementations for tGenericPort
 */
class tGenericPortImpl
{
  friend class tGenericPort;

  /*!
   * Creates port backend for specified port creation info
   *
   * \param pci Information for port creation
   */
  static tGenericPortImpl* CreatePortImpl(const tPortCreationInfoBase& pci);

  /*!
   * Gets Port's current value
   *
   * (Note that numbers and "cheap copy" types also have a method: T GetValue();  (defined in tPortParent<T>))
   *
   * \param result Buffer to (deep) copy port's current value to
   * (Using this get()-variant is more efficient when using CC types, but can be extremely costly with large data types)
   */
  virtual void Get(rrlib::rtti::tGenericObject& result, rrlib::time::tTimestamp& timestamp) = 0;

  /*!
   * \return Wrapped port.
   */
  virtual tAbstractPort* GetWrapped() = 0;

  /*!
   * Publish Data Buffer. This data will be forwarded to any connected ports.
   * Should only be called on output ports.
   *
   * \param data Data to publish. It will be deep-copied.
   * This publish()-variant is efficient when using CC types, but can be extremely costly with large data types)
   */
  virtual void Publish(const rrlib::rtti::tGenericObject& data, const rrlib::time::tTimestamp& timestamp) = 0;

  /*!
   * Set new bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param b New Bounds
   */
  virtual void SetBounds(const rrlib::rtti::tGenericObject& min, const rrlib::rtti::tGenericObject& max) = 0;

};

} // namespace finroc
} // namespace core

#endif
