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
#include "core/datatype/tNumber.h"
#include "core/datatype/tBounds.h"
#include "core/port/tPortWrapperBase.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "core/datatype/tBoolean.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/cc/tCCPortBase.h"

namespace finroc
{
namespace core
{
template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry) :
    tParameterBase<T>(description, parent, config_entry, tNumber::cTYPE),
    cache(new tNumberCache())
{
  this->AddPortListener(cache.get());
}

template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, const T& default_value, tUnit* u, const util::tString& config_entry) :
    tParameterBase<T>(description, parent, GetDefaultValue(default_value), u, config_entry, tNumber::cTYPE),
    cache(new tNumberCache())
{
  cache->current_value = default_value;
  this->AddPortListener(cache.get());
}

template<typename T>
tParameterNumeric<T>::tParameterNumeric(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds<T> b, tUnit* u, const util::tString& config_entry) :
    tParameterBase<T>(description, parent, GetDefaultValue(default_value), b, u, config_entry, tNumber::cTYPE),
    cache(new tNumberCache())
{
  T d = default_value;
  if (b.InBounds(d))
  {
    this->SetDefault(default_value);
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "Default value is out of bounds");

    this->SetDefault(default_value);
  }
  cache->current_value = default_value;
  this->AddPortListener(cache.get());
}

template<typename T>
void tParameterNumeric<T>::Set(T v)
{
  tCCPortDataManagerTL* cb = tThreadLocalCache::Get()->GetUnusedBuffer(tBoolean::cTYPE);
  cb->GetObject()->GetData<tNumber>()->SetValue(v, (static_cast<tCCPortBase*>(this->wrapped))->GetUnit());
  (static_cast<tCCPortBase*>(this->wrapped))->BrowserPublishRaw(cb);
  cache->current_value = v;
}

template<typename T>
tParameterNumeric<T>::tNumberCache::tNumberCache() :
    current_value(0)
{
}

} // namespace finroc
} // namespace core

