//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
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
/*!\file    core/tRuntimeSettings.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tRuntimeSettings
 *
 * \b tRuntimeSettings
 *
 * Contains modifiable global settings of runtime environment.
 * Plugins should add global parameters accessible via ports to this tRuntimeSettings framework element.
 *
 * StaticInit() should be called after runtime and data types have been initialized.
 */
//----------------------------------------------------------------------
#ifndef __core__tRuntimeSettings_h__
#define __core__tRuntimeSettings_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

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
//! Modifiable runtime settings
/*!
 * Contains modifiable global settings of runtime environment.
 * Plugins should add global parameters accessible via ports to this tRuntimeSettings framework element.
 *
 * StaticInit() should be called after runtime and data types have been initialized.
 */
class tRuntimeSettings : public tFrameworkElement
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Allow creation of framework elements with the same qualified names.
   * Activating this is somewhat dangerous. Only do this if you really have to!
   * If ports have the same qualified names, connecting to them by-string causes undefined behaviour.
   */
  static void AllowDuplicateQualifiedNames()
  {
    duplicate_qualified_names_allowed = true;
  }

  /*!
   * \return Is creation of framework elements with the same qualified names allowed?
   */
  static bool DuplicateQualifiedNamesAllowed()
  {
    return duplicate_qualified_names_allowed;
  }

  /*!
   * \return Singleton instance
   */
  static tRuntimeSettings& GetInstance();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tRuntimeEnvironment;

  tRuntimeSettings();

  /*! Singleton Instance */
  static tRuntimeSettings* instance;

  /*!
   * Is creation of framework elements with the same qualified names allowed?
   * (by default it is not, because this causes undefined behaviour with port connections by-string
   *  when ports have the same names (e.g. in fingui and in finstructable groups)
   */
  static bool duplicate_qualified_names_allowed;

  /*! Completes initialization */
  static void StaticInit();
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
