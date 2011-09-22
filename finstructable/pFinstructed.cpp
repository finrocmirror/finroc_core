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
const char * const cPROGRAM_DESCRIPTION = "This program instantiates and executes all .xml files passed as command line arguments.";

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

int cycle_time = 40;

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

//----------------------------------------------------------------------
// StartUp
//----------------------------------------------------------------------
void StartUp()
{
  rrlib::getopt::AddValue("cycle-time", 't', "Cycle time of main thread in ms (default is 40)", &CycleTimeHandler);
}

//----------------------------------------------------------------------
// InitMainGroup
//----------------------------------------------------------------------
void InitMainGroup(finroc::core::tThreadContainer *main_thread, std::vector<char*> remaining_args)
{

  // slightly ugly command line parsing
  for (size_t i = 0; i < remaining_args.size(); i++)
  {
    finroc::util::tString arg(remaining_args[i]);
    if (arg.Substring(arg.Length() - 4).Equals(".xml"))
    {
      finroc::util::tString group_name;
      finroc::util::tString xml_name;
      if (arg.Contains(":"))
      {
        std::vector<finroc::util::tString> split = arg.Split(":");
        if (split.size() != 2)
        {
          continue;
        }
        group_name = split[0];
        xml_name = split[1];
      }
      else
      {
        xml_name = arg;
        group_name = arg;
        if (arg.Contains("/"))
        {
          group_name = arg.Substring(arg.LastIndexOf("/") + 1); // cut off path
        }
        group_name = group_name.Substring(0, group_name.Length() - 4); // cut off .xml
      }

      new tFinstructableGroup(main_thread, group_name, xml_name);
    }
  }

  if (main_thread->ChildCount() == 0)
  {
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "No finstructable groups specified.");
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "Usage:  finstructed <xml-file1> <xml-file2>");
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_USER, "To set group name use <name>:<xml-file>. Otherwise xml-file name is used as group name.");
    exit(-1);
  }

  main_thread->SetCycleTime(cycle_time);
}

