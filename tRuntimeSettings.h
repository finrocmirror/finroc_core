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

#ifndef core__tRuntimeSettings_h__
#define core__tRuntimeSettings_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/tFrameworkElement.h"
#include "core/port/tPortListener.h"
#include "core/port/net/tUpdateTimeChangeListener.h"
#include "rrlib/serialization/tDataTypeBase.h"

namespace finroc
{
namespace core
{
template<typename T>
class tParameterNumeric;
class tParameterBool;

template <typename T>
class tParameter;

/*!
 * \author Max Reichardt
 *
 * Contains global settings of runtime environment.
 *
 * For variable settings it contains ports inside a module.
 *
 * staticInit() should be called after runtime and data types have been initialized.
 */
class tRuntimeSettings : public tFrameworkElement, public tPortListener<int>
{
private:

  /*! Singleton Instance */
  static tRuntimeSettings* inst;

  /*! List with listeners for update times */
  tUpdateTimeChangeListener::tManager update_time_listener;

public:

  /*! Display warning, if loop times of CoreLoopThreads are exceeded? */
  static tParameter<bool>* cWARN_ON_CYCLE_TIME_EXCEED;

  /*! Default cycle time of CoreLoopThreads in ms*/
  static tParameter<long long int>* cDEFAULT_CYCLE_TIME;

  /*! Default number of event threads */
  //public static final IntSetting NUM_OF_EVENT_THREADS = inst.add("NUM_OF_EVENT_THREADS", 2, false);

  /*! Default minimum network update time (ms) */
  static tParameter<int>* cDEFAULT_MINIMUM_NETWORK_UPDATE_TIME;

  static const int cEDGE_LIST_DEFAULT_SIZE = 0;

  static const int cEDGE_LIST_SIZE_INCREASE_FACTOR = 2;

  /*! Loop time for buffer tracker (in ms) */
  //public static final IntSetting BUFFER_TRACKER_LOOP_TIME = inst.add("BUFFER_TRACKER_LOOP_TIME", 140, true);

  /*! Cycle time for stream thread */
  static tParameter<int>* cSTREAM_THREAD_CYCLE_TIME;

  /*! > 0 if Runtime is instantiated in Java Applet - contains bit size of server CPU */
  //public static final IntSetting runningInApplet = inst.add("RUNNING_IN_APPLET", 0, false);

  /*!
   * Period in ms after which garbage collector will delete objects... any threads
   * still working on objects while creating deletion task should be finished by then
   */
  static tParameter<int>* cGARBAGE_COLLECTOR_SAFETY_PERIOD;

  /*! Collect edge statistics ? */
  static const bool cCOLLECT_EDGE_STATISTICS = false;

protected:

  tRuntimeSettings();

public:

  /*!
   * \param listener Listener to add
   */
  inline void AddUpdateTimeChangeListener(tUpdateTimeChangeListener* listener)
  {
    update_time_listener.Add(listener);
  }

  /*! @return Singleton instance */
  static tRuntimeSettings* GetInstance();

  virtual void PortChanged(tAbstractPort* origin, const int& value)
  {
    update_time_listener.Notify(NULL, NULL, static_cast<int16>(value));
  }

  /*!
   * Notify update time change listener of change
   *
   * \param dt Datatype whose default time has changed
   * \param time New time
   */
  inline void NotifyUpdateTimeChangeListener(rrlib::serialization::tDataTypeBase dt, int16 time)
  {
    update_time_listener.Notify(&(dt), NULL, time);
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

#endif // core__tRuntimeSettings_h__
