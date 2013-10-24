//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/tRuntimeListener.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tRuntimeListener
 *
 * \b tRuntimeListener
 *
 * Classes implementing this interface can register at the runtime and will
 * be informed whenever an port is added or removed
 *
 */
//----------------------------------------------------------------------
#ifndef __core__tRuntimeListener_h__
#define __core__tRuntimeListener_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
class tFrameworkElement;
class tAbstractPort;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Runtime Listener
/*!
 * Classes implementing this interface can register at the runtime and will
 * be informed whenever an port is added or removed
 */
class tRuntimeListener
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! Constants for Change type */
  enum tEvent
  {
    ADD,     //!< element added
    CHANGE,  //!< element changed
    REMOVE,  //!< element removed
    PRE_INIT //!< called with this constant before framework element is initialized
  };

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tRuntimeEnvironment;

  /*!
   * Called whenever a framework element was added, removed or changed
   *
   * \param change_type Type of change
   * \param element FrameworkElement that changed
   *
   * (Is called in synchronized (Runtime & Element) context in local runtime... so method should not block)
   */
  virtual void OnFrameworkElementChange(tEvent change_type, tFrameworkElement& element) = 0;

  /*!
   * Called whenever an edge was added or removed
   *
   * \param change_type Type of change
   * \param source Source of edge
   * \param target Target of edge
   *
   * (Is called in synchronized (Runtime & Element) context in local runtime... so method should not block)
   */
  virtual void OnEdgeChange(tEvent change_type, tAbstractPort& source, tAbstractPort& target) = 0;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
