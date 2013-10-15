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
/*!\file    core/internal/tPlugins.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tPlugins
 *
 * \b tPlugins
 *
 * This class is used for managing the Finroc Runtime's plugins
 *
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tPlugins_h__
#define __core__internal__tPlugins_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <vector>
#include "rrlib/util/tNoncopyable.h"

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
class tPlugin;

namespace internal
{

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Plugin management
/*!
 * This class is used for managing the Finroc Runtime's plugins
 */
class tPlugins : private rrlib::util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * (possibly manually) add plugin
   *
   * \param p Plugin to add
   */
  void AddPlugin(tPlugin& p);

  /*!
   * \return Plugins singleton instance
   */
  static tPlugins& GetInstance();

  /*!
   * Initializes any plugins that have been registered since last initialization
   */
  void InitializeNewPlugins();

  /*!
   * Loads plugins
   */
  static void StaticInit();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! All Plugins that are currently available */
  std::vector<tPlugin*> plugins;

  /*! Instantly initialize plugins when they are added? True after StaticInit() has been called */
  bool instantly_initialize_plugins;

  /*! Number of plugins that were initialized */
  size_t initialized_plugin_count;


  tPlugins();

  void FindAndLoadPlugins();

  inline static const char* GetLogDescription()
  {
    return "Plugins";
  }

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
