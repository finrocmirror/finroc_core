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

#include "core/port/rpc/tThreadLocalRPCData.h"
#include "core/port/rpc/tMethodCallSyncher.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{

tThreadLocalRPCData::tThreadLocalRPCData() :
  method_syncher(NULL),
  method_calls(new util::tReusablesPool<tMethodCall>()),
  pull_calls(new util::tReusablesPool<tPullCall>())
{
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Creating tThreadLocalRPCData for thread ", rrlib::thread::tThread::CurrentThread().GetName());
  tThreadLocalCache::GetFast()->AddAnnotation(this);
}

tThreadLocalRPCData::~tThreadLocalRPCData()
{
  /*! Return MethodCallSyncher to pool */
  if (method_syncher != NULL && (!tRuntimeEnvironment::ShuttingDown()))
  {
    method_syncher->Release();
  }

  method_calls->ControlledDelete();
  pull_calls->ControlledDelete();
}

tMethodCall* tThreadLocalRPCData::CreateMethodCall()
{
  tMethodCall* result = new tMethodCall();
  method_calls->Attach(result, false);
  return result;
}

tMethodCallSyncher* tThreadLocalRPCData::CreateMethodSyncher()
{
  return tMethodCallSyncher::GetFreeInstance(tThreadLocalCache::GetFast());
}

tPullCall* tThreadLocalRPCData::CreatePullCall()
{
  tPullCall* result = new tPullCall();
  pull_calls->Attach(result, false);
  return result;
}

} // namespace finroc
} // namespace core

