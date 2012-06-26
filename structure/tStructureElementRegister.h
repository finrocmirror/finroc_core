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

//! Contains information about auto-generated port name of a single module type
struct tModulePorts
{
  /*! demangled RTTI name of module type (without template arguments) */
  util::tString name;

  /*! port name in module */
  std::vector<util::tString> ports;
};

//! Info on a single instantiated module */
struct tInstantiatedModule
{
  /*! Pointer to beginning of memory block */
  char* address;

  /*! Size of memory block */
  size_t size;

  /*! Pointer to module (usually == address, but not always) */
  tFrameworkElement* module;
};

//!
/*!
 * Internal helper class: Can be used to determine which is the parent module or group of a port
 */
class tStructureElementRegister
{
  /*!
   * Get Mutex for StructureElementRegister access
   */
  static util::tMutex& GetMutex();

  /*!
   * \return Register containing all instantiated elements
   */
  static std::vector<tInstantiatedModule>& GetRegister();

  /*!
   * \return Register containing port names for all module types with auto-generated port names
   */
  static std::vector<tModulePorts>& GetModuleTypeRegister();

public:

  /*!
   * Add memory block in which a tModuleBase or tGroup will be constructed
   * (should only be called by tModuleBase and tGroup)
   */
  static void AddMemoryBlock(void* address, size_t size);

  /*!
   * Add Module to register
   * (should only be called by tModuleBase and tGroup)
   */
  static void AddModule(tFrameworkElement* module);

  /*!
   * Add Module to register
   * (should only be called by tModuleBase and tGroup)
   */
  static void RemoveModule(tFrameworkElement* module);

  /*!
   * Add port names for a module type
   * (typically called by auto-generated code)
   */
  static void AddPortNamesForModuleType(const util::tString& name, const std::vector<util::tString>& names);

  /*!
   * \return Parent module/group of port class at address ptr
   */
  static tFrameworkElement* FindParent(void* ptr, bool abort_if_not_found = true);

  /*!
   * Retrieves auto-generated port name
   *
   * \param parent Parent Module of port
   * \param port_index Index of port
   * \param data_type Type of port (for consistency check)
   * \return auto-generated port name
   */
  static finroc::util::tString& GetAutoGeneratedPortName(tFrameworkElement* parent, size_t port_index);

  static inline const char* GetLogDescription()
  {
    return "tStructureElementRegister";
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
