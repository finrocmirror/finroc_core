/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/parameter/tParameterInfo.h"
#include "core/tAnnotatable.h"
#include "core/datatype/tCoreNumber.h"
#include "core/port/cc/tCCPort.h"
#include "core/datatype/tUnit.h"
#include "core/port/cc/tNumberPort.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/port/cc/tCCPortDataContainer.h"

namespace finroc
{
namespace core
{
template<typename T>
tNumericParameter<T>::tNumericParameter(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b, const util::tString& config_entry) :
    tBoundedNumberPort(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, u), b),
    info(new tParameterInfo()),
    current_value(default_value)
{
  // this(description,parent,u,defaultValue,b);
  AddAnnotation(info);
  ::finroc::core::tNumberPort::SetDefault(tCoreNumber(default_value, u));
  AddPortListener(this);
  info->SetConfigEntry(config_entry);
}

template<typename T>
tNumericParameter<T>::tNumericParameter(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds b) :
    tBoundedNumberPort(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, &(tUnit::cNO_UNIT)), b),
    info(new tParameterInfo()),
    current_value(default_value)
{
  // this(description,parent,Unit.NO_UNIT,defaultValue,b);
  AddAnnotation(info);
  ::finroc::core::tNumberPort::SetDefault(tCoreNumber(default_value, &(tUnit::cNO_UNIT)));
  AddPortListener(this);
}

template<typename T>
tNumericParameter<T>::tNumericParameter(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b) :
    tBoundedNumberPort(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, u), b),
    info(new tParameterInfo()),
    current_value(default_value)
{
  AddAnnotation(info);
  ::finroc::core::tNumberPort::SetDefault(tCoreNumber(default_value, u));
  AddPortListener(this);
}

template<typename T>
void tNumericParameter<T>::PortChanged(tCCPortBase* origin, const tCoreNumber* value)
{
  if (GetUnit() != value->GetUnit())
  {
    double val = value->GetUnit()->ConvertTo(value->DoubleValue(), GetUnit());

    current_value = (T)val;
  }
  else
  {
    current_value = value->Value<T>();
  }
}

template<typename T>
void tNumericParameter<T>::PostChildInit()
{
  ::finroc::core::tFrameworkElement::PostChildInit();
  try
  {
    info->LoadValue(true);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
  }
}

template<typename T>
void tNumericParameter<T>::Set(T v)
{
  tCCPortDataContainer<tCoreNumber>* cb = GetUnusedBuffer();
  cb->GetData()->SetValue(v, GetUnit());
  ::finroc::core::tCCPortBase::Publish(reinterpret_cast<tCCPortDataContainer<>*>(cb));
  current_value = v;
}

} // namespace finroc
} // namespace core

