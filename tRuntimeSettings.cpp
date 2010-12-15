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
#include "core/parameter/tParameterBool.h"
#include "core/parameter/tParameterNumeric.h"

#include "core/tRuntimeSettings.h"
#include "core/tRuntimeEnvironment.h"
#include "core/parameter/tParameterBool.h"
#include "core/parameter/tParameterNumeric.h"
#include "core/datatype/tBounds.h"
#include "rrlib/finroc_core_utils/tAutoDeleter.h"

namespace finroc
{
namespace core
{
tRuntimeSettings* tRuntimeSettings::inst = NULL;
tParameterBool* tRuntimeSettings::cWARN_ON_CYCLE_TIME_EXCEED;
tParameterNumeric<int64>* tRuntimeSettings::cDEFAULT_CYCLE_TIME;
tParameterNumeric<int>* tRuntimeSettings::cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME;
const int tRuntimeSettings::cEDGE_LIST_DEFAULT_SIZE;
const int tRuntimeSettings::cEDGE_LIST_SIZE_INCREASE_FACTOR;
tParameterNumeric<int>* tRuntimeSettings::cSTREAM_THREAD_CYCLE_TIME;
tParameterNumeric<int>* tRuntimeSettings::cGARBAGE_COLLECTOR_SAFETY_PERIOD;
const bool tRuntimeSettings::cCOLLECT_EDGE_STATISTICS;

tRuntimeSettings::tRuntimeSettings() :
    tFrameworkElement(tRuntimeEnvironment::GetInstance(), "Settings"),
    update_time_listener()
{
  cWARN_ON_CYCLE_TIME_EXCEED = util::tAutoDeleter::AddStatic(new tParameterBool("WARN_ON_CYCLE_TIME_EXCEED", this, true));
  cDEFAULT_CYCLE_TIME = util::tAutoDeleter::AddStatic(new tParameterNumeric<int64>("DEFAULT_CYCLE_TIME", this, 50LL, tBounds(1, 2000)));
  cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME = util::tAutoDeleter::AddStatic(new tParameterNumeric<int>("DEFAULT_MINIMUM_NETWORK_UPDATE_TIME", this, 40, tBounds(1, 2000)));
  cSTREAM_THREAD_CYCLE_TIME = util::tAutoDeleter::AddStatic(new tParameterNumeric<int>("STREAM_THREAD_CYCLE_TIME", this, 200, tBounds(1, 2000)));
  cGARBAGE_COLLECTOR_SAFETY_PERIOD = util::tAutoDeleter::AddStatic(new tParameterNumeric<int>("GARBAGE_COLLECTOR_SAFETY_PERIOD", this, 5000, tBounds(500, 50000)));

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
    //util::tAutoDeleter::AddStatic(inst);
  }
  return inst;
}

void tRuntimeSettings::StaticInit()
{
  //inst.sharedPorts = new SharedPorts(inst.portRoot);
  //inst.init(RuntimeEnvironment.getInstance());
  GetInstance();
  cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME->AddPortListener(inst);
}

} // namespace finroc
} // namespace core

