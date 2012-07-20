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
#include "core/port/rpc/tSynchMethodCallLogic.h"
#include "core/port/rpc/tMethodCallSyncher.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tThreadLocalRPCData.h"

namespace finroc
{
namespace core
{
void tSynchMethodCallLogic::HandleMethodReturn(tAbstractCall::tPtr& call)
{
  // return value
  tMethodCallSyncher* mcs = tMethodCallSyncher::Get(call->GetSyncherID());
  if (mcs)
  {
    mcs->ReturnValue(call);
  }
  else
  {
    FINROC_LOG_PRINTF(WARNING, "Received method return value with invalid synchronization id %d: '%s'. Ignoring.", call->GetSyncherID(), call->ToString().c_str());
  }
}

} // namespace finroc
} // namespace core

