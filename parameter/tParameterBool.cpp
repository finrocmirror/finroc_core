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
#include "core/parameter/tParameterBool.h"
#include "core/port/tPort.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/cc/tCCPortBase.h"

namespace finroc
{
namespace core
{
tParameterBool::tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value, const util::tString& config_entry) :
    tParameterBase<bool>(description, parent),
    cache(new tBoolCache())
{
  // this(description,parent,defaultValue);
  this->AddPortListener(cache.get());
  cache->current_value = default_value;
  SetDefault(default_value);
  SetConfigEntry(config_entry);
}

tParameterBool::tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value) :
    tParameterBase<bool>(description, parent),
    cache(new tBoolCache())
{
  this->AddPortListener(cache.get());
  cache->current_value = default_value;
  SetDefault(default_value);
}

void tParameterBool::Set(bool b)
{
  tCCPortDataManagerTL* cb = tThreadLocalCache::Get()->GetUnusedBuffer(tBoolean::cTYPE);
  cb->GetObject()->GetData<tBoolean>()->Set(b);
  (static_cast<tCCPortBase*>(this->wrapped))->BrowserPublishRaw(cb);
  cache->current_value = b;
}

} // namespace finroc
} // namespace core

