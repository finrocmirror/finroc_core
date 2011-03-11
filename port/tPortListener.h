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

#ifndef core__port__tPortListener_h__
#define core__port__tPortListener_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortListenerAdapter.h"

namespace finroc
{
namespace core
{
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Can register at port to receive callbacks whenever the port's value changes
 */
template < typename T = void* >
class tPortListener : public tPortListenerAdapter < T, typeutil::tIsCCType<T>::value, boost::is_integral<T>::value || boost::is_floating_point<T>::value >
{
public:

  /*!
   * Called whenever port's value has changed
   *
   * \param origin Port that value comes from
   * \param value Port's new value (locked for duration of method call)
   */
  virtual void PortChanged(tAbstractPort* origin, const T& value) = 0;

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortListener_h__