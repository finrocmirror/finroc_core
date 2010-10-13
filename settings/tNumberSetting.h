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
#ifndef CORE__SETTINGS__TNUMBERSETTING_H
#define CORE__SETTINGS__TNUMBERSETTING_H

#include "rrlib/finroc_core_utils/tJCBase.h"
#include "core/settings/tSetting.h"
#include "core/port/cc/tCCPortListener.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/datatype/tBounds.h"

namespace finroc
{
namespace core
{

class tNumberPort;

class tNumberSettingUtil
{
public:
  static tAbstractPort* CreatePort(tPortCreationInfo pci, const tBounds& bounds, tCCPortListener<tCoreNumber>* l);

  static tNumberPort* GetPort(tAbstractPort* p);

  // Helper - because NumberPort is forward-declared and not available in template
  static void SetDefault(tAbstractPort* p, tCoreNumber value);
};

/*!
 * @author max
 *
 * Numerical Setting
 */
template <typename T>
class tNumberSetting : public tSetting, public tCCPortListener<tCoreNumber>
{
private:

  /*! Current value */
  T current_value;

  /*! Unit */
  tUnit* unit;

  /*! Bounds */
  tBounds bounds;

public:

  tNumberSetting(const ::finroc::util::tString& description, T default_val, bool publish_as_port, tUnit* unit_ = &(tUnit::cNO_UNIT), tBounds bounds_ = tBounds()) :
      tSetting(description, publish_as_port),
      current_value(default_val),
      unit(unit_),
      bounds(bounds_)
  {}

  T Get() const
  {
    return current_value;
  }

  virtual tAbstractPort* CreatePort(tFrameworkElement* parent)
  {
    tAbstractPort* p = tNumberSettingUtil::CreatePort(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, unit), bounds, this);
    tNumberSettingUtil::SetDefault(p, current_value);
    return p;
  }

  virtual void PortChanged(tCCPortBase* origin, const tCoreNumber* value)
  {
    current_value = value->Value<T>();
  }

  tNumberPort* GetPort()
  {
    return tNumberSettingUtil::GetPort(tSetting::GetPort());
  }
};

typedef tNumberSetting<bool> tBoolSetting;
typedef tNumberSetting<int> tIntSetting;
typedef tNumberSetting<int64> tLongSetting;
typedef tNumberSetting<double> tDoubleSetting;
typedef tNumberSetting<float> tFloatSetting;

} // namespace finroc
} // namespace core


#endif // CORE__SETTINGS__TNUMBERSETTING_H
