//
// You received this file as part of Finroc
// A framework for integrated robot control
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
/*!\file    default_main_wrapper.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 */
//----------------------------------------------------------------------
#include "core/default_main_wrapper.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <cstdlib>

extern "C"
{
#include <libgen.h>
}
//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/logging/definitions.h"

#include "core/tRuntimeEnvironment.h"
#include "plugins/tcp/tTCPServer.h"

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

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

bool LogConfigHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption log_config(name_to_option_map.at("log-config"));
  if (log_config->IsActive())
  {
    rrlib::logging::tLogDomainRegistry::GetInstance()->ConfigureFromFile(boost::any_cast<const char *>(log_config->GetValue()));
  }

  return true;
}


rrlib::getopt::tOption& ParameterConfig()
{
  static rrlib::getopt::tOption parameter_config;
  return parameter_config;
}

bool ParameterConfigHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption parameter_config(name_to_option_map.at("config_file"));
  if (parameter_config->IsActive())
  {
    ParameterConfig() = parameter_config;
  }
  return true;
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
  rrlib::logging::default_log_description = basename(argv[0]);

  rrlib::getopt::SetProgramVersion(cPROGRAM_VERSION);
  rrlib::getopt::SetProgramDescription(cPROGRAM_DESCRIPTION);

  rrlib::getopt::AddValue("log-config", 'l', "Log config file", &LogConfigHandler);

  rrlib::getopt::AddValue("config_file", 'c', "Parameter config file", &ParameterConfigHandler);

  StartUp();

  rrlib::getopt::ProcessCommandLine(argc, argv);

  finroc::core::tRuntimeEnvironment *runtime_environment = finroc::core::tRuntimeEnvironment::GetInstance();
  finroc::tcp::tTCPServer *server = new finroc::tcp::tTCPServer(4444, true, NULL);
  server->Init();

  finroc::core::tThreadContainer *main_thread = new finroc::core::tThreadContainer(runtime_environment, "Main Thread");

  InitMainGroup(main_thread, ParameterConfig());

  main_thread->Init();
  main_thread->StartExecution();

  while (true)
  {
    try
    {
      finroc::util::tThread::Sleep(10000);
    }
    catch (const finroc::util::tInterruptedException& e)
    {
    }
  }
  //  main_thread->JoinThread();

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
