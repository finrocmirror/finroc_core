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
#include "core/port/net/tNetPort.h"

#include "core/admin/tAdminClient.h"
#include "core/admin/tAdminServer.h"
#include "core/port/rpc/tMethodCallException.h"

namespace finroc
{
namespace core
{
tAdminClient::tAdminClient(const util::tString& description, tFrameworkElement* parent) :
    tInterfaceClientPort(description, parent, tAdminServer::cDATA_TYPE)
{
}

void tAdminClient::Connect(tNetPort* np1, tNetPort* np2)
{
  if (np1 != NULL && np2 != NULL && np1->GetAdminInterface() == this && np2->GetAdminInterface() == this)
  {
    try
    {
      tAdminServer::cCONNECT.Call(this, np1->GetRemoteHandle(), np2->GetRemoteHandle(), false);
      return;
    }
    catch (const tMethodCallException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
  FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Connecting remote ports failed");
}

void tAdminClient::Disconnect(tNetPort* np1, tNetPort* np2)
{
  if (np1 != NULL && np2 != NULL && np1->GetAdminInterface() == this && np2->GetAdminInterface() == this)
  {
    try
    {
      tAdminServer::cDISCONNECT.Call(this, np1->GetRemoteHandle(), np2->GetRemoteHandle(), false);
      return;
    }
    catch (const tMethodCallException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
  FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Disconnecting remote ports failed");
}

void tAdminClient::DisconnectAll(tNetPort* np1)
{
  if (np1 != NULL && np1->GetAdminInterface() == this)
  {
    try
    {
      tAdminServer::cDISCONNECT_ALL.Call(this, np1->GetRemoteHandle(), false);
      return;
    }
    catch (const tMethodCallException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
  FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Disconnecting remote port failed");
}

void tAdminClient::SetRemotePortValue(tNetPort* np, tCCInterThreadContainer<>* container)
{
  if (np != NULL && np->GetAdminInterface() == this)
  {
    try
    {
      tAdminServer::cSET_PORT_VALUE.Call(this, np->GetRemoteHandle(), container, NULL, false);
      return;
    }
    catch (const tMethodCallException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
  FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Setting value of remote port failed");
}

void tAdminClient::SetRemotePortValue(tNetPort* np, tPortData* port_data)
{
  if (np != NULL && np->GetAdminInterface() == this)
  {
    try
    {
      tAdminServer::cSET_PORT_VALUE.Call(this, np->GetRemoteHandle(), NULL, port_data, false);
      return;
    }
    catch (const tMethodCallException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
  FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Setting value of remote port failed");
}

} // namespace finroc
} // namespace core

