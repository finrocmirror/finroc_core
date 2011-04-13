//
// You received this file as part of Finroc
// A framework for innovative robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    tStructureElementRegister.h
 *
 * \author  Max Reichardt
 *
 * \date    2011-04-13
 *
 * \brief Contains tStructureElementRegister
 *
 * \b tStructureElementRegister
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tStructureElementRegister_h_
#define _core__structure__tStructureElementRegister_h_

#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tPort.h"
#include "core/plugin/tStandardCreateModuleAction.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace structure
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//!
/*!
 * Internal helper class: Can be used to determine which is the parent module or group of a port
 */
class tStructureElementRegister : public finroc::core::tFrameworkElement
{
  friend class tGroup;
  friend class tModuleBase;

  /*!
   * \return Register containing all instantiated elements
   */
  static std::vector<tFrameworkElement*>& GetRegister();

public:

  /**
   * \return Parent module/group of port class at address ptr
   */
  static tFrameworkElement* FindParent(void* ptr);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
