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

#ifndef CORE__TRUNTIMELISTENER_H
#define CORE__TRUNTIMELISTENER_H

#include "finroc_core_utils/tListenerManager.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Classes implementing this interface can register at the runtime and will
 * be informed whenever an port is added or removed
 */
class tRuntimeListener : public util::tInterface
{
public:

  /*! Constants for Change type (element added, element changed, element removed, edges changed) */
  static const int8 cADD = 1, cCHANGE = 2, cREMOVE = 3, cEDGE_CHANGE = 4;

  /*! Call ID before framework element is initialized */
  static const int8 cPRE_INIT = -1;

  /*!
   * Called whenever a framework element was added/removed or changed
   *
   * \param change_type Type of change (see Constants)
   * \param element FrameworkElement that changed
   *
   * (Is called in synchronized (Runtime & Element) context in local runtime... so method should not block)
   */
  virtual void RuntimeChange(int8 change_type, tFrameworkElement* element) = 0;

};

/*!
 * Manager for port listeners
 */
class tRuntimeListenerManager : public util::tListenerManager<tFrameworkElement, util::tObject, tRuntimeListener, tRuntimeListenerManager>
{
public:

  tRuntimeListenerManager() {}

  inline void SingleNotify(tRuntimeListener* listener, tFrameworkElement* origin, const util::tObject* parameter, int call_id)
  {
    listener->RuntimeChange(static_cast<int8>(call_id), origin);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TRUNTIMELISTENER_H
