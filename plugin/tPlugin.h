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

#ifndef core__plugin__tPlugin_h__
#define core__plugin__tPlugin_h__

#include "core/plugin/tPlugins.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * One class in a Plugin/Library must implement this interface.
 * It should be instantiated in a .cpp file.
 */
class tPlugin : public boost::noncopyable
{
public:

  /*!
   * Constructor registers plugin at tPlugins
   */
  tPlugin()
  {
    tPlugins::GetInstance()->AddPlugin(this);
  }

  /*!
   * This method is called once at initialization.
   * Global parameters, data types, module types etc. may be added here.
   */
  virtual void Init() = 0;

};

} // namespace finroc
} // namespace core

#endif // core__plugin__tPlugin_h__
