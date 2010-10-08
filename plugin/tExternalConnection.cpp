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
#include "core/plugin/tExternalConnection.h"
#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "core/tLockOrderLevels.h"
#include "core/parameter/tStructureParameterList.h"
#include "finroc_core_utils/log/tLogUser.h"

namespace finroc
{
namespace core
{
tExternalConnection::tExternalConnection(const util::tString& description, const util::tString& default_address) :
    tFrameworkElement(description, tRuntimeEnvironment::GetInstance(), tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cNETWORK_ELEMENT, tLockOrderLevels::cLEAF_GROUP),
    last_address(default_address),
    connected(false),
    listener(),
    first_connect(true),
    auto_connect_to(new tStringStructureParameter("Autoconnect to", ""))
{
  tStructureParameterList::GetOrCreate(this)->Add(auto_connect_to);
}

void tExternalConnection::Connect(const util::tString& address)
{
  util::tLock lock1(this);

  ConnectImpl(address, (!first_connect) && address.Equals(last_address));
  PostConnect(address);
}

void tExternalConnection::Disconnect()
{
  util::tLock lock1(this);
  try
  {
    DisconnectImpl();
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);

  }
  FireConnectionEvent(tConnectionListener::cNOT_CONNECTED);
}

void tExternalConnection::PostConnect(const util::tString& address)
{
  last_address = address;
  first_connect = false;
  FireConnectionEvent(tConnectionListener::cCONNECTED);
}

void tExternalConnection::PrepareDelete()
{
  util::tLock lock1(this);
  try
  {
    Disconnect();
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
  }
  ::finroc::core::tFrameworkElement::PrepareDelete();
}

void tExternalConnection::StructureParametersChanged()
{
  util::tString s = auto_connect_to->Get();
  if (s.Length() > 0)
  {
    if (!s.Equals(last_address))
    {
      if (IsConnected())
      {
        try
        {
          Disconnect();
        }
        catch (const util::tException& e)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
        }
      }
      last_address = s;
    }
    if (!IsConnected())
    {
      try
      {
        Connect(s);
      }
      catch (const util::tException& e)
      {
        FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
      }
    }
  }
}

} // namespace finroc
} // namespace core

