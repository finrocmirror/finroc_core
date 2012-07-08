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
 * \author  Max Reichardt
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
#include <boost/lexical_cast.hpp>

extern "C"
{
#include <libgen.h>
}

#include "rrlib/logging/configuration.h"
#include "rrlib/finroc_core_utils/log/log_definitions.h"
#include "rrlib/finroc_core_utils/sFiles.h"
#include "rrlib/finroc_core_utils/crash_handler.h"

#include "plugins/tcp/tTCPServer.h"
#include "plugins/tcp/tTCPPeer.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

#include "core/tRuntimeEnvironment.h"
#include "core/parameter/tConfigFile.h"
#include "core/thread/tExecutionControl.h"

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

int finroc_argc_copy;     // copy of argc for 'finroc' part. TODO: remove when rrlib_getopt supports prioritized evaluation of -m option
char ** finroc_argv_copy; // copy of argv for 'finroc' part. TODO: remove when rrlib_getopt supports prioritized evaluation of -m option

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

static bool run_main_loop = false;
static bool pause_at_startup = false;
static int network_port = 4444;
bool links_are_unique = true;
static finroc::util::tString connect_to;
#ifdef NDEBUG
static bool enable_crash_handler = false;
#else
static bool enable_crash_handler = true;
#endif

// We do not use stuff from rrlib_thread, because we have the rare case that in signal handler
// waiting thread does something else, which is problematic with respect to enforcing lock order
static std::mutex main_thread_wait_mutex;
static std::condition_variable main_thread_wait_variable;

//----------------------------------------------------------------------
// HandleSignalSIGINT
//----------------------------------------------------------------------
void HandleSignalSIGINT(int signal)
{
  static int call_count = 0; // How many time has function been called?
  assert(signal == SIGINT);
  call_count++;
  if (call_count == 1)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_USER, "\nCaught SIGINT. Exiting...");
    run_main_loop = false;
    std::unique_lock<std::mutex> l(main_thread_wait_mutex);
    main_thread_wait_variable.notify_all();
  }
  else if (call_count < 5)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_USER, "\nCaught SIGINT again. Unfortunately, the program still hasn't terminated. Program will be aborted at fifth SIGINT.");
  }
  else
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_USER, "\nCaught SIGINT for the fifth time. Aborting program.");
    abort();
  }
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
    rrlib::logging::ConfigureFromFile(boost::any_cast<const char *>(log_config->GetValue()));
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
    if (!finroc::util::sFiles::FinrocFileExists(file))
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
// PortHandler
//----------------------------------------------------------------------
bool PortHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption port_option(name_to_option_map.at("port"));
  if (port_option->IsActive())
  {
    const char* port_string = boost::any_cast<const char *>(port_option->GetValue());
    int port = atoi(port_string);
    if (port < 1 || port > 65535)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Invalid port '", port_string, "'. Using default: ", network_port);
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Listening on user defined port ", port, ".");
      network_port = port;
    }
  }

  return true;
}

//----------------------------------------------------------------------
// UniqueHandler
//----------------------------------------------------------------------
bool UniqueHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption opt(name_to_option_map.at("port-links-are-not-unique"));
  links_are_unique = !opt->IsActive();
  return true;
}

//----------------------------------------------------------------------
// ConnectHandler
//----------------------------------------------------------------------
bool ConnectHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption connect_option(name_to_option_map.at("connect"));
  if (connect_option->IsActive())
  {
    connect_to = boost::any_cast<const char *>(connect_option->GetValue());
    FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Connecting to ", connect_to);
  }

  return true;
}

bool MaxPortsHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption max_ports(name_to_option_map.at("max-ports"));
  if (max_ports->IsActive())
  {
    const char* max_string = boost::any_cast<const char *>(max_ports->GetValue());
    int m = atoi(max_string);
    if (m < 2 || m > 0xFFFFFF)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Invalid value for maximum number of ports: ", max_string, ". Please provide a value between 2 and 16.7 million");
    }
    else
    {
      finroc::core::tCoreRegister<finroc::core::tAbstractPort*>::SetMaximumNumberOfElements(m);
    }
  }
  return true;
}

bool MaxElementsHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption max_elements(name_to_option_map.at("max-elements"));
  if (max_elements->IsActive())
  {
    const char* max_string = boost::any_cast<const char *>(max_elements->GetValue());
    int m = atoi(max_string);
    if (m < 2 || m > 0xFFFFFF)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Invalid value for maximum number of framework elements: ", max_string, ". Please provide a value between 2 and 16.7 million");
    }
    else
    {
      finroc::core::tCoreRegister<finroc::core::tFrameworkElement*>::SetMaximumNumberOfElements(m);
    }
  }
  return true;
}

//----------------------------------------------------------------------
// CrashHandler
//----------------------------------------------------------------------
bool CrashHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption crash_config(name_to_option_map.at("crash-handler"));
  if (crash_config->IsActive())
  {
    std::string s(boost::any_cast<const char*>(crash_config->GetValue()));
    if (s.compare("on") == 0)
    {
      enable_crash_handler = true;
    }
    else if (s.compare("off") == 0)
    {
      enable_crash_handler = false;
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Option --crash-handler needs be either 'on' or 'off' (not '", s, "'). Using default.");
    }
  }

  return true;
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{

  // TODO: remove when rrlib_getopt supports prioritized evaluation of -m option
  finroc_argc_copy = argc;
  finroc_argv_copy = argv;

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
  rrlib::getopt::AddValue("port", 'p', "Network port to use", &PortHandler);
  rrlib::getopt::AddValue("connect", 0, "TCP address of finroc application to connect to (default: localhost:<port>)", &ConnectHandler);
  rrlib::getopt::AddValue("max-ports", 0, "Maximum number of ports (default: 65535). Has significant impact on memory footprint.", &MaxPortsHandler);
  rrlib::getopt::AddValue("max-elements", 0, "Maximum number of framework elements excluding ports (default: 65535).", &MaxElementsHandler);
  rrlib::getopt::AddValue("crash-handler", 0, "Enable/disable crash handler (default: 'on' in debug mode - 'off' in release mode).", &CrashHandler);
  rrlib::getopt::AddFlag("pause", 0, "Pause program at startup", &PauseHandler);
  rrlib::getopt::AddFlag("port-links-are-not-unique", 0, "Port links in this part are not unique in P2P network (=> host name is prepended in GUI, for instance).", &UniqueHandler);

  StartUp();

  std::vector<char *> remaining_args = rrlib::getopt::ProcessCommandLine(argc, argv);

  if (enable_crash_handler)
  {
    if (!finroc::util::InstallCrashHandler())
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Error installing crash handler. Crashes will simply terminate the program.");
    }
  }

  finroc::core::tRuntimeEnvironment *runtime_environment = finroc::core::tRuntimeEnvironment::GetInstance();

  // Have any top-level framework elements containing threads already been created?
  // In this case, we won't create an extra thread container (finstructed part does not need one for example)
  std::vector<finroc::core::tFrameworkElement*> executables;
  finroc::core::tFrameworkElement::tChildIterator ci(*runtime_environment, false);
  finroc::core::tFrameworkElement* next = NULL;
  while ((next = ci.Next()) != NULL)
  {
    if (next->GetAnnotation<finroc::core::tExecutionControl>() != NULL && (next->GetFlag(finroc::core::tCoreFlags::cFINSTRUCTABLE_GROUP) || next->GetFlag(finroc::core::tCoreFlags::cEDGE_AGGREGATOR)))
    {
      executables.push_back(next);
    }
  }

  // Create and connect TCP peer
  if (connect_to.length() == 0)
  {
    connect_to = std::string("localhost:") + boost::lexical_cast<std::string>(network_port);
  }
  finroc::tcp::tTCPPeer* tcp_peer = new finroc::tcp::tTCPPeer(connect_to, "", finroc::tcp::tTCPPeer::eFULL, network_port, finroc::tcp::tTCPPeer::cDEFAULT_FILTER, true);
  tcp_peer->Init();
  try
  {
    tcp_peer->Connect();
  }
  catch (const finroc::util::tException& e1)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Error connecting Peer", e1);
  }

  if (executables.size() == 0)
  {
    finroc::core::tThreadContainer *main_thread = new finroc::core::tThreadContainer(runtime_environment, "Main Thread", links_are_unique ? finroc::core::tCoreFlags::cGLOBALLY_UNIQUE_LINK : 0);
    InitMainGroup(main_thread, remaining_args);
    executables.push_back(main_thread);
  }
  else
  {
    InitMainGroup(NULL, remaining_args);
  }

  for (size_t i = 0; i < executables.size(); i++)
  {
    finroc::core::tFrameworkElement* fe = executables[i];
    if (!fe->IsReady())
    {
      fe->Init();
    }
    if (pause_at_startup)
    {
      finroc::core::tExecutionControl::PauseAll(*fe); // Shouldn't be necessary, but who knows what people might implement
    }
    else
    {
      finroc::core::tExecutionControl::StartAll(*fe);
    }
  }

  run_main_loop = true;
  {
    std::unique_lock<std::mutex> l(main_thread_wait_mutex);
    while (run_main_loop)
    {
      main_thread_wait_variable.wait_for(l, std::chrono::seconds(10));
    }
  }
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Left main loop");

  // In many cases this is not necessary.
  // However, doing this before static deinitialization can avoid issues with external libraries and thread container threads still running.
  finroc::core::tRuntimeEnvironment::Shutdown();

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
