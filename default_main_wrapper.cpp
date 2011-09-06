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
#include <csignal>

extern "C"
{
#include <libgen.h>
}
//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/logging/definitions.h"

#include "core/tRuntimeEnvironment.h"
#include "core/parameter/tConfigFile.h"
#include "core/thread/tExecutionControl.h"
#include "rrlib/finroc_core_utils/sFiles.h"
#include "plugins/tcp/tTCPServer.h"
#include "plugins/tcp/tTCPPeer.h"

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

bool run_main_loop = false;
bool pause_at_startup = false;

//----------------------------------------------------------------------
// HandleSignalSIGINT
//----------------------------------------------------------------------
void HandleSignalSIGINT(int signal)
{
  assert(signal == SIGINT);
  FINROC_LOG_PRINT(rrlib::logging::eLL_USER, "\nCaught SIGINT. Exiting...");
  run_main_loop = false;
}

//----------------------------------------------------------------------
// InstallSignalHandler
//----------------------------------------------------------------------
bool InstallSignalHandler()
{
  struct sigaction signal_action;
  signal_action.sa_handler = HandleSignalSIGINT;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;

  if (sigaction(SIGINT, &signal_action, NULL) != 0)
  {
    perror("Could not install signal handler");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
// LogConfigHandler
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

//----------------------------------------------------------------------
// ParameterConfigHandler
//----------------------------------------------------------------------
bool ParameterConfigHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption parameter_config(name_to_option_map.at("config-file"));
  if (parameter_config->IsActive())
  {
    const char* file = boost::any_cast<const char *>(parameter_config->GetValue());
    if (!finroc::util::sFiles::Exists(file))
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Could not find specified config file ", file);
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Loading config file ", file);
    }
    finroc::core::tRuntimeEnvironment::GetInstance()->AddAnnotation(new finroc::core::tConfigFile(file));
  }
  return true;
}

//----------------------------------------------------------------------
// PauseHandler
//----------------------------------------------------------------------
bool PauseHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption pause(name_to_option_map.at("pause"));
  pause_at_startup = pause->IsActive();
  return true;
}


//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
  if (!InstallSignalHandler())
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Error installing signal handler. Exiting...");
    return EXIT_FAILURE;
  }

  rrlib::logging::default_log_description = basename(argv[0]);

  rrlib::getopt::SetProgramVersion(cPROGRAM_VERSION);
  rrlib::getopt::SetProgramDescription(cPROGRAM_DESCRIPTION);

  rrlib::getopt::AddValue("log-config", 'l', "Log config file", &LogConfigHandler);
  rrlib::getopt::AddValue("config-file", 'c', "Parameter config file", &ParameterConfigHandler);
  rrlib::getopt::AddFlag("pause", 'p', "Pause program at startup", &PauseHandler);

  StartUp();

  std::vector<char *> remaining_args = rrlib::getopt::ProcessCommandLine(argc, argv);

  finroc::core::tRuntimeEnvironment *runtime_environment = finroc::core::tRuntimeEnvironment::GetInstance();

  // Create and connect TCP peer
  finroc::tcp::tTCPPeer* tcp_peer = new finroc::tcp::tTCPPeer(finroc::util::tStringBuilder("localhost:") + 4444, "", finroc::tcp::tTCPPeer::eFULL, 4444, finroc::tcp::tTCPPeer::cDEFAULT_FILTER, true);
  tcp_peer->Init();
  try
  {
    tcp_peer->Connect();
  }
  catch (const finroc::util::tException& e1)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Error connecting Peer", e1);
  }

  finroc::core::tThreadContainer *main_thread = new finroc::core::tThreadContainer(runtime_environment, "Main Thread");

  InitMainGroup(main_thread, remaining_args);

  main_thread->Init();
  if (pause_at_startup)
  {
    finroc::core::tExecutionControl::PauseAll(main_thread); // Shouldn't be necessary, but who knows what people might implement
  }
  else
  {
    finroc::core::tExecutionControl::StartAll(main_thread);
  }

  run_main_loop = true;
  while (run_main_loop)
  {
    try
    {
      finroc::util::tThread::Sleep(10000);
    }
    catch (const finroc::util::tInterruptedException& e)
    {
    }
  }
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Left main loop");

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
