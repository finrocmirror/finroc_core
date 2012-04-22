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
#include "rrlib/finroc_core_utils/tTime.h"
#include "rrlib/finroc_core_utils/tGarbageCollector.h"
#include "rrlib/finroc_core_utils/stream/tChunkedBuffer.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/util/patterns/singleton.h"

#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "core/tLinkEdge.h"
#include "core/port/tAbstractPort.h"
#include "core/tRuntimeSettings.h"
#include "core/datatype/tUnit.h"
#include "core/datatype/tNumber.h"
#include "core/plugin/tPlugins.h"
#include "core/tFrameworkElementTreeFilter.h"

namespace finroc
{
namespace core
{
typedef rrlib::util::tSingletonHolder<tRuntimeEnvironment, rrlib::util::singleton::Longevity> tRuntimeEnvironmentInstance;
static inline unsigned int GetLongevity(tRuntimeEnvironment*)
{
  return 1; // delete before (almost) any other singleton
}

tRuntimeEnvironment* tRuntimeEnvironment::instance_raw_ptr = NULL;
bool tRuntimeEnvironment::active = false;
util::tMutexLockOrder tRuntimeEnvironment::static_class_mutex(tLockOrderLevels::cFIRST);

tRuntimeEnvironment::tRegistry::tRegistry() :
  ports(new tCoreRegister<tAbstractPort*>(true)),
  elements(false),
  link_edges(),
  listeners(),
  temp_buffer(),
  alternative_link_roots(),
  obj_mutex(tLockOrderLevels::cRUNTIME_REGISTER)
{}

tRuntimeEnvironment::tRuntimeEnvironment() :
  tFrameworkElement(NULL, "Runtime", tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cIS_RUNTIME, tLockOrderLevels::cRUNTIME_ROOT),
  registry(),
  creation_time(util::tTime::GetPrecise()),
  command_line_args(),
  unrelated(NULL)
{
  active = true;
}

tRuntimeEnvironment::~tRuntimeEnvironment()
{
  active = false;
  util::tThread::StopThreads();

  // delete all children - (runtime settings last)
  tFrameworkElement::tChildIterator ci(this);
  ::finroc::core::tFrameworkElement* next = NULL;
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


void tRuntimeEnvironment::AddLinkEdge(const util::tString& link, tLinkEdge* edge)
{
  FINROC_LOG_PRINT_TO(edges, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Adding link edge connecting to ", link);
  {
    util::tLock lock2(registry);
    if (registry.link_edges.find(link) == registry.link_edges.end())
    {
      // add first edge
      registry.link_edges[link] = edge;
    }
    else
    {
      // insert edge
      tLinkEdge* interested = registry.link_edges[link];
      tLinkEdge* next = interested->GetNext();
      interested->SetNext(edge);
      edge->SetNext(next);
    }

    // directly notify link edge?
    tAbstractPort* p = GetPort(link);
    if (p != NULL)
    {
      edge->LinkAdded(this, link, p);
    }
  }
}

void tRuntimeEnvironment::AddListener(tRuntimeListener* listener)
{
  util::tLock lock2(registry);
  registry.listeners.Add(listener);
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
  ::finroc::core::tFrameworkElement* fe = handle >= 0 ? registry.ports->Get(handle) : registry.elements.Get(handle);
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
  util::tLock lock2(registry);

  ::finroc::core::tFrameworkElement* fe = GetChildElement(link_name, false);
  if (fe == NULL)
  {
    for (size_t i = 0u; i < registry.alternative_link_roots.Size(); i++)
    {
      ::finroc::core::tFrameworkElement* alt_root = registry.alternative_link_roots.Get(i);
      fe = alt_root->GetChildElement(link_name, 0, true, alt_root);
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
  util::tLock lock1(static_class_mutex);
  assert((!ShuttingDown()));
  rrlib::rtti::tDataType<std::string>("String"); // Make sure std::string data type has name "String" - as in Java

  // Finish initializing static members of classes
  tUnit::StaticInit();  // can safely be done first
  tConstant::StaticInit();  // needs to be done after unit
  util::tTime::GetInstance();  // (possibly) init timing thread
  util::tGarbageCollector::CreateAndStartInstance();

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

void tRuntimeEnvironment::MarkElementDeleted(tFrameworkElement* fe)
{
  util::tLock lock2(registry);
  if (fe->IsPort())
  {
    registry.ports->MarkDeleted(fe->GetHandle());
  }
  else
  {
    registry.elements.MarkDeleted(fe->GetHandle());
  }
}

void tRuntimeEnvironment::PreElementInit(tFrameworkElement* element)
{
  util::tLock lock2(registry);
  registry.listeners.Notify(element, NULL, tRuntimeListener::cPRE_INIT);
}

int tRuntimeEnvironment::RegisterElement(tFrameworkElement* fe)
{
  util::tLock lock2(registry);
  return fe->IsPort() ? registry.ports->Add(static_cast<tAbstractPort*>(fe)) : registry.elements.Add(fe);
}

void tRuntimeEnvironment::RemoveLinkEdge(const util::tString& link, tLinkEdge* edge)
{
  util::tLock lock2(registry);
  tLinkEdge* current = registry.link_edges[link];
  if (current == edge)
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
      if (current == edge)
      {
        prev->SetNext(current->GetNext());
        return;
      }
      prev = current;
      current = current->GetNext();
    }
    FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_WARNING, "warning: Could not remove link edge for link: ", link);
  }
}

void tRuntimeEnvironment::RemoveLinkEdge(const util::tString& link, tAbstractPort* partner_port)
{
  util::tLock lock2(registry);
  for (tLinkEdge* current = registry.link_edges[link]; current != NULL; current = current->GetNext())
  {
    if (current->GetPortHandle() == partner_port->GetHandle())
    {
      delete current;
      return;
    }
  }
}

void tRuntimeEnvironment::RemoveListener(tRuntimeListener* listener)
{
  util::tLock lock2(registry);
  registry.listeners.Remove(listener);
}

void tRuntimeEnvironment::RuntimeChange(int8 change_type, tFrameworkElement* element, tAbstractPort* edge_target)
{
  util::tLock lock2(registry);
  if (!ShuttingDown())
  {
    if (element->GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT))
    {
      if (change_type == tRuntimeListener::cADD)
      {
        registry.alternative_link_roots.Add(element);
      }
      else if (change_type == tRuntimeListener::cREMOVE)
      {
        registry.alternative_link_roots.RemoveElem(element);
      }
    }

    if (change_type == tRuntimeListener::cADD && element->IsPort())    // check links
    {
      tAbstractPort* ap = static_cast<tAbstractPort*>(element);
      for (size_t i = 0u; i < ap->GetLinkCount(); i++)
      {
        ap->GetQualifiedLink(registry.temp_buffer, i);
        util::tString s = registry.temp_buffer;
        FINROC_LOG_PRINT_TO(edges, rrlib::logging::eLL_DEBUG_VERBOSE_2, "Checking link ", s, " with respect to link edges");

        if (registry.link_edges.find(s) != registry.link_edges.end())
        {
          tLinkEdge* le = registry.link_edges[s];
          while (le != NULL)
          {
            le->LinkAdded(this, s, ap);
            le = le->GetNext();
          }
        }
      }

    }

    registry.listeners.Notify(element, edge_target, change_type);
  }
}

void tRuntimeEnvironment::Shutdown()
{
  util::tThread::StopThreads();
  if (active)
  {
    //instance.reset();
  }
}

void tRuntimeEnvironment::UnregisterElement(tFrameworkElement* fe)
{
  util::tLock lock2(registry);
  if (fe->IsPort())
  {
    registry.ports->Remove(fe->GetHandle());
  }
  else
  {
    registry.elements.Remove(fe->GetHandle());
  }
}

} // namespace finroc
} // namespace core

