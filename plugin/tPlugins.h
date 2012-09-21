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

#ifndef core__plugin__tPlugins_h__
#define core__plugin__tPlugins_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
class tPlugin;
class tCreateExternalConnectionAction;

/*!
 * \author Max Reichardt
 *
 * This class is used for managing the Runtime's plugins
 */
class tPlugins : public util::tUncopyableObject
{
private:

  /*! All Plugins that are currently available */
  std::vector<tPlugin*> plugins;

  /*! Instantly initialize plugins when they are added? True after StaticInit() has been called */
  bool instantly_initialize_plugins;

  tPlugins();

  void FindAndLoadPlugins();

  inline static const char* GetLogDescription()
  {
    return "Plugins";
  }

public:

  /*!
   * (possibly manually) add plugin
   *
   * \param p Plugin to add
   */
  void AddPlugin(tPlugin* p);

  /*!
   * \return Plugins singleton instance
   */
  static tPlugins* GetInstance();

  /*!
   * Loads plugins
   */
  static void StaticInit();

};

} // namespace finroc
} // namespace core

#endif // core__plugin__tPlugins_h__
