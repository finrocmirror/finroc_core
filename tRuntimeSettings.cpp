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
#include "core/port/cc/tNumberPort.h"

#include "core/tRuntimeSettings.h"
#include "finroc_core_utils/tAutoDeleter.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
tRuntimeSettings* tRuntimeSettings::inst = GetInstance();
const bool tRuntimeSettings::cCPP_CORE;
tBoolSetting* tRuntimeSettings::cWARN_ON_CYCLE_TIME_EXCEED = tRuntimeSettings::inst->Add("WARN_ON_CYCLE_TIME_EXCEED", true, true);
tLongSetting* tRuntimeSettings::cDEFAULT_CYCLE_TIME = tRuntimeSettings::inst->Add("DEFAULT_CYCLE_TIME", 50LL, true);
tIntSetting* tRuntimeSettings::cNUM_OF_LOOP_THREADS = tRuntimeSettings::inst->Add("NUM_OF_LOOP_THREADS", 8, false);
tIntSetting* tRuntimeSettings::cNUM_OF_EVENT_THREADS = tRuntimeSettings::inst->Add("NUM_OF_EVENT_THREADS", 2, false);
tIntSetting* tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME = tRuntimeSettings::inst->Add("DEFAULT_MINIMUM_NETWORK_UPDATE_TIME", 40, true);
const int tRuntimeSettings::cEDGE_LIST_DEFAULT_SIZE;
const int tRuntimeSettings::cEDGE_LIST_SIZE_INCREASE_FACTOR;
tBoolSetting* tRuntimeSettings::cDISPLAY_MODULE_UPDATES = tRuntimeSettings::inst->Add("DISPLAY_MODULE_UPDATES", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_MCA_MODULE_UPDATES = tRuntimeSettings::inst->Add("DISPLAY_MCA_MODULE_UPDATES", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_MCA_MESSAGES = tRuntimeSettings::inst->Add("DISPLAY_MCA_MESSAGES", true, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_MCA_BB_MESSAGES = tRuntimeSettings::inst->Add("DISPLAY_MCA_BB_MESSAGES", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_LOOP_TIME = tRuntimeSettings::inst->Add("DISPLAY_LOOP_TIME", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_DATATYPE_INIT = tRuntimeSettings::inst->Add("DISPLAY_DATATYPE_INIT", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_BUFFER_ALLOCATION = tRuntimeSettings::inst->Add("DISPLAY_BUFFER_ALLOCATION", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_INCOMING_PORT_INFO = tRuntimeSettings::inst->Add("DISPLAY_INCOMING_PORT_INFO", false, true);
tBoolSetting* tRuntimeSettings::cLOG_LOOP_TIMES = tRuntimeSettings::inst->Add("LOG_LOOP_TIMES", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_CONSOLE = tRuntimeSettings::inst->Add("DISPLAY_CONSOLE", false, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_CONSTRUCTION_DESTRUCTION = tRuntimeSettings::inst->Add("DISPLAY_CONSTRUCTION_DESTRUCTION", true, true);
tBoolSetting* tRuntimeSettings::cDISPLAY_EDGE_CREATION = tRuntimeSettings::inst->Add("DISPLAY_EDGE_CREATION", false, true);
tIntSetting* tRuntimeSettings::cSTREAM_THREAD_CYCLE_TIME = tRuntimeSettings::inst->Add("STREAM_THREAD_CYCLE_TIME", 200, true);
tIntSetting* tRuntimeSettings::running_in_applet = tRuntimeSettings::inst->Add("RUNNING_IN_APPLET", 0, false);
tIntSetting* tRuntimeSettings::cGARBAGE_COLLECTOR_SAFETY_PERIOD = tRuntimeSettings::inst->Add("GARBAGE_COLLECTOR_SAFETY_PERIOD", 5000, true);
const bool tRuntimeSettings::cCOLLECT_EDGE_STATISTICS;

tRuntimeSettings::tRuntimeSettings() :
    tSettings("Settings", "Runtime", true),
    update_time_listener()
{
  // add shared ports port
  //addChild(sharedPorts.getPortSet());

  // init data type register
  //DataTypeRegister.init();

  // add ports with update times
  //addChild(DataTypeRegister2.getInstance());

  // Beanshell console?
  //if (DISPLAY_CONSOLE) {
  //  new DebugConsole(null);
  //}
}

tRuntimeSettings* tRuntimeSettings::GetInstance()
{
  if (inst == NULL)
  {
    inst = new tRuntimeSettings();
    util::tAutoDeleter::AddStatic(inst);
  }
  return inst;
}

void tRuntimeSettings::StaticInit()
{
  //inst.sharedPorts = new SharedPorts(inst.portRoot);
  inst->Init(tRuntimeEnvironment::GetInstance());
  cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME->GetPort()->AddPortListener(inst);
}

} // namespace finroc
} // namespace core

