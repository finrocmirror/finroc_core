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
#include "core/port/rpc/tPullCall.h"
#include "rrlib/serialization/tInputStream.h"
#include "core/port/net/tNetPort.h"
#include "core/port/tAbstractPort.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"
#include "rrlib/serialization/tOutputStream.h"

namespace finroc
{
namespace core
{
tPullCall::tPullCall() :
    tAbstractCall(),
    intermediate_assign(false),
    cc_pull(false),
    port(NULL)
{
  Reset();
}

void tPullCall::Deserialize(rrlib::serialization::tInputStream& is)
{
  ::finroc::core::tAbstractCall::Deserialize(is);
  intermediate_assign = is.ReadBoolean();
  cc_pull = is.ReadBoolean();
}

void tPullCall::ExecuteTask()
{
  assert((port != NULL));
  {
    util::tLock lock2(port->GetPort());
    if (!port->GetPort()->IsReady())
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG, log_domain, "pull call received for port that will soon be deleted");
      Recycle();
    }

    if (tFinrocTypeInfo::IsCCType(port->GetPort()->GetDataType()))
    {
      tCCPortBase* cp = static_cast<tCCPortBase*>(port->GetPort());
      tCCPortDataManager* cpd = cp->GetPullInInterthreadContainerRaw(true, true);
      RecycleParameters();

      tPortDataPtr<rrlib::serialization::tGenericObject> tmp(cpd->GetObject(), cpd);
      AddParam(0, tmp);

      SetStatusReturn();
      port->SendCallReturn(this);
    }
    else if (tFinrocTypeInfo::IsStdType(port->GetPort()->GetDataType()))
    {
      tPortBase* p = static_cast<tPortBase*>(port->GetPort());
      tPortDataManager* pd = p->GetPullLockedUnsafe(true, true);
      RecycleParameters();

      tPortDataPtr<rrlib::serialization::tGenericObject> tmp(pd->GetObject(), pd);
      AddParam(0, tmp);

      SetStatusReturn();
      port->SendCallReturn(this);
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "pull call received for port with invalid data type");
      Recycle();
    }
  }
}

void tPullCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  ::finroc::core::tAbstractCall::Serialize(oos);
  oos.WriteBoolean(intermediate_assign);
  oos.WriteBoolean(cc_pull);
}

} // namespace finroc
} // namespace core

