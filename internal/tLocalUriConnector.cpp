//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/internal/tLocalUriConnector.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2015-12-22
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tLocalUriConnector.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

namespace
{
static class tLocalSchemeHandler : public tUriConnector::tSchemeHandler
{
public:
  tLocalSchemeHandler() : tSchemeHandler("", tUriConnector::tConstParameterDefinitionRange())
  {}

  virtual bool Create(core::tAbstractPort& owner_port, const rrlib::uri::tURI& uri, const rrlib::uri::tURIElements& uri_elements, const tUriConnectOptions& connect_options) override
  {
    throw std::runtime_error("LocalSchemeHandler::Create() should not be called");
  }
} cLOCAL_SCHEME_HANDLER;
}

void CheckConnectionFlags(const tConnectionFlags& flags)
{
  if (flags.Get(tConnectionFlag::DISCONNECTED))
  {
    throw rrlib::util::tTraceableException<std::invalid_argument>("Disconnected flag must not be set");
  }
  if (flags.Get(tConnectionFlag::DIRECTION_TO_DESTINATION) && flags.Get(tConnectionFlag::DIRECTION_TO_SOURCE))
  {
    throw rrlib::util::tTraceableException<std::invalid_argument>("DIRECTION_TO_DESTINATION and DIRECTION_TO_SOURCE must not be set at the same time");
  }
}

//----------------------------------------------------------------------
// tLocalUriConnector constructors
//----------------------------------------------------------------------
tLocalUriConnector::tLocalUriConnector(const tPortReference& port1, const tPath& port2, const tConnectOptions& connect_options, tOwner& owner, tConnector* existing_connection) :
  tUriConnector(owner, port2, connect_options, cLOCAL_SCHEME_HANDLER),
  ports({{ tPortReference(port1), tPortReference(port2) }}),
connection(existing_connection)
{
  if (existing_connection)
  {
    assert(existing_connection->Flags().Get(tConnectionFlag::NON_PRIMARY_CONNECTOR));
    connection->NotifyOnDisconnect(*this);
  }
}

//----------------------------------------------------------------------
// tLocalUriConnector destructor
//----------------------------------------------------------------------
tLocalUriConnector::~tLocalUriConnector()
{
}

bool tLocalUriConnector::Create(const tPortReference& port1, const tPath& port2, const tConnectOptions& connect_options, tOwner& owner, tConnector* existing_connection)
{
  // Check/init parameters
  CheckConnectionFlags(connect_options.flags);

  // Check manager list for duplicates - and then add new connector
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  for (auto & raw_connector : owner.UriConnectors())
  {
    if (raw_connector && typeid(*raw_connector) == typeid(tLocalUriConnector))
    {
      tLocalUriConnector& connector = static_cast<tLocalUriConnector&>(*raw_connector);
      if ((connector.ports[0] == port1 && connector.ports[1] == port2) || (connector.ports[1] == port1 && connector.ports[0] == port2))
      {
        return false;
      }
    }
  }

  tLocalUriConnector* result = new tLocalUriConnector(port1, port2, connect_options, owner, existing_connection);

  // Register at runtime to listen for URIs
  auto& runtime = tRuntimeEnvironment::GetInstance();
  for (size_t i = 0; i < 2; i++)
  {
    if (result->ports[i].path.Size())
    {
      runtime.RegisterConnector(result->ports[i].path, *result);
    }
  }
  result->Publish();

  return true;
}

bool tLocalUriConnector::IsOutgoingConnector(core::tAbstractPort& owning_port) const
{
  tConnectionFlags connect_direction_flags = Flags() & (tConnectionFlag::DIRECTION_TO_DESTINATION | tConnectionFlag::DIRECTION_TO_SOURCE);
  if (connect_direction_flags.Raw()) // direction specified?
  {
    return (connect_direction_flags & tConnectionFlag::DIRECTION_TO_DESTINATION).Raw();
  }
  if (connection)
  {
    return &connection->Source() == &owning_port;
  }
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  return owning_port.InferConnectDirection(ports[1].path) == tAbstractPort::tConnectDirection::TO_DESTINATION;
}

void tLocalUriConnector::OnDisconnect()
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  for (size_t i = 0; i < 2; i++)
  {
    if (ports[i].path.Size() > 0)
    {
      tRuntimeEnvironment::GetInstance().UnregisterConnector(ports[i].path, *this);
    }
    flags.Set(tConnectionFlag::DISCONNECTED, true);
    if (connection && (connection->Flags().Get(tConnectionFlag::NON_PRIMARY_CONNECTOR)))
    {
      connection->Disconnect();
    }
    connection = nullptr;
  }
}

void tLocalUriConnector::OnConnectorDisconnect(bool stop_any_reconnecting)
{
  connection = nullptr;
  SetStatus(tStatus::DISCONNECTED);
  if (stop_any_reconnecting)
  {
    this->Disconnect();
  }
}


void tLocalUriConnector::OnPortAdded(tRuntimeEnvironment& runtime, const tPath& path, tAbstractPort& port)
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());

  if (connection || flags.Get(tConnectionFlag::DISCONNECTED))
  {
    return;
  }

  for (size_t i = 0; i < 2; i++)
  {
    if (path == ports[i].path)
    {
      const tPortReference& other = ports[i == 0 ? 1 : 0];
      tAbstractPort* other_port = other.path.Size() > 0 ? runtime.GetPort(other.path) : other.pointer;
      if (other_port)
      {
        bool reconnect = flags.Get(tConnectionFlag::RECONNECT) || port.GetFlag(tFrameworkElementFlag::VOLATILE) || (other.path.Size() && other_port->GetFlag(tFrameworkElementFlag::VOLATILE));
        const tConnectionFlags cKEEP_FLAGS = tConnectionFlag::FINSTRUCTED | tConnectionFlag::OPTIONAL | tConnectionFlag::SCHEDULING_NEUTRAL;
        tConnectionFlags connection_flags = tConnectionFlags(cKEEP_FLAGS.Raw() & flags.Raw());

        if (reconnect)
        {
          connection_flags |= tConnectionFlag::NON_PRIMARY_CONNECTOR;
        }
        connection = port.ConnectTo(*other_port, tConnectOptions(conversion_operations, connection_flags));
        if (connection)
        {
          SetStatus(tStatus::CONNECTED);
          if (reconnect)
          {
            connection->NotifyOnDisconnect(*this);
          }
          else
          {
            this->Disconnect();
          }
        }
        else
        {
          SetStatus(tStatus::ERROR);
        }
      }
    }
  }
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
