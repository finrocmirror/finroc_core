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
#include "core/plugin/sDynamicLoading.h"
#include "core/tRuntimeEnvironment.h"
#include <fstream>
#include <dlfcn.h>
#include <boost/filesystem.hpp>

namespace finroc
{
namespace core
{
sDynamicLoading::tDLCloser::~tDLCloser()
{
  tRuntimeEnvironment::Shutdown();
  for (size_t i = 0; i < loaded.size(); i++)
  {
    dlclose(loaded[i]);
  }
}

bool sDynamicLoading::DLOpen(const char* open)
{
  static tDLCloser dlcloser;

  void* handle = dlopen(open, RTLD_NOW | RTLD_GLOBAL);
  if (handle)
  {
    dlcloser.loaded.push_back(handle);
    return true;
  }
  FINROC_LOG_PRINTF(rrlib::logging::eLL_ERROR, "Error from dlopen: %s", dlerror());
  return false;
}

std::set<std::string> sDynamicLoading::GetAvailableFinrocLibraries()
{
  // this implementation searches in path of libfinroc_core.so and in path <$FINROC_HOME>/export/<$TARGET>/lib
  std::vector<std::string> paths;
  std::string core_path(GetBinary((void*)GetBinary));
  core_path = core_path.substr(0, core_path.rfind("/"));
  paths.push_back(core_path);

  char* finroc_home = getenv("FINROC_HOME");
  char* target = getenv("FINROC_TARGET");
  if (finroc_home == NULL || target == NULL)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "FINROC_HOME/FINROC_TARGET not set.");
  }
  else
  {
    std::string local_path = std::string(finroc_home) + "/export/" + std::string(target) + "/lib";
    if (local_path.compare(core_path) != 0)
    {
      FINROC_LOG_PRINTF(rrlib::logging::eLL_DEBUG, "Searching for finroc modules in %s and %s.\n", core_path.c_str(), local_path.c_str());
      paths.push_back(local_path);
    }
  }

  std::set<std::string> result;
  for (size_t i = 0; i < paths.size(); i++)
  {
    std::string path = paths[i];
    if (boost::filesystem::exists(path))
    {
      boost::filesystem::directory_iterator end;
      for (boost::filesystem::directory_iterator it(path); it != end; ++it)
      {
        if (!boost::filesystem::is_directory(it->status()))
        {
          std::string file(it->path().filename().c_str());
          if ((file.substr(0, 10).compare("libfinroc_") == 0 || file.substr(0, 9).compare("librrlib_") == 0) && file.substr(file.length() - 3, 3).compare(".so") == 0)
          {
            result.insert(file);
          }
        }
      }
    }
  }
  return result;
}


std::string sDynamicLoading::GetBinary(void* addr)
{
  Dl_info info;
  dladdr(addr, &info);
  return info.dli_fname;
}


std::set<std::string> sDynamicLoading::GetLoadedFinrocLibraries()
{
  // this implementation looks in /proc/<pid>/maps for loaded .so files

  // get process id
  __pid_t pid = getpid();

  // scan for loaded .so files
  std::stringstream mapsfile;
  mapsfile << "/proc/" << pid << "/maps";
  std::set<std::string> result;
  std::ifstream maps(mapsfile.str().c_str());
  std::string line;
  while (!maps.eof())
  {
    std::getline(maps, line);
    if (line.find("/libfinroc_") != std::string::npos && line.substr(line.length() - 3, 3).compare(".so") == 0)
    {
      std::string loaded = line.substr(line.find("/libfinroc_") + 1);
      if (result.find(loaded) == result.end())
      {
        FINROC_LOG_PRINTF(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Found loaded finroc library: %s", loaded.c_str());
        result.insert(loaded);
      }
    }
    else if (line.find("/librrlib_") != std::string::npos && line.substr(line.length() - 3, 3).compare(".so") == 0)
    {
      std::string loaded = line.substr(line.find("/librrlib_") + 1);
      if (result.find(loaded) == result.end())
      {
        FINROC_LOG_PRINTF(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Found loaded finroc library: %s", loaded.c_str());
        result.insert(loaded);
      }
    }
  }
  maps.close();
  return result;
}

std::vector<std::string> sDynamicLoading::GetLoadableFinrocLibraries()
{
  std::vector<std::string> result;
  std::set<std::string> available = GetAvailableFinrocLibraries();
  std::set<std::string> loaded = GetLoadedFinrocLibraries();
  std::set<std::string>::iterator it;
  for (it = available.begin(); it != available.end(); it++)
  {
    if (loaded.find(*it) == loaded.end())
    {
      result.push_back(*it);
    }
  }
  return result;
}

} // namespace finroc
} // namespace core
