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
#include "core/port/net/tNetworkSettings.h"
#include "core/plugin/tPlugin.h"

namespace finroc
{
namespace core
{

namespace internal
{

/*!
 * \author Max Reichardt
 *
 * RPC mechanism plugin
 */
class tNetworkBasePlugin : public tPlugin
{

  virtual void Init()
  {
    tNetworkSettings::GetInstance();
  }
};

static tNetworkBasePlugin rpc_plugin_instance;

}


tNetworkSettings::tNetworkSettings() :
  tFrameworkElement(tRuntimeSettings::GetInstance(), "Default minimum network update times"),
  update_time_listener(),
  default_minimum_network_update_time("Global Default", this, 40, tBounds<int16_t>(1, 2000))
{
  default_minimum_network_update_time.AddPortListener(this);

  for (int16_t i = 0; i < rrlib::rtti::tDataTypeBase::GetTypeCount(); i++)
  {
    rrlib::rtti::tDataTypeBase dt = rrlib::rtti::tDataTypeBase::GetType(i);
    if (dt != NULL && (tFinrocTypeInfo::IsCCType(dt) || tFinrocTypeInfo::IsStdType(dt)))
    {
      tParameter<int16_t> p(dt.GetName(), this, -1, tBounds<int16_t>(-1, 10000));
      p.AddPortListener(this);
    }
  }
}

tNetworkSettings& tNetworkSettings::GetInstance()
{
  static tNetworkSettings* settings = new tNetworkSettings();
  return *settings;
}

void tNetworkSettings::PortChanged(tAbstractPort* origin, const int16_t& value)
{
  if (origin == default_minimum_network_update_time)
  {
    update_time_listener.Notify(NULL, NULL, static_cast<int16>(value));
  }
  else
  {
    rrlib::rtti::tDataTypeBase dt = rrlib::rtti::tDataTypeBase::FindType(origin->GetName());
    update_time_listener.Notify(&(dt), NULL, value);
  }
}


} // namespace finroc
} // namespace core

