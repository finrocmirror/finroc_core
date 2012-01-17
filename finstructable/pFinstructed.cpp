/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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

#include "core/default_main_wrapper.h"
#include "core/finstructable/tFinstructableGroup.h"
#include "core/tRuntimeEnvironment.h"
#include "rrlib/finroc_core_utils/sFiles.h"

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
using namespace finroc::core;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
const char * const cPROGRAM_VERSION = "ver 1.0";
const char * const cPROGRAM_DESCRIPTION = "This program instantiates and executes all .finroc files passed as command line arguments.";

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

int cycle_time = 40;
tFinstructableThreadContainer* finstructable_thread_container = NULL;
std::vector<finroc::util::tString> finroc_file_extra_args;

/*!
 * Parses command line arguments of the form [<group_name>]:<xml-name>
 */
bool ParseXMLArg(const finroc::util::tString& arg, finroc::util::tString& group_name, finroc::util::tString& xml_name)
{
  if (!arg.Substring(arg.Length() - 7).Equals(".finroc"))
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Error parsing argument: ", arg);
    return false;
  }
  if (arg.Contains(":"))
  {
    std::vector<finroc::util::tString> split = arg.Split(":");
    if (split.size() != 2)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Error parsing argument: ", arg);
      return false;
    }
    group_name = split[0];
    xml_name = split[1];
  }
  else
  {
    xml_name = arg;
    if (finroc::util::sFiles::FinrocFileExists(xml_name))
    {
      try
      {
        rrlib::xml2::tXMLDocument doc(finroc::util::sFiles::GetFinrocXMLDocument(xml_name, false));
        rrlib::xml2::tXMLNode& root = doc.RootNode();
        if (root.HasAttribute("defaultname"))
        {
          group_name = root.GetStringAttribute("defaultname");
        }
      }
      catch (std::exception& e)
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Error scanning file: ", xml_name);
      }
    }
    if (group_name.Length() == 0)
    {
      group_name = arg;
      if (arg.Contains("/"))
      {
        group_name = arg.Substring(arg.LastIndexOf("/") + 1); // cut off path
      }
      group_name = group_name.Substring(0, group_name.Length() - 7); // cut off .finroc
    }
  }
  return true;
}

bool CycleTimeHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption port_option(name_to_option_map.at("cycle-time"));
  if (port_option->IsActive())
  {
    const char* time_string = boost::any_cast<const char *>(port_option->GetValue());
    int time = atoi(time_string);
    if (time < 1 || time > 10000)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Invalid cycle time '", time_string, "'. Using default: ", cycle_time, " ms");
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "Setting main thread cycle time to ", time, " ms.");
      cycle_time = time;
    }
  }

  return true;
}

bool MainHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  rrlib::getopt::tOption port_option(name_to_option_map.at("main"));
  if (port_option->IsActive())
  {
    finroc::util::tString main_xml(boost::any_cast<const char *>(port_option->GetValue()));
    finroc::util::tString group_name;
    finroc::util::tString xml_name;
    if (ParseXMLArg(main_xml, group_name, xml_name))
    {
      finstructable_thread_container = new tFinstructableThreadContainer(finroc::core::tRuntimeEnvironment::GetInstance(), group_name, xml_name, links_are_unique ? tCoreFlags::cGLOBALLY_UNIQUE_LINK : 0);
      finstructable_thread_container->SetMainName(group_name);
    }
  }

  return true;
}

bool FinrocFileArgHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  for (size_t i = 0; i < finroc_file_extra_args.size(); i++)
  {
    rrlib::getopt::tOption extra_option(name_to_option_map.at(finroc_file_extra_args[i].GetCString()));
    if (extra_option->IsActive())
    {
      finroc::core::tRuntimeEnvironment::GetInstance()->AddCommandLineArgument(finroc_file_extra_args[i], boost::any_cast<const char *>(extra_option->GetValue()));
    }
  }

  return true;
}

//----------------------------------------------------------------------
// StartUp
//----------------------------------------------------------------------
void StartUp()
{
  rrlib::getopt::AddValue("cycle-time", 't', "Cycle time of main thread in ms (default is 40)", &CycleTimeHandler);
  rrlib::getopt::AddValue("main", 'm', ".finroc file of main program (FinstructableThreadContainer). Will replace 'Main thread'.", &MainHandler);

  // search for further command line arguments in main file that might have been provided
  for (int i = 0; i < finroc_argc_copy; i++)
  {
    finroc::util::tString arg(finroc_argv_copy[i]);
    finroc::util::tString main;
    if (arg.StartsWith("--main="))
    {
      main = arg.Substring(7);
    }
    else if (i < (finroc_argc_copy - 1) && arg.Equals("-m"))
    {
      i++;
      main = finroc_argv_copy[i];
    }
    if (main.Length() > 0 && main.EndsWith(".finroc"))
    {
      finroc_file_extra_args = finroc::core::tFinstructableGroup::ScanForCommandLineArgs(main);
      for (size_t i = 0; i < finroc_file_extra_args.size(); i++)
      {
        rrlib::getopt::AddValue(finroc_file_extra_args[i].GetCString(), 0, "", &FinrocFileArgHandler);
      }
      return;
    }
  }
}

//----------------------------------------------------------------------
// InitMainGroup
//----------------------------------------------------------------------
void InitMainGroup(finroc::core::tThreadContainer *main_thread, std::vector<char*> remaining_args)
{
  finroc::core::tFrameworkElement* parent = main_thread;
  if (parent == NULL)
  {
    parent = finstructable_thread_container;
  }
  assert(parent != NULL);

  // slightly ugly command line parsing
  for (size_t i = 0; i < remaining_args.size(); i++)
  {
    finroc::util::tString arg(remaining_args[i]);
    if (arg.Substring(arg.Length() - 4).Equals(".xml"))
    {
      finroc::util::tString group_name;
      finroc::util::tString xml_name;
      if (ParseXMLArg(arg, group_name, xml_name))
      {
        new tFinstructableGroup(main_thread, group_name, xml_name);
      }
    }
  }

  if ((main_thread == NULL || main_thread->ChildCount() == 0) && finstructable_thread_container == NULL)
  {
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "No finstructable groups specified.");
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "Usage:  finroc [-m <main-xml-file>] <xml-file1> <xml-file2>");
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "To set group name use <name>:<xml-file>. Otherwise xml-file name is used as group name.");
    exit(-1);
  }

  if (main_thread)
  {
    main_thread->SetCycleTime(cycle_time);
  }
  else if (finstructable_thread_container)
  {
    finstructable_thread_container->SetCycleTime(cycle_time);
  }
}

