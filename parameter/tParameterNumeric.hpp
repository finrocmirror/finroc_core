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
#include "core/port/tPortFlags.h"
#include "core/datatype/tNumber.h"
#include "core/port/tPortWrapperBase.h"
#include "core/datatype/tUnit.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/port/cc/tCCPort.h"
#include "core/port/cc/tPortNumeric.h"
#include "core/tAnnotatable.h"
#include "core/port/cc/tCCPortBase.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b, const util::tString& config_entry)
{
  // this(description,parent,u,defaultValue,b);
  this->wrapped = new tPortImpl2(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, u), b, u);
  T d = default_value;
  if (b.InBounds(d))
  {
    ::finroc::core::tPortNumeric::SetDefault(tNumber(default_value, u));
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "Default value is out of bounds");
    ::finroc::core::tPortNumeric::SetDefault(tNumber(b.ToBounds(d), u));
  }
  (static_cast<tPortImpl2*>(this->wrapped))->current_value = default_value;
  (static_cast<tPortImpl2*>(this->wrapped))->info->SetConfigEntry(config_entry);
}

template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds b)
{
  // this(description,parent,Unit.NO_UNIT,defaultValue,b);
  this->wrapped = new tPortImpl2(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, &(tUnit::cNO_UNIT)), b, &(tUnit::cNO_UNIT));
  T d = default_value;
  if (b.InBounds(d))
  {
    ::finroc::core::tPortNumeric::SetDefault(tNumber(default_value, &(tUnit::cNO_UNIT)));
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "Default value is out of bounds");
    ::finroc::core::tPortNumeric::SetDefault(tNumber(b.ToBounds(d), &(tUnit::cNO_UNIT)));
  }
  (static_cast<tPortImpl2*>(this->wrapped))->current_value = default_value;
}

template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b)
{
  this->wrapped = new tPortImpl2(tPortCreationInfo(description, parent, tPortFlags::cINPUT_PORT, u), b, u);
  T d = default_value;
  if (b.InBounds(d))
  {
    ::finroc::core::tPortNumeric::SetDefault(tNumber(default_value, u));
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "Default value is out of bounds");
    ::finroc::core::tPortNumeric::SetDefault(tNumber(b.ToBounds(d), u));
  }
  (static_cast<tPortImpl2*>(this->wrapped))->current_value = default_value;
}

template<typename T>
void tParameterNumeric<T>::Set(T v)
{
  tCCPortDataContainer<tNumber>* cb = GetUnusedBuffer();
  cb->GetData()->SetValue(v, GetUnit());
  ::finroc::core::tCCPort<tNumber>::BrowserPublish(cb);
  (static_cast<tPortImpl2*>(this->wrapped))->current_value = v;
}

template<typename T>
void tParameterNumeric<T>::SetConfigEntry(const util::tString& config_entry)
{
  (static_cast<tPortImpl2*>(this->wrapped))->info->SetConfigEntry(config_entry);
}


template<typename T>
tParameterNumeric<T>::tPortImpl2::tPortImpl2(tPortCreationInfo pci, tBounds b, tUnit* u) :
    tPortNumericBounded::tPortImpl(ProcessPci(pci), b, u),
    info(new tParameterInfo()),
    current_value()
{
  AddAnnotation(info);
  AddPortListenerRaw(this);
}

template<typename T>
void tParameterNumeric<T>::tPortImpl2::PortChanged(tCCPortBase* origin, const tCCPortData* value_raw)
{
  const tNumber* value = reinterpret_cast<const tNumber*>(value_raw);
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
void tParameterNumeric<T>::tPortImpl2::PostChildInit()
{
  ::finroc::core::tFrameworkElement::PostChildInit();
  try
  {
    this->info->LoadValue(true);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, ::finroc::core::tFrameworkElement::log_domain, e);
  }
}

} // namespace finroc
} // namespace core

