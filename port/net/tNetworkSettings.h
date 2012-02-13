/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
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

#ifndef core__port__net__tNetworkSettings_h__
#define core__port__net__tNetworkSettings_h__

#include "core/tRuntimeSettings.h"

#include "core/port/tPortListener.h"
#include "core/parameter/tParameter.h"
#include "core/port/net/tUpdateTimeChangeListener.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Contains global settings for minimum network update intervals.
 */
class tNetworkSettings : public tFrameworkElement, public tPortListener<int16_t>
{
private:

  /*! List with listeners for update times */
  tUpdateTimeChangeListener::tManager update_time_listener;

public:

  tNetworkSettings();

  /*! Default minimum network update time (ms) */
  tParameter<int16_t> default_minimum_network_update_time;

  /*!
   * \param listener Listener to add
   */
  inline void AddUpdateTimeChangeListener(tUpdateTimeChangeListener* listener)
  {
    update_time_listener.Add(listener);
  }

  static tNetworkSettings& GetInstance();

  virtual void PortChanged(tAbstractPort* origin, const int16_t& value);

  /*!
   * Notify update time change listener of change
   *
   * \param dt Datatype whose default time has changed
   * \param time New time
   */
  inline void NotifyUpdateTimeChangeListener(rrlib::rtti::tDataTypeBase dt, int16 time)
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
};

} // namespace finroc
} // namespace core

#endif // core__port__net__tNetworkSettings_h__
