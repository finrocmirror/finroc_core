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
#include "core/internal/tLinkEdge.h"
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

tRuntimeEnvironment* tRuntimeEnvironment::instance_raw_ptr = NULL;
bool tRuntimeEnvironment::active = false;

//----------------------------------------------------------------------
// tRuntimeEnvironment constructors
//----------------------------------------------------------------------
tRuntimeEnvironment::tRuntimeEnvironment() :
  tFrameworkElement(NULL, "Runtime", tFlag::RUNTIME),
  elements(),
  link_edges(),
  global_link_edges(),
  runtime_listeners(),
  temp_buffer(),
  alternative_link_roots(),
  structure_mutex("Runtime Registry", static_cast<int>(tLockOrderLevel::RUNTIME_REGISTER)),
  creation_time(rrlib::time::Now()),
  command_line_args(),
  special_runtime_elements()
{
  special_runtime_elements.fill(NULL);
  active = true;
}

//----------------------------------------------------------------------
// tRuntimeEnvironment destructor
//----------------------------------------------------------------------
tRuntimeEnvironment::~tRuntimeEnvironment()
{
  global_link_edges.clear();
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
  instance_raw_ptr = NULL;
}

void tRuntimeEnvironment::AddLinkEdge(const tString& link, internal::tLinkEdge& edge)
{
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Adding link edge connecting to ", link);
  {
    tLock lock(structure_mutex);
    if (link_edges.find(link) == link_edges.end())
    {
      // add first edge
      link_edges[link] = &edge;
    }
    else
    {
      // insert edge
      internal::tLinkEdge* interested = link_edges[link];
      internal::tLinkEdge* next = interested->GetNextEdge();
      interested->SetNextEdge(&edge);
      edge.SetNextEdge(next);
    }

    // directly notify link edge?
    tAbstractPort* p = GetPort(link);
    if (p && p->IsReady())
    {
      edge.LinkAdded(*this, link, *p);
    }
  }
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
  return fe->IsReady() ? fe : NULL;
}

tRuntimeEnvironment& tRuntimeEnvironment::GetInstance()
{
  if (instance_raw_ptr == NULL)
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
  if (p == NULL)
  {
    return NULL;
  }
  return p->IsReady() ? static_cast<tAbstractPort*>(p) : NULL;
}

tAbstractPort* tRuntimeEnvironment::GetPort(const tString& link_name)
{
  tLock lock(structure_mutex);

  tFrameworkElement* fe = GetChildElement(link_name, false);
  if (fe == NULL)
  {
    for (auto it = alternative_link_roots.begin(); it != alternative_link_roots.end(); ++it)
    {
      fe = (*it)->GetChildElement(link_name, 0, true, **it);
      if (fe != NULL && !fe->IsDeleted() && fe->IsPort())
      {
        return static_cast<tAbstractPort*>(fe);
      }
    }
    return NULL;
  }
  if (!fe->IsPort())
  {
    return NULL;
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

tRuntimeEnvironment::tHandle tRuntimeEnvironment::RegisterElement(tFrameworkElement& fe, bool port)
{
  tLock lock(structure_mutex);
  return elements.Add(fe, port);
}

void tRuntimeEnvironment::RemoveLinkEdge(const tString& link, internal::tLinkEdge& edge)
{
  tLock lock(structure_mutex);
  internal::tLinkEdge* current = link_edges[link];
  if (current == &edge)
  {
    if (current->GetNextEdge() == NULL)    // remove entries for this link completely
    {
      link_edges.erase(link);
    }
    else    // remove first element
    {
      link_edges[link] = current->GetNextEdge();
    }
  }
  else    // remove element out of linked list
  {
    internal::tLinkEdge* prev = current;
    current = current->GetNextEdge();
    while (current != NULL)
    {
      if (current == &edge)
      {
        prev->SetNextEdge(current->GetNextEdge());
        return;
      }
      prev = current;
      current = current->GetNextEdge();
    }
    FINROC_LOG_PRINT(DEBUG_WARNING, "Could not remove link edge for link: ", link);
  }
}

void tRuntimeEnvironment::RemoveListener(tRuntimeListener& listener)
{
  tLock lock(structure_mutex);
  runtime_listeners.Remove(&listener);
}

void tRuntimeEnvironment::RuntimeChange(tRuntimeListener::tEvent change_type, tFrameworkElement& element, tAbstractPort* edge_target)
{
  tLock lock(structure_mutex);
  if (!ShuttingDown())
  {
    if (element.GetFlag(tFlag::ALTERNATIVE_LINK_ROOT))
    {
      if (change_type == tRuntimeListener::tEvent::ADD)
      {
        alternative_link_roots.push_back(&element);
      }
      else if (change_type == tRuntimeListener::tEvent::REMOVE)
      {
        alternative_link_roots.erase(std::remove(alternative_link_roots.begin(), alternative_link_roots.end(), &element), alternative_link_roots.end());
      }
    }

    if (change_type == tRuntimeListener::tEvent::ADD && element.IsPort())    // check links
    {
      tAbstractPort& ap = static_cast<tAbstractPort&>(element);
      for (size_t i = 0u; i < ap.GetLinkCount(); i++)
      {
        ap.GetQualifiedLink(temp_buffer, i);
        tString s = temp_buffer;
        FINROC_LOG_PRINT(DEBUG_VERBOSE_2, "Checking link ", s, " with respect to link edges");

        if (link_edges.find(s) != link_edges.end())
        {
          internal::tLinkEdge* le = link_edges[s];
          while (le)
          {
            le->LinkAdded(*this, s, ap);
            le = le->GetNextEdge();
          }
        }
      }
      if (ap.link_edges)
      {
        for (auto it = ap.link_edges->begin(); it != ap.link_edges->end(); ++it)
        {
          if ((*it)->GetSourceLink().length() > 0)
          {
            tAbstractPort* source = GetPort((*it)->GetSourceLink());
            if (source)
            {
              (*it)->LinkAdded(*this, (*it)->GetSourceLink(), *source);
            }
          }
          if ((*it)->GetTargetLink().length() > 0)
          {
            tAbstractPort* target = GetPort((*it)->GetTargetLink());
            if (target)
            {
              (*it)->LinkAdded(*this, (*it)->GetTargetLink(), *target);
            }
          }
        }
      }
    }

    if (edge_target)
    {
      assert(element.IsPort());
      for (auto it = runtime_listeners.Begin(); it != runtime_listeners.End(); ++it)
      {
        (*it)->OnEdgeChange(change_type, static_cast<tAbstractPort&>(element), *edge_target);
      }
    }
    else
    {
      for (auto it = runtime_listeners.Begin(); it != runtime_listeners.End(); ++it)
      {
        (*it)->OnFrameworkElementChange(change_type, element);
      }
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
