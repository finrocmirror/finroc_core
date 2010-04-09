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
#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"

#include "core/settings/tSettings.h"

namespace finroc
{
namespace core
{
tSettings::~tSettings()
{
  if (is_static)
  {
    tRuntimeEnvironment::Shutdown();
  }
  if (port_root != NULL && (!tRuntimeEnvironment::ShuttingDown()))
  {
    port_root->ManagedDelete();
  }
  for (size_t i = 0, n = settings.Size(); i < n; i++)
  {
    delete settings.Get(i);
  }
  settings.Clear();
}

tSettings::tSettings(const util::tString& description_, const util::tString& config_prefix_, bool is_static_) :
    config_prefix(config_prefix_ + "."),
    description(description_),
    settings(),
    initialized(false),
    is_static(is_static_),
    port_root(NULL)
{
}

tBoolSetting* tSettings::Add(const util::tString& description_, bool default_val, bool publish_as_port)
{
  tBoolSetting* setting = new tBoolSetting(description_, default_val, publish_as_port);
  settings.Add(setting);
  return setting;
}

tIntSetting* tSettings::Add(const util::tString& description_, int default_val, bool publish_as_port, tUnit* unit, tBounds bounds)
{
  tIntSetting* setting = new tIntSetting(description_, default_val, publish_as_port, unit, bounds);
  settings.Add(setting);
  return setting;
}

tLongSetting* tSettings::Add(const util::tString& description_, int64 default_val, bool publish_as_port, tUnit* unit, tBounds bounds)
{
  tLongSetting* setting = new tLongSetting(description_, default_val, publish_as_port, unit, bounds);
  settings.Add(setting);
  return setting;
}

tDoubleSetting* tSettings::Add(const util::tString& description_, double default_val, bool publish_as_port, tUnit* unit, tBounds bounds)
{
  tDoubleSetting* setting = new tDoubleSetting(description_, default_val, publish_as_port, unit, bounds);
  settings.Add(setting);
  return setting;
}

void tSettings::Init(tFrameworkElement* parent)
{
  if (initialized)
  {
    return;
  }
  port_root = new tFrameworkElement(description, parent);
  for (int i = 0, n = settings.Size(); i < n; i++)
  {
    tSetting* set = settings.Get(i);
    if (set->PublishAsPort())
    {
      set->port = set->CreatePort(port_root);
    }
  }
  port_root->Init();
  initialized = true;
}

} // namespace finroc
} // namespace core

