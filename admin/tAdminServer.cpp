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
#include "core/admin/tAdminServer.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/tCoreFlags.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
tPortInterface tAdminServer::cMETHODS("Admin Interface");
tVoid2Method<tAdminServer*, int, int> tAdminServer::cCONNECT(tAdminServer::cMETHODS, "Connect", "source port handle", "destination port handle", false);
tVoid2Method<tAdminServer*, int, int> tAdminServer::cDISCONNECT(tAdminServer::cMETHODS, "Disconnect", "source port handle", "destination port handle", false);
tVoid1Method<tAdminServer*, int> tAdminServer::cDISCONNECT_ALL(tAdminServer::cMETHODS, "DisconnectAll", "source port handle", false);
tDataType* tAdminServer::cDATA_TYPE = tDataTypeRegister::GetInstance()->AddMethodDataType("Administration method calls", &(tAdminServer::cMETHODS));
util::tString tAdminServer::cPORT_NAME = "Administration";
util::tString tAdminServer::cQUALIFIED_PORT_NAME = "Unrelated/Administration";

tAdminServer::tAdminServer() :
    tInterfaceServerPort(cPORT_NAME, NULL, cDATA_TYPE, NULL, tCoreFlags::cSHARED)
{
  SetCallHandler(this);
}

void tAdminServer::HandleVoidCall(const tAbstractMethod* method, int p1, int p2)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  ::finroc::core::tAbstractPort* src = re->GetPort(p1);
  ::finroc::core::tAbstractPort* dest = re->GetPort(p2);
  if (src == NULL || dest == NULL)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Can't (dis)connect ports that do not exists");
    return;
  }
  if (method == &(cCONNECT))
  {
    if (src->MayConnectTo(dest))
    {
      src->ConnectToTarget(dest);
    }
    else if (dest->MayConnectTo(src))
    {
      dest->ConnectToTarget(src);
    }
    if (!src->IsConnectedTo(dest))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, util::tStringBuilder("Could not connect ports "), src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, util::tStringBuilder("Connected ports "), src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
  else if (method == &(cDISCONNECT))
  {
    src->DisconnectFrom(dest);
    if (src->IsConnectedTo(dest))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, util::tStringBuilder("Could not disconnect ports "), src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, util::tStringBuilder("Disconnected ports "), src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
}

void tAdminServer::HandleVoidCall(const tAbstractMethod* method, int p1)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  ::finroc::core::tAbstractPort* src = re->GetPort(p1);
  if (src == NULL)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Can't disconnect port that doesn't exist");
    return;
  }
  src->DisconnectAll();
  FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, util::tStringBuilder("Disconnected port "), src->GetQualifiedName());
}

} // namespace finroc
} // namespace core

