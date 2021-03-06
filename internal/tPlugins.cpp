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
/*!\file    core/internal/tPlugins.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tPlugins.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tPlugin.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tPlugins::tPlugins() :
  plugins(),
  instantly_initialize_plugins(false),
  initialized_plugin_count(0)
{}

void tPlugins::AddPlugin(tPlugin& p)
{
  plugins.push_back(&p);

  // This does not work - because plugin is not fully constructed yet
  /*if (instantly_initialize_plugins)
  {
    p->Init();
  }*/
}

tPlugins& tPlugins::GetInstance()
{
  static tPlugins instance;
  return instance;
}

void tPlugins::InitializeNewPlugins()
{
  if (instantly_initialize_plugins)
  {
    for (size_t i = initialized_plugin_count; i < plugins.size(); i++)
    {
      plugins[i]->Init();
    }
    initialized_plugin_count = plugins.size();
  }
}

bool tPlugins::IsPluginLoaded(const std::string& plugin_name)
{
  for (tPlugin * plugin : plugins)
  {
    if (plugin_name == plugin->GetName())
    {
      return true;
    }
  }
  return false;
}

void tPlugins::StaticInit()
{
  tPlugins& p = GetInstance();
  for (size_t i = 0; i < p.plugins.size(); i++)
  {
    p.plugins[i]->Init();
  }
  p.instantly_initialize_plugins = true;
  p.initialized_plugin_count = p.plugins.size();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
