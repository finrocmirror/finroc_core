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
/*!\file    core/tRuntimeEnvironment.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tThread.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tLockOrderLevel.h"
#include "core/tRuntimeSettings.h"
#include "core/internal/tGarbageDeleter.h"
#include "core/internal/tLocalUriConnector.h"
#include "core/internal/tPlugins.h"
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
typedef rrlib::thread::tLock tLock;

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
typedef rrlib::design_patterns::tSingletonHolder<tRuntimeEnvironment, rrlib::design_patterns::singleton::Longevity> tRuntimeEnvironmentInstance;
static inline unsigned int GetLongevity(tRuntimeEnvironment*)
{
  return 1; // delete before (almost) any other singleton
}

tRuntimeEnvironment* tRuntimeEnvironment::instance_raw_ptr = nullptr;
bool tRuntimeEnvironment::active = false;

//----------------------------------------------------------------------
// tRuntimeEnvironment constructors
//----------------------------------------------------------------------
tRuntimeEnvironment::tRuntimeEnvironment() :
  tOwner(nullptr, "Runtime", tFlag::RUNTIME),
  elements(),
  registered_connectors(),
  runtime_listeners(),
  temp_path(),
  alternative_uri_roots(),
  structure_mutex("Runtime Registry", static_cast<int>(tLockOrderLevel::RUNTIME_REGISTER)),
  creation_time(rrlib::time::Now()),
  command_line_args(),
  special_runtime_elements()
{
  special_runtime_elements.fill(nullptr);
  active = true;
}

//----------------------------------------------------------------------
// tRuntimeEnvironment destructor
//----------------------------------------------------------------------
tRuntimeEnvironment::~tRuntimeEnvironment()
{
  ClearUriConnectors();
  active = false;
  rrlib::thread::tThread::StopThreads();

  // delete all children - (runtime settings last)
  for (auto it = this->ChildrenBegin(); it != this->ChildrenEnd(); ++it)
  {
    if (&(*it) != &GetElement(tSpecialRuntimeElement::RUNTIME_NODE))
    {
      it->ManagedDelete();
    }
  }
  GetElement(tSpecialRuntimeElement::RUNTIME_NODE).ManagedDelete();
  instance_raw_ptr = nullptr;
}

void tRuntimeEnvironment::AddListener(tRuntimeListener& listener)
{
  tLock lock(structure_mutex);
  runtime_listeners.Add(&listener);
}

size_t tRuntimeEnvironment::GetAllElements(tFrameworkElement** result_buffer, size_t max_elements, tHandle start_from_handle)
{
  tLock lock(structure_mutex);
  return elements.GetAllElements(result_buffer, max_elements, start_from_handle);
}

size_t tRuntimeEnvironment::GetAllPorts(tAbstractPort** result_buffer, size_t max_ports, tHandle start_from_handle)
{
  if (start_from_handle < internal::tFrameworkElementRegister::cFIRST_PORT_HANDLE)
  {
    start_from_handle = internal::tFrameworkElementRegister::cFIRST_PORT_HANDLE;
  }
  return GetAllElements(reinterpret_cast<tFrameworkElement**>(result_buffer), max_ports, start_from_handle);
}

tString tRuntimeEnvironment::GetCommandLineArgument(const tString& name)
{
  if (command_line_args.find(name) != command_line_args.end())
  {
    return command_line_args[name];
  }
  return "";
}

tFrameworkElement* tRuntimeEnvironment::GetElement(tHandle handle)
{
  if (handle == this->GetHandle())
  {
    return this;
  }
  tFrameworkElement* fe = elements.Get(handle);
  if (fe == NULL)
  {
    return NULL;
  }
  return fe->IsReady() ? fe : nullptr;
}

tRuntimeEnvironment& tRuntimeEnvironment::GetInstance()
{
  if (instance_raw_ptr == nullptr)
  {
    InitialInit();
  }
  assert(instance_raw_ptr);
  return *instance_raw_ptr;
}

tAbstractPort* tRuntimeEnvironment::GetPort(tHandle port_handle)
{
  if (port_handle < 0x80000000)
  {
    throw std::runtime_error("No port handle");
  }
  tFrameworkElement* p = elements.Get(port_handle);
  if (!p)
  {
    return nullptr;
  }
  return p->IsReady() ? static_cast<tAbstractPort*>(p) : nullptr;
}

tAbstractPort* tRuntimeEnvironment::GetPort(const tPath& path)
{
  tLock lock(structure_mutex);

  tFrameworkElement* fe = GetChild(path);
  if (!fe)
  {
    for (auto it = alternative_uri_roots.begin(); it != alternative_uri_roots.end(); ++it)
    {
      fe = (*it)->GetChild(path, 0, **it);
      if (fe && !fe->IsDeleted() && fe->IsPort())
      {
        return static_cast<tAbstractPort*>(fe);
      }
    }
    return nullptr;
  }
  if (!fe->IsPort())
  {
    return nullptr;
  }
  return static_cast<tAbstractPort*>(fe);
}

void tRuntimeEnvironment::InitialInit()
{
  static rrlib::thread::tOrderedMutex init_mutex("tRuntimeEnvironment::InitialInit()", 0);
  tLock lock1(init_mutex);
  assert((!ShuttingDown()));

  // Finish initializing static members of classes
  //tUnit::StaticInit();  // can safely be done first
  //tConstant::StaticInit();  // needs to be done after unit
#ifndef RRLIB_SINGLE_THREADED
  internal::tGarbageDeleter::CreateAndStartInstance();
#endif

  instance_raw_ptr = &tRuntimeEnvironmentInstance::Instance(); // should be done before any ports/elements are added

  // add special runtime elements
  instance_raw_ptr->special_runtime_elements[(size_t)tSpecialRuntimeElement::UNRELATED] = new tFrameworkElement(instance_raw_ptr, "Unrelated");
  instance_raw_ptr->special_runtime_elements[(size_t)tSpecialRuntimeElement::RUNTIME_NODE] = new tFrameworkElement(instance_raw_ptr, "Runtime");
  instance_raw_ptr->special_runtime_elements[(size_t)tSpecialRuntimeElement::SERVICES] = new tFrameworkElement(&instance_raw_ptr->GetElement(tSpecialRuntimeElement::RUNTIME_NODE), "Services");
  tRuntimeSettings::StaticInit();  // can be done now... or last
  instance_raw_ptr->special_runtime_elements[(size_t)tSpecialRuntimeElement::SETTINGS] = &tRuntimeSettings::GetInstance();
  tFrameworkElement::InitAll();
  assert(instance_raw_ptr->GetHandle() == 0);

  //Load plugins
  internal::tPlugins::StaticInit();
  tFrameworkElement::InitAll();
}

void tRuntimeEnvironment::PreElementInit(tFrameworkElement& element)
{
  tLock lock(structure_mutex);
  for (auto it = runtime_listeners.Begin(); it != runtime_listeners.End(); ++it)
  {
    (*it)->OnFrameworkElementChange(tRuntimeListener::tEvent::PRE_INIT, element);
  }
}

void tRuntimeEnvironment::RegisterConnector(const tPath& path, internal::tLocalUriConnector& connector)
{
  tLock lock(structure_mutex);
  registered_connectors[path].push_back(&connector);

  // directly notify link edge?
  tAbstractPort* p = GetPort(path);
  if (p && p->IsReady())
  {
    connector.OnPortAdded(*this, path, *p);
  }
}


tRuntimeEnvironment::tHandle tRuntimeEnvironment::RegisterElement(tFrameworkElement& fe, bool port)
{
  tLock lock(structure_mutex);
  return elements.Add(fe, port);
}

void tRuntimeEnvironment::RemoveListener(tRuntimeListener& listener)
{
  tLock lock(structure_mutex);
  runtime_listeners.Remove(&listener);
}

void tRuntimeEnvironment::RuntimeChange(tRuntimeListener::tEvent change_type, tFrameworkElement& element)
{
  tLock lock(structure_mutex);
  if (!ShuttingDown())
  {
    if (element.GetFlag(tFlag::ALTERNATIVE_LOCAL_URI_ROOT))
    {
      if (change_type == tRuntimeListener::tEvent::ADD)
      {
        alternative_uri_roots.push_back(&element);
      }
      else if (change_type == tRuntimeListener::tEvent::REMOVE)
      {
        alternative_uri_roots.erase(std::remove(alternative_uri_roots.begin(), alternative_uri_roots.end(), &element), alternative_uri_roots.end());
      }
    }

    if (change_type == tRuntimeListener::tEvent::ADD && element.IsPort())    // check links
    {
      tAbstractPort& port = static_cast<tAbstractPort&>(element);
      for (size_t i = 0u; i < port.GetLinkCount(); i++)
      {
        port.GetPath(temp_path, i);
        auto connector_list = registered_connectors.find(temp_path);
        if (connector_list != registered_connectors.end())
        {
          std::vector<internal::tLocalUriConnector*> connector_list_copy = connector_list->second;  // important: a copy is made here, since vector may be changed by tByStringConnector::OnLinkAdded()
          for (internal::tLocalUriConnector * connector : connector_list_copy)
          {
            connector->OnPortAdded(*this, temp_path, port);
          }
        }
      }
      if (port.UriConnectors().size())
      {
        size_t size = port.UriConnectors().size();
        tUriConnector* list_copy[size]; // important: a copy is made here, since vector may be changed by tHighLevelConnector::Disconnect()
        for (size_t i = 0; i < size; i++)
        {
          list_copy[i] = port.UriConnectors()[i].get();
        }

        for (size_t i = 0; i < size; i++)
        {
          if (list_copy[i] && typeid(*list_copy[i]) == typeid(internal::tLocalUriConnector))
          {
            internal::tLocalUriConnector& connector = static_cast<internal::tLocalUriConnector&>(*list_copy[i]);
            for (size_t i = 0; i < 2 && (!connector.GetConnection()); i++)
            {
              const tPath& path = connector.GetPortReferences()[i].path;
              if (path.Size())
              {
                tAbstractPort* port2 = GetPort(path);
                if (port2)
                {
                  connector.OnPortAdded(*this, path, *port2);
                }
              }
            }
          }
        }
      }
    }

    for (auto it = runtime_listeners.Begin(); it != runtime_listeners.End(); ++it)
    {
      (*it)->OnFrameworkElementChange(change_type, element);
    }
  }
}

void tRuntimeEnvironment::Shutdown()
{
  rrlib::thread::tThread::StopThreads();
  if (active)
  {
    //instance.reset();
  }
}

bool tRuntimeEnvironment::ShuttingDown()
{
  return rrlib::thread::tThread::StoppingThreads();
}

void tRuntimeEnvironment::UnregisterConnector(const tPath& path, internal::tLocalUriConnector& connector)
{
  tLock lock(structure_mutex);
  bool removed = false;
  auto vector = registered_connectors[path];
  for (auto it = vector.begin(); it != vector.end(); ++it)
  {
    if ((*it) == &connector)
    {
      vector.erase(it);
      removed = true;
      break;
    }
  }

  if (vector.size() == 0)
  {
    registered_connectors.erase(path);
  }

  if (!removed)
  {
    FINROC_LOG_PRINT(ERROR, "Could not remove local URI connection for path: ", path);
  }
}

void tRuntimeEnvironment::UnregisterElement(tFrameworkElement& fe)
{
  tLock lock(structure_mutex);
  elements.Remove(fe.GetHandle());
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
