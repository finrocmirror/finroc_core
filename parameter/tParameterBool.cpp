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
#include "core/parameter/tParameter.h"
#include "core/port/tPort.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/cc/tCCPortBase.h"

namespace finroc
{
namespace core
{
void tParameterBool::Set(bool b)
{
  tCCPortDataManagerTL* cb = tThreadLocalCache::Get()->GetUnusedBuffer(rrlib::rtti::tDataType<bool>());
  (*(cb->GetObject()->GetData<bool>())) = b;
  std::string error = (static_cast<tCCPortBase*>(this->wrapped))->BrowserPublishRaw(cb);
  assert(error.size() == 0);
  cache->current_value = b;
}

} // namespace finroc
} // namespace core

