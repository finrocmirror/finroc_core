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
/*!\file    core/port/tPortFactory.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tPortFactory
 *
 * \b tPortFactory
 *
 * Factory for ports when only the desired port data type is available.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tPortFactory_h__
#define __core__port__tPortFactory_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tAbstractPort.h"

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

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Port factory interface
/*!
 * Factory for ports when only the desired port data type is available.
 *
 * To create a factory for specific data types, derive from this class
 * and instantiate one object of this factory in a .cpp file.
 * Constructor registers factory at global port factory registry.
 *
 * To create a port, call static function CreatePort
 */
class tPortFactory : private rrlib::util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Constructor registers plugin at tPlugins
   */
  tPortFactory();

  /*!
   * External interface for anyone who wants to create a port of specified type.
   * For RPC types this will create routing ports only.
   *
   * Looks for a suitable factory among all available ones..
   * Returns NULL if there's no suitable factory.
   *
   * \param port_name Name of port
   * \param parent Parent of port
   * \param type Data type of port (should be the same as annotation comes from)
   * \param flags Port's flags
   * \return Created port or NULL if there's no suitable factory
   */
  static tAbstractPort* CreatePort(const std::string& port_name, tFrameworkElement& parent,
                                   const rrlib::rtti::tType& type, tFrameworkElement::tFlags flags);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Create port of specified type.
   * For RPC types this will create routing ports only.
   *
   * \param port_name Name of port
   * \param parent Parent of port
   * \param type Data type of port (should be the same as annotation comes from)
   * \param flags Port's flags
   * \return Created port
   */
  virtual tAbstractPort& CreatePortImplementation(const std::string& port_name, tFrameworkElement& parent,
      const rrlib::rtti::tType& type, tFrameworkElement::tFlags flags) = 0;

  /*!
   * \param type Data type for which a factory is needed
   * \return True, if this factory can create a port for the specified data type
   */
  virtual bool HandlesDataType(const rrlib::rtti::tType& type) = 0;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
