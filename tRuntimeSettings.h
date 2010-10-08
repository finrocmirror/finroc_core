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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__TRUNTIMESETTINGS_H
#define CORE__TRUNTIMESETTINGS_H

#include "core/port/net/tUpdateTimeChangeListener.h"
#include "core/datatype/tCoreNumber.h"
#include "core/settings/tSettings.h"
#include "core/port/cc/tCCPortListener.h"

#include "core/settings/tNumberSetting.h"

namespace finroc
{
namespace core
{
class tSetting;
class tDataType;
class tCCPortBase;

/*!
 * \author Max Reichardt
 *
 * Contains global settings of runtime environment.
 *
 * For variable settings it contains ports inside a module.
 *
 * staticInit() should be called after runtime and data types have been initialized.
 */
class tRuntimeSettings : public tSettings, public tCCPortListener<tCoreNumber>
{
private:

  /*! Singleton Instance */
  static tRuntimeSettings* inst;

  /*! List with listeners for update times */
  tUpdateTimeChangeListener::tManager update_time_listener;

public:

  /*!
   * This is turned to #ifdefs in C++
   * Perform checks on buffer usage and report unsafeties.
   * If no such unsafeties are reported when running an application,
   * this flag may be turned off in release mode to gain performance.
   */
  //public static final boolean CORE_DEBUG_CHECKS = false; // Replaced by asserts

  /*!
   * There's a Java and C++ version of this framework. Java modules can
   * be used in both. This flag indicates, whether the Java modules are
   * used together with a C++ core.
   */
  static const bool cCPP_CORE = false;

  /*! Display warning, if loop times of CoreLoopThreads are exceeded? */
  static tBoolSetting* cWARN_ON_CYCLE_TIME_EXCEED;

  /*! Default cycle time of CoreLoopThreads in ms*/
  static tLongSetting* cDEFAULT_CYCLE_TIME;

  /*! Maximum number of threads */
  //public static final IntSetting MAX_THREADS = inst.add("MAX_THREADS", 256, false); // only required during initialization

  /*! Default number of loop threads */
  static tIntSetting* cNUM_OF_LOOP_THREADS;

  /*! Default number of self updating loop threads */
  //public static final IntSetting SELF_UPDATING_LOOP_THREADS = inst.add("SELF_UPDATING_LOOP_THREADS", 8, false);

  /*! Default number of event threads */
  static tIntSetting* cNUM_OF_EVENT_THREADS;

  /*! Maximum queue size for reference queues in ports */
  //public static final int MAX_PORT_REFERENCE_QUEUE_SIZE = getInt("MAX_PORT_REFERENCE_QUEUE_SIZE", Integer.MAX_VALUE);

  /*! Default minimum network update time (ms) */
  static tIntSetting* cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME;

  static const int cEDGE_LIST_DEFAULT_SIZE = 0;

  static const int cEDGE_LIST_SIZE_INCREASE_FACTOR = 2;

  /*! Debug Settings */
  /*public static final BoolSetting DISPLAY_MODULE_UPDATES = inst.add("DISPLAY_MODULE_UPDATES", false, true);
  public static final BoolSetting DISPLAY_MCA_MODULE_UPDATES = inst.add("DISPLAY_MCA_MODULE_UPDATES", false, true);
  public static final BoolSetting DISPLAY_MCA_MESSAGES = inst.add("DISPLAY_MCA_MESSAGES", true, true);
  public static final BoolSetting DISPLAY_MCA_BB_MESSAGES = inst.add("DISPLAY_MCA_BB_MESSAGES", false, true);
  public static final BoolSetting DISPLAY_LOOP_TIME = inst.add("DISPLAY_LOOP_TIME", false, true);
  public static final BoolSetting DISPLAY_DATATYPE_INIT = inst.add("DISPLAY_DATATYPE_INIT", false, true);
  public static final BoolSetting DISPLAY_BUFFER_ALLOCATION = inst.add("DISPLAY_BUFFER_ALLOCATION", false, true);
  public static final BoolSetting DISPLAY_INCOMING_PORT_INFO = inst.add("DISPLAY_INCOMING_PORT_INFO", false, true);
  public static final BoolSetting LOG_LOOP_TIMES = inst.add("LOG_LOOP_TIMES", false, true);
  public static final BoolSetting DISPLAY_CONSOLE = inst.add("DISPLAY_CONSOLE", false, true);*/

  /*! Loop time for buffer tracker (in ms) */
  //public static final IntSetting BUFFER_TRACKER_LOOP_TIME = inst.add("BUFFER_TRACKER_LOOP_TIME", 140, true);

  /*! Cycle time for stream thread */
  static tIntSetting* cSTREAM_THREAD_CYCLE_TIME;

  /*! > 0 if Runtime is instantiated in Java Applet - contains bit size of server CPU */
  static tIntSetting* running_in_applet;

  /*!
   * Period in ms after which garbage collector will delete objects... any threads
   * still working on objects while creating deletion task should be finished by then
   */
  static tIntSetting* cGARBAGE_COLLECTOR_SAFETY_PERIOD;

  /*! Port with information about shared ports */
  //SharedPorts sharedPorts; // always has port index zero

  /*! Collect edge statistics ? */
  static const bool cCOLLECT_EDGE_STATISTICS = false;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "settings");

private:

  inline static const char* GetLogDescription()
  {
    return "RuntimeSettings";
  }

protected:

  tRuntimeSettings();

public:

  //  @Override
  //  protected void update() {}

  //@Override
  //public synchronized void delete() {}

  //  /**
  //   * \return Local information list about shared ports
  //   */
  //  public SharedPorts getSharedPorts() {
  //      return sharedPorts;
  //  }

  /*!
   * \param listener Listener to add
   */
  inline void AddUpdateTimeChangeListener(tUpdateTimeChangeListener* listener)
  {
    update_time_listener.Add(listener);
  }

  /*! @return Singleton instance */
  static tRuntimeSettings* GetInstance();

  /*!
   * Notify update time change listener of change
   *
   * \param dt Datatype whose default time has changed
   * \param time New time
   */
  inline void NotifyUpdateTimeChangeListener(tDataType* dt, int16 time)
  {
    update_time_listener.Notify(dt, NULL, time);
  }

  virtual void PortChanged(tCCPortBase* origin, const tCoreNumber* value)
  {
    update_time_listener.Notify(NULL, NULL, static_cast<int16>(value->IntValue()));
  }

  /*!
   * \param listener Listener to remove
   */
  inline void RemoveUpdateTimeChangeListener(tUpdateTimeChangeListener* listener)
  {
    update_time_listener.Remove(listener);
  }

  /*! Completes initialization */
  static void StaticInit();

};

} // namespace finroc
} // namespace core

#endif // CORE__TRUNTIMESETTINGS_H
