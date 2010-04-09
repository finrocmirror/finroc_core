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

namespace finroc
{
namespace core
{
tExternalConnection::tExternalConnection(const util::tString& description, const util::tString& default_address) :
    tFrameworkElement(description, tRuntimeEnvironment::GetInstance(), tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cNETWORK_ELEMENT),
    last_address(default_address),
    connected(false),
    listener(),
    first_connect(true)
{
}

void tExternalConnection::Connect(const util::tString& address)
{
  util::tLock lock1(obj_synch);

  ConnectImpl(address, (!first_connect) && address.Equals(last_address));
  PostConnect(address);
}

void tExternalConnection::Disconnect()
{
  util::tLock lock1(obj_synch);
  try
  {
    DisconnectImpl();
  }
  catch (const util::tException& e)
  {
    e.PrintStackTrace();

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
  util::tLock lock1(obj_synch);
  try
  {
    Disconnect();
  }
  catch (const util::tException& e)
  {
    e.PrintStackTrace();
  }
  ::finroc::core::tFrameworkElement::PrepareDelete();
}

} // namespace finroc
} // namespace core

