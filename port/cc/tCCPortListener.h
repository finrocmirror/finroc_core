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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__CC__TCCPORTLISTENER_H
#define CORE__PORT__CC__TCCPORTLISTENER_H

#include "finroc_core_utils/tListenerManager.h"

#include "core/port/cc/tCCPortData.h"

namespace finroc
{
namespace core
{
class tCCPortBase;

/*!
 * \author Max Reichardt
 *
 * Can register at cc port to receive callbacks whenever the port's value changes
 */
template < typename T = tCCPortData >
class tCCPortListener : public util::tInterface
{
public:

  /*!
   * Called whenever port's value has changed
   *
   * \param origin Port that value comes from
   * \param value Port's new value (locked for duration of method call)
   */
  virtual void PortChanged(tCCPortBase* origin, const T* value) = 0;

};

/*!
 * Manager for port listeners
 */
template < typename T = tCCPortData >
class tCCPortListenerManager : public util::tListenerManager<tCCPortBase, T, tCCPortListener<T>, tCCPortListenerManager<T> >
{
public:

  tCCPortListenerManager() {}

  inline void SingleNotify(tCCPortListener<T>* listener, tCCPortBase* origin, const T* parameter, int call_id)
  {
    listener->PortChanged(origin, parameter);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCPORTLISTENER_H
