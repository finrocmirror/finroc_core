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
#include "core/tRuntimeSettings.h"
#include "rrlib/finroc_core_utils/tAutoDeleter.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/cc/tNumberPort.h"
#include "core/settings/tSetting.h"

namespace finroc
{
namespace core
{
tRuntimeSettings* tRuntimeSettings::inst = GetInstance();
tBoolSetting* tRuntimeSettings::cWARN_ON_CYCLE_TIME_EXCEED = tRuntimeSettings::inst->Add("WARN_ON_CYCLE_TIME_EXCEED", true, true);
tLongSetting* tRuntimeSettings::cDEFAULT_CYCLE_TIME = tRuntimeSettings::inst->Add("DEFAULT_CYCLE_TIME", 50LL, true);
tIntSetting* tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME = tRuntimeSettings::inst->Add("DEFAULT_MINIMUM_NETWORK_UPDATE_TIME", 40, true);
const int tRuntimeSettings::cEDGE_LIST_DEFAULT_SIZE;
const int tRuntimeSettings::cEDGE_LIST_SIZE_INCREASE_FACTOR;
tIntSetting* tRuntimeSettings::cSTREAM_THREAD_CYCLE_TIME = tRuntimeSettings::inst->Add("STREAM_THREAD_CYCLE_TIME", 200, true);
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

