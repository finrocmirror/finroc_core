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
#include "rrlib/finroc_core_utils/tGarbageDeleter.h"
#include "rrlib/finroc_core_utils/stream/tChunkedBuffer.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/design_patterns/singleton.h"

#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "core/tLinkEdge.h"
#include "core/port/tAbstractPort.h"
#include "core/tRuntimeSettings.h"
#include "core/datatype/tUnit.h"
#include "core/datatype/tNumber.h"
#include "core/plugin/tPlugins.h"

namespace finroc
{
namespace core
{
typedef rrlib::design_patterns::tSingletonHolder<tRuntimeEnvironment, rrlib::design_patterns::singleton::Longevity> tRuntimeEnvironmentInstance;
static inline unsigned int GetLongevity(tRuntimeEnvironment*)
{
  return 1; // delete before (almost) any other singleton
}

tRuntimeEnvironment* tRuntimeEnvironment::instance_raw_ptr = NULL;
bool tRuntimeEnvironment::active = false;
rrlib::thread::tOrderedMutex tRuntimeEnvironment::static_class_mutex("Runtime", tLockOrderLevels::cFIRST);

tRuntimeEnvironment::tRegistry::tRegistry() :
  ports(new tCoreRegister<tAbstractPort*>(true)),
  elements(false),
  link_edges(),
  listeners(),
  temp_buffer(),
  alternative_link_roots(),
  mutex("Runtime Registry", tLockOrderLevels::cRUNTIME_REGISTER)
{}

tRuntimeEnvironment::tRuntimeEnvironment() :
  tFrameworkElement(NULL, "Runtime", tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cIS_RUNTIME, tLockOrderLevels::cRUNTIME_ROOT),
  registry(),
  creation_time(rrlib::time::Now()),
  command_line_args(),
  unrelated(NULL)
{
  active = true;
}

tRuntimeEnvironment::~tRuntimeEnvironment()
{
  active = false;
  rrlib::thread::tThread::StopThreads();

  // delete all children - (runtime settings last)
  tFrameworkElement::tChildIterator ci(*this, false);
  tFrameworkElement* next = NULL;
  while ((next = ci.Next()) != NULL)
  {
    if (next != tRuntimeSettings::GetInstance())
    {
      next->ManagedDelete();
    }
  }
  tRuntimeSettings::GetInstance()->ManagedDelete();
  instance_raw_ptr = NULL;
}


void tRuntimeEnvironment::AddLinkEdge(const util::tString& link, tLinkEdge& edge)
{
  FINROC_LOG_PRINT_TO(edges, DEBUG_VERBOSE_1, "Adding link edge connecting to ", link);
  {
    tLock lock2(registry.mutex);
    if (registry.link_edges.find(link) == registry.link_edges.end())
    {
      // add first edge
      registry.link_edges[link] = &edge;
    }
    else
    {
      // insert edge
      tLinkEdge* interested = registry.link_edges[link];
      tLinkEdge* next = interested->GetNext();
      interested->SetNext(&edge);
      edge.SetNext(next);
    }

    // directly notify link edge?
    tAbstractPort* p = GetPort(link);
    if (p)
    {
      edge.LinkAdded(*this, link, *p);
    }
  }
}

void tRuntimeEnvironment::AddListener(tRuntimeListener& listener)
{
  tLock lock2(registry.mutex);
  registry.listeners.AddListener(listener);
}

util::tString tRuntimeEnvironment::GetCommandLineArgument(const util::tString& name)
{
  if (command_line_args.find(name) != command_line_args.end())
  {
    return command_line_args[name];
  }
  return "";
}

::finroc::core::tFrameworkElement* tRuntimeEnvironment::GetElement(int handle)
{
  if (handle == this->GetHandle())
  {
    return this;
  }
  tFrameworkElement* fe = handle >= 0 ? registry.ports->Get(handle) : registry.elements.Get(handle);
  if (fe == NULL)
  {
    return NULL;
  }
  return fe->IsReady() ? fe : NULL;
}

tRuntimeEnvironment* tRuntimeEnvironment::GetInstance()
{
  if (instance_raw_ptr == NULL)
  {
    InitialInit();
  }
  return instance_raw_ptr;
}

tAbstractPort* tRuntimeEnvironment::GetPort(int port_handle)
{
  tAbstractPort* p = registry.ports->Get(port_handle);
  if (p == NULL)
  {
    return NULL;
  }
  return p->IsReady() ? p : NULL;
}

tAbstractPort* tRuntimeEnvironment::GetPort(const util::tString& link_name)
{
  tLock lock2(registry.mutex);

  tFrameworkElement* fe = GetChildElement(link_name, false);
  if (fe == NULL)
  {
    for (size_t i = 0u; i < registry.alternative_link_roots.Size(); i++)
    {
      tFrameworkElement* alt_root = registry.alternative_link_roots.Get(i);
      fe = alt_root->GetChildElement(link_name, 0, true, *alt_root);
      if (fe != NULL && !fe->IsDeleted())
      {
        assert(fe->IsPort());
        return static_cast<tAbstractPort*>(fe);
      }
    }
    return NULL;
  }
  assert(fe->IsPort());
  return static_cast<tAbstractPort*>(fe);
}

void tRuntimeEnvironment::InitialInit()
{
  tLock lock1(static_class_mutex);
  assert((!ShuttingDown()));
  rrlib::rtti::tDataType<std::string>("String"); // Make sure std::string data type has name "String" - as in Java

  // Finish initializing static members of classes
  tUnit::StaticInit();  // can safely be done first
  tConstant::StaticInit();  // needs to be done after unit
  util::tGarbageDeleter::CreateAndStartInstance();

  instance_raw_ptr = &tRuntimeEnvironmentInstance::Instance(); // should be done before any ports/elements are added

  // add uninitialized child
  instance_raw_ptr->unrelated = new ::finroc::core::tFrameworkElement(instance_raw_ptr, "Unrelated");

  //ConfigFile.init(conffile);
  tRuntimeSettings::StaticInit();  // can be done now... or last
  ::finroc::core::tFrameworkElement::InitAll();

  //Load plugins
  tPlugins::StaticInit();
  ::finroc::core::tFrameworkElement::InitAll();
  //deleteLast(RuntimeSettings.getInstance());

  instance_raw_ptr->SetFlag(tCoreFlags::cREADY);
}

void tRuntimeEnvironment::MarkElementDeleted(tFrameworkElement& fe)
{
  tLock lock2(registry.mutex);
  if (fe.IsPort())
  {
    registry.ports->MarkDeleted(fe.GetHandle());
  }
  else
  {
    registry.elements.MarkDeleted(fe.GetHandle());
  }
}

void tRuntimeEnvironment::PreElementInit(tFrameworkElement& element)
{
  tLock lock2(registry.mutex);
  registry.listeners.Notify([&](tRuntimeListener & l)
  {
    l.RuntimeChange(tRuntimeListener::cPRE_INIT, element);
  });
}

int tRuntimeEnvironment::RegisterElement(tFrameworkElement& fe, bool port)
{
  tLock lock2(registry.mutex);
  return port ? registry.ports->Add(static_cast<tAbstractPort*>(&fe)) : registry.elements.Add(&fe);
}

void tRuntimeEnvironment::RemoveLinkEdge(const util::tString& link, tLinkEdge& edge)
{
  tLock lock2(registry.mutex);
  tLinkEdge* current = registry.link_edges[link];
  if (current == &edge)
  {
    if (current->GetNext() == NULL)    // remove entries for this link completely
    {
      registry.link_edges.erase(link);
    }
    else    // remove first element
    {
      registry.link_edges[link] = current->GetNext();
    }
  }
  else    // remove element out of linked list
  {
    tLinkEdge* prev = current;
    current = current->GetNext();
    while (current != NULL)
    {
      if (current == &edge)
      {
        prev->SetNext(current->GetNext());
        return;
      }
      prev = current;
      current = current->GetNext();
    }
    FINROC_LOG_PRINT_TO(framework_elements, DEBUG_WARNING, "warning: Could not remove link edge for link: ", link);
  }
}

void tRuntimeEnvironment::RemoveListener(tRuntimeListener& listener)
{
  tLock lock2(registry.mutex);
  registry.listeners.RemoveListener(listener);
}

void tRuntimeEnvironment::RuntimeChange(int8 change_type, tFrameworkElement& element, tAbstractPort* edge_target)
{
  tLock lock2(registry.mutex);
  if (!ShuttingDown())
  {
    if (element.GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT))
    {
      if (change_type == tRuntimeListener::cADD)
      {
        registry.alternative_link_roots.Add(&element);
      }
      else if (change_type == tRuntimeListener::cREMOVE)
      {
        registry.alternative_link_roots.RemoveElem(&element);
      }
    }

    if (change_type == tRuntimeListener::cADD && element.IsPort())    // check links
    {
      tAbstractPort& ap = static_cast<tAbstractPort&>(element);
      for (size_t i = 0u; i < ap.GetLinkCount(); i++)
      {
        ap.GetQualifiedLink(registry.temp_buffer, i);
        util::tString s = registry.temp_buffer;
        FINROC_LOG_PRINT_TO(edges, DEBUG_VERBOSE_2, "Checking link ", s, " with respect to link edges");

        if (registry.link_edges.find(s) != registry.link_edges.end())
        {
          tLinkEdge* le = registry.link_edges[s];
          while (le != NULL)
          {
            le->LinkAdded(*this, s, ap);
            le = le->GetNext();
          }
        }
      }
      for (size_t i = 0; ap.GetLinkEdges() && i < ap.GetLinkEdges()->Size(); i++)
      {
        tLinkEdge& e = *ap.GetLinkEdges()->Get(i);
        if (e.GetSourceLink().length() > 0)
        {
          tAbstractPort* source = GetPort(e.GetSourceLink());
          if (source)
          {
            e.LinkAdded(*this, e.GetSourceLink(), *source);
          }
        }
        if (e.GetTargetLink().length() > 0)
        {
          tAbstractPort* target = GetPort(e.GetTargetLink());
          if (target)
          {
            e.LinkAdded(*this, e.GetTargetLink(), *target);
          }
        }
      }
    }

    if (edge_target)
    {
      assert(element.IsPort());
      registry.listeners.Notify([&](tRuntimeListener & l)
      {
        l.RuntimeEdgeChange(change_type, static_cast<tAbstractPort&>(element), *edge_target);
      });
    }
    else
    {
      registry.listeners.Notify([&](tRuntimeListener & l)
      {
        l.RuntimeChange(change_type, element);
      });
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

void tRuntimeEnvironment::UnregisterElement(tFrameworkElement& fe)
{
  tLock lock2(registry.mutex);
  if (fe.IsPort())
  {
    registry.ports->Remove(fe.GetHandle());
  }
  else
  {
    registry.elements.Remove(fe.GetHandle());
  }
}

} // namespace finroc
} // namespace core

