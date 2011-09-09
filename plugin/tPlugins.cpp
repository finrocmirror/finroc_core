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
#include "core/tRuntimeEnvironment.h"

#include "core/plugin/tPlugins.h"
#include "core/plugin/tCreateExternalConnectionAction.h"

namespace finroc
{
namespace core
{

void tPlugins::FindAndLoadPlugins()
{
  //TODO do properly
  //TODO do properly
  //TODO do properly
  //TODO do properly
  //TODO do properly
  //JavaPlugins.loadAllDataTypesInPackage(BehaviourInfo.class);

  // plugins.Add(new tMCA2Plugin());
}

tCreateFrameworkElementAction* tPlugins::LoadModuleType(const util::tString& group, const util::tString& name)
{
  // dynamically loaded .so files
  static util::tSimpleList<util::tString> loaded;

  // try to find module among existing modules
  const util::tSimpleList<tCreateFrameworkElementAction*>& modules = GetModuleTypes();
  for (size_t i = 0u; i < modules.Size(); i++)
  {
    tCreateFrameworkElementAction* cma = modules.Get(i);
    if (cma->GetModuleGroup().Equals(group) && cma->GetName().Equals(name))
    {
      return cma;
    }
  }

  // hmm... we didn't find it - have we already tried to load .so?
  bool already_loaded = false;
  for (size_t i = 0; i < loaded.Size(); i++)
  {
    if (loaded.Get(i).Equals(group))
    {
      already_loaded = true;
      break;
    }
  }

  if (!already_loaded)
  {
    loaded.Add(group);
    std::set<std::string> loaded = sDynamicLoading::GetLoadedFinrocLibraries();
    if (loaded.find(group) == loaded.end() && sDynamicLoading::DLOpen(group.GetCString()))
    {
      return LoadModuleType(group, name);
    }
  }

  FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, "Could not find/load module ", name, " in ", group);
  return NULL;
}

tCreateExternalConnectionAction* tPlugins::RegisterExternalConnection(tCreateExternalConnectionAction* action)
{
  external_connections.Add(action);
  GetModuleTypes().Add(action);
  return action;
}

} // namespace finroc
} // namespace core

