/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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
  port(NULL),
  return_to(NULL),
  desired_encoding(rrlib::serialization::tDataEncoding::BINARY),
  pulled_buffer()
{
}

void tPullCall::Deserialize(rrlib::serialization::tInputStream& is)
{
  tAbstractCall::Deserialize(is);
  intermediate_assign = is.ReadBoolean();
  is >> desired_encoding;
  if (IsReturning(false))
  {
    pulled_buffer = tCallParameter::Lock(rrlib::rtti::ReadObject(is, this, desired_encoding));
  }
}

void tPullCall::ExecuteTask(tSerializableReusableTask::tPtr& self)
{
  tPullCall::tPtr& self2 = reinterpret_cast<tPullCall::tPtr&>(self);
  assert(port);
  {
    util::tLock lock2(*port);
    if (!port->IsReady())
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, "pull call received for port that will soon be deleted");
      return;
    }
    assert(!pulled_buffer);

    if (tFinrocTypeInfo::IsCCType(port->GetDataType()))
    {
      tCCPortBase* cp = static_cast<tCCPortBase*>(port);
      tCCPortDataManager* cpd = cp->GetPullInInterthreadContainerRaw(true, true);
      pulled_buffer = tPortDataPtr<rrlib::rtti::tGenericObject>(cpd);
      SetStatusReturn();
      return_to->InvokeCall(self2);
    }
    else if (tFinrocTypeInfo::IsStdType(port->GetDataType()))
    {
      tPortBase* p = static_cast<tPortBase*>(port);
      tPortDataManager* pd = p->GetPullLockedUnsafe(true, true);
      pulled_buffer = tPortDataPtr<rrlib::rtti::tGenericObject>(pd);
      SetStatusReturn();
      return_to->InvokeCall(self2);
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "pull call received for port with invalid data type");
      return;
    }
  }
}

void tPullCall::Recycle()
{
  intermediate_assign = false;
  port = NULL;
  return_to = NULL;
  desired_encoding = rrlib::serialization::tDataEncoding::BINARY;
  pulled_buffer.reset();
  tAbstractCall::Recycle();
}

void tPullCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  tAbstractCall::Serialize(oos);
  oos.WriteBoolean(intermediate_assign);
  oos << desired_encoding;
  if (IsReturning(false))
  {
    rrlib::rtti::WriteObject(oos, pulled_buffer.get(), desired_encoding);
  }
}

const util::tString tPullCall::ToString() const
{
  std::ostringstream os;
  os << "PullCall (" << GetStatusString() << ", callid: " << ::finroc::core::tAbstractCall::GetMethodCallIndex() << ", threaduid: " << ::finroc::core::tAbstractCall::GetThreadUid() << ")";
  return os.str();
}

} // namespace finroc
} // namespace core

