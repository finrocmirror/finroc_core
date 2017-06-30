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
/*!\file    core/port/tUriConnector.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-16
 *
 */
//----------------------------------------------------------------------
#include "core/port/tUriConnector.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "core/internal/tLocalUriConnector.h"
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

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
const std::vector<std::unique_ptr<tUriConnector>> tUriConnector::tOwner::cEMPTY_CONNECTOR_LIST;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

namespace internal
{
/*!
 * \return List with available scheme handlers
 */
static tUriConnector::tSchemeHandlerRegister& SchemeHandlers()
{
  static tUriConnector::tSchemeHandlerRegister scheme_handlers;
  return scheme_handlers;
}
}

tUriConnector::tUriConnector(tOwner& owner, const tURI& uri, const tConnectOptions& connect_options, const tSchemeHandler& scheme_handler) :
  flags(tConnectOptions::UnsetAutoFlags(connect_options.flags) | (connect_options.conversion_operations.Size() ? tFlags(tFlag::CONVERSION) : tFlags())),
  owner(owner),
  conversion_operations(connect_options.conversion_operations),
  uri(uri),
  scheme_handler(scheme_handler),
  status(tStatus::DISCONNECTED)
{
  if (!owner.connectors)
  {
    owner.connectors.reset(new std::vector<std::unique_ptr<tUriConnector>>());
  }

  for (auto & connector_entry : (*owner.connectors))
  {
    if (!connector_entry)
    {
      connector_entry.reset(this);
      return;
    }
  }
  owner.connectors->emplace_back(this);
}

tUriConnector::~tUriConnector()
{}

bool tUriConnector::Create(core::tAbstractPort& owner_port, const tURI& uri, const tUriConnectOptions& connect_options)
{
  rrlib::uri::tURIElements uri_elements;
  uri.Parse(uri_elements);
  if (uri_elements.scheme.length() == 0)
  {
    return internal::tLocalUriConnector::Create(owner_port, uri_elements.path, connect_options, owner_port);
  }

  for (tSchemeHandler * handler : internal::SchemeHandlers())
  {
    if (uri_elements.scheme == handler->scheme_name)
    {
      return handler->Create(owner_port, uri, uri_elements, connect_options);
    }
  }
  throw std::runtime_error("No scheme handler for scheme '" + uri_elements.scheme + "' registered.");
}

void tUriConnector::Publish()
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  if (!flags.Get(tFlag::PUBLISHED))
  {
    flags.Set(tFlag::PUBLISHED);
    auto& runtime = tRuntimeEnvironment::GetInstance();
    for (auto it = runtime.runtime_listeners.Begin(); it != runtime.runtime_listeners.End(); ++it)
    {
      (*it)->OnUriConnectorChange(tRuntimeListener::tEvent::ADD, *this);
    }
  }
}

const tUriConnector::tSchemeHandlerRegister& tUriConnector::GetSchemeHandlerRegister()
{
  return internal::SchemeHandlers();
}

bool tUriConnector::SetParameter(size_t index, const rrlib::rtti::tTypedConstPointer& new_value)
{
  auto parameter_definitions = scheme_handler.GetParameterDefinitions();
  if (static_cast<long int>(index) >= (parameter_definitions.End() - parameter_definitions.Begin()))
  {
    FINROC_LOG_PRINT(WARNING, "Index out of bounds");
    return false;
  }
  const rrlib::rtti::tParameterDefinition* parameter_definition = parameter_definitions.Begin() + index;
  if (parameter_definition->IsStatic())
  {
    FINROC_LOG_PRINT(WARNING, "Static parameter cannot be changed");
    return false;
  }
  auto parameter_value = GetParameterValues().Begin() + index;
  if (parameter_value->GetType() != new_value.GetType())
  {
    FINROC_LOG_PRINT(WARNING, "New parameter value has incompatible type");
    return false;
  }

  bool equals = new_value.Equals(*parameter_value);
  if (!equals)
  {
    rrlib::rtti::tTypedPointer writable_parameter(const_cast<void*>(parameter_value->GetRawDataPointer()), parameter_value->GetType());
    writable_parameter.DeepCopyFrom(new_value);
  }
  return (!equals);
}

void tUriConnector::SetParametersInConstructor(const core::tUriConnectOptions& connect_options)
{
  auto parameter_definitions = scheme_handler.GetParameterDefinitions();
  for (auto & parameter : connect_options.parameters)
  {
    bool found = false;
    for (auto it = parameter_definitions.begin(); it != parameter_definitions.end(); ++it)
    {
      if (parameter.first == it->GetName())
      {
        try
        {
          rrlib::serialization::tStringInputStream stream(parameter.second);
          auto parameter_value = GetParameterValues().Begin() + (it - parameter_definitions.begin());
          rrlib::rtti::tTypedPointer writable_parameter(const_cast<void*>(parameter_value->GetRawDataPointer()), parameter_value->GetType());
          writable_parameter.Deserialize(stream);
        }
        catch (const std::exception& e)
        {
          FINROC_LOG_PRINT(WARNING, "Failed setting parameter '", parameter.first, "' to '", parameter.second, "': ", e);
        }
        found = true;
        break;
      }
    }
    if (!found)
    {
      FINROC_LOG_PRINT(WARNING, "Unknown parameter ", parameter.first, ". Ignoring.");
    }
  }
}

void tUriConnector::SetStatus(tStatus new_status)
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  if (new_status != status)
  {
    status = new_status;
    assert(flags.Get(tFlag::PUBLISHED));
    auto& runtime = tRuntimeEnvironment::GetInstance();
    for (auto it = runtime.runtime_listeners.Begin(); it != runtime.runtime_listeners.End(); ++it)
    {
      (*it)->OnUriConnectorChange(tRuntimeListener::tEvent::CHANGE, *this);
    }
  }
}

tUriConnector::tOwner::~tOwner()
{
  ClearUriConnectors();
}

void tUriConnector::tOwner::ClearUriConnectors()
{
  if (connectors)
  {
    for (auto & connector : (*connectors))
    {
      connector->OnDisconnect();
    }
    connectors.reset();
  }
}


void tUriConnector::tOwner::Disconnect(tUriConnector& connector)
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  connector.OnDisconnect();

  if (connectors)
  {
    for (auto & connector_entry : (*connectors))
    {
      if (connector_entry.get() == &connector)
      {
        if (connector.Flags().Get(tUriConnector::tFlag::PUBLISHED))
        {
          auto& runtime = tRuntimeEnvironment::GetInstance();
          for (auto it = runtime.runtime_listeners.Begin(); it != runtime.runtime_listeners.End(); ++it)
          {
            (*it)->OnUriConnectorChange(tRuntimeListener::tEvent::REMOVE, connector);
          }
        }

        connector_entry.reset();
        return;
      }
    }
  }
}

tUriConnector::tSchemeHandler::tSchemeHandler(const char* scheme_name, const tConstParameterDefinitionRange& parameter_definitions) :
  scheme_name(scheme_name),
  parameter_definitions(parameter_definitions),
  handle(static_cast<uint8_t>(internal::SchemeHandlers().Add(this)))
{
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
