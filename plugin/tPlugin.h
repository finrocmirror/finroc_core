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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PLUGIN__TPLUGIN_H
#define CORE__PLUGIN__TPLUGIN_H

namespace finroc
{
namespace core
{
class tPluginManager;

/*!
 * \author Max Reichardt
 *
 * One class in a Plugin/Library must implement this interface.
 *
 * The plugin class needs to have an empty constructor.
 */
class tPlugin : public util::tInterface
{
public:

  /*!
   * This method is called once at initialization.
   * Data types, modules, widgets etc. can/should be added to the central
   * registry by calling the Plugin manager's approriate methods.
   *
   * \param mgr Plugin Manager
   */
  virtual void Init(tPluginManager& mgr) = 0;

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TPLUGIN_H
