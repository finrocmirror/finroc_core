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
#include "rrlib/finroc_core_utils/log/tLogUser.h"

#include "core/port/rpc/tPullCall.h"
#include "core/port/tAbstractPort.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
tPullCall::tPullCall() :
  tAbstractCall(),
  intermediate_assign(false),
  cc_pull(false),
  port(NULL),
  return_to(NULL)
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
    util::tLock lock2(port);
    if (!port->IsReady())
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "pull call received for port that will soon be deleted");
      Recycle();
    }

    if (tFinrocTypeInfo::IsCCType(port->GetDataType()))
    {
      tCCPortBase* cp = static_cast<tCCPortBase*>(port);
      tCCPortDataManager* cpd = cp->GetPullInInterthreadContainerRaw(true, true);
      RecycleParameters();

      tPortDataPtr<rrlib::rtti::tGenericObject> tmp(cpd->GetObject(), cpd);
      AddParam(0, tmp);

      SetStatusReturn();
      return_to->InvokeCall(this);
    }
    else if (tFinrocTypeInfo::IsStdType(port->GetDataType()))
    {
      tPortBase* p = static_cast<tPortBase*>(port);
      tPortDataManager* pd = p->GetPullLockedUnsafe(true, true);
      RecycleParameters();

      tPortDataPtr<rrlib::rtti::tGenericObject> tmp(pd->GetObject(), pd);
      AddParam(0, tmp);

      SetStatusReturn();
      return_to->InvokeCall(this);
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "pull call received for port with invalid data type");
      Recycle();
    }
  }
}

void tPullCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  finroc::core::tAbstractCall::Serialize(oos);
  oos.WriteBoolean(intermediate_assign);
  oos.WriteBoolean(cc_pull);
}

} // namespace finroc
} // namespace core

