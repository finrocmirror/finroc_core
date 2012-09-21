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

#include "core/plugin/tPlugins.h"
#include "core/plugin/tPlugin.h"

namespace finroc
{
namespace core
{

tPlugins::tPlugins() :
  plugins(),
  instantly_initialize_plugins(false)
{}

void tPlugins::AddPlugin(tPlugin* p)
{
  plugins.push_back(p);

  // TODO: This does not work - because plugin is not fully constructed yet
  /*if (instantly_initialize_plugins)
  {
    p->Init();
  }*/
}

void tPlugins::FindAndLoadPlugins()
{
  //TODO implement: dynamic loading of all available finroc plugin .so files
}

tPlugins* tPlugins::GetInstance()
{
  static tPlugins instance;
  return &instance;
}

void tPlugins::StaticInit()
{
  tPlugins* p = GetInstance();
  p->FindAndLoadPlugins();
  for (size_t i = 0; i < p->plugins.size(); i++)
  {
    p->plugins[i]->Init();
  }
  p->instantly_initialize_plugins = true;
}

} // namespace finroc
} // namespace core

