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
#include "core/tLinkEdge.h"

#include "core/tRuntimeEnvironment.h"
#include "core/tCoreFlags.h"
#include "finroc_core_utils/tTime.h"
#include "core/datatype/tUnit.h"
#include "core/datatype/tConstant.h"
#include "core/port/rpc/tMethodCallSyncher.h"
#include "finroc_core_utils/container/tBoundedQElementContainer.h"
#include "finroc_core_utils/stream/tChunkedBuffer.h"
#include "core/port/stream/tStreamCommitThread.h"
#include "core/tRuntimeSettings.h"
#include "core/plugin/tPlugins.h"

namespace finroc
{
namespace core
{
tRuntimeEnvironment::tStaticDeleter::~tStaticDeleter()
{
  tRuntimeEnvironment::Shutdown();
}

util::tMutex tRuntimeEnvironment::static_obj_synch;
::std::tr1::shared_ptr<tRuntimeEnvironment> tRuntimeEnvironment::instance;
tRuntimeEnvironment* tRuntimeEnvironment::instance_raw_ptr = NULL;
bool tRuntimeEnvironment::active = false;

tRuntimeEnvironment::tRuntimeEnvironment() :
    tFrameworkElement("Runtime", NULL, tCoreFlags::cALLOWS_CHILDREN | tCoreFlags::cIS_RUNTIME),
    ports(new tCoreRegister<tAbstractPort*>(true)),
    elements(false),
    link_edges(NULL),
    listeners(),
    temp_buffer(),
    creation_time(util::tTime::GetPrecise()),
    infos_lock(),
    alternative_link_roots(),
    unrelated(NULL)
{
  assert(instance == NULL);
  instance = ::std::tr1::shared_ptr<tRuntimeEnvironment>(this);
  instance_raw_ptr = this;
  active = true;

  //    for (int i = 0; i < RuntimeSettings.NUM_OF_LOOP_THREADS; i++) {
  //      loopThreads.add(new CoreLoopThread(i, i < RuntimeSettings.SELF_UPDATING_LOOP_THREADS));
  //    }
  //    for (int i = 0; i < RuntimeSettings.NUM_OF_EVENT_THREADS; i++) {
  //      eventThreads.add(new CoreEventThread(i));
  //    }

  // init runtime settings
  //settings = new RuntimeSettings();
  //addChild(settings);

  // init Plugins etc.
  //Plugins.getInstance().addPluginsFromApplicationDir();
}

void tRuntimeEnvironment::AddLinkEdge(const util::tString& link, tLinkEdge* edge)
{
  util::tLock lock1(obj_synch);
  tLinkEdge* interested = link_edges.Get(link);
  if (interested == NULL)
  {
    // add first edge
    link_edges.Put(link, edge);
  }
  else
  {
    // insert edge
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

tRuntimeEnvironment* tRuntimeEnvironment::GetInstance()
{
  if (instance_raw_ptr == NULL)
  {
    //throw new RuntimeException("Runtime Environment not initialized");
    InitialInit();
  }
  return instance_raw_ptr;
}

tAbstractPort* tRuntimeEnvironment::GetPort(int port_handle)
{
  tAbstractPort* p = ports->Get(port_handle);
  if (p == NULL)
  {
    return NULL;
  }
  return p->IsReady() ? p : NULL;
}

tAbstractPort* tRuntimeEnvironment::GetPort(const util::tString& link_name)
{
  ::finroc::core::tFrameworkElement* fe = GetChildElement(link_name, false);
  if (fe == NULL)
  {
    for (size_t i = 0u; i < alternative_link_roots.Size(); i++)
    {
      ::finroc::core::tFrameworkElement* alt_root = alternative_link_roots.Get(i);
      fe = alt_root->GetChildElement(link_name, 0, true, alt_root);
      if (fe != NULL)
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

tRuntimeEnvironment* tRuntimeEnvironment::InitialInit()
{
  util::tLock lock1(static_obj_synch);
  assert((!ShuttingDown()));

  // Finish initializing static members of classes
  tUnit::StaticInit();  // can safely be done first
  tConstant::StaticInit();  // needs to be done after unit
  //    CoreNumber.staticInit(); // can be after data type register has been created
  util::tTime::GetInstance();  // (possibly) init timing thread
  //    MethodCall.staticInit();
  //    PullCall.staticInit();
  ::std::tr1::shared_ptr<util::tSimpleList<tThreadLocalCache*> > infos_lock = tThreadLocalCache::StaticInit();  // can safely be done first
  tMethodCallSyncher::StaticInit();  // dito
  util::tBoundedQElementContainer::StaticInit();
  util::tChunkedBuffer::StaticInit();
  tStreamCommitThread::StaticInit();
  //TransactionPacket.staticInit();
  //TransactionPacket.staticInit();

  new tRuntimeEnvironment(); // should be done before any ports/elements are added

  // Start thread - because it needs a thread local cache
  tStreamCommitThread::GetInstance()->Start();

  // add uninitialized child
  instance->unrelated = new ::finroc::core::tFrameworkElement("Unrelated", instance.get());
  ::finroc::core::tFrameworkElement::InitAll();

  //ConfigFile.init(conffile);
  tRuntimeSettings::StaticInit();  // can be done now... or last

  //Load plugins
  tPlugins::StaticInit();
  //deleteLast(RuntimeSettings.getInstance());

  instance->SetFlag(tCoreFlags::cREADY);

  return instance.get();
}

void tRuntimeEnvironment::RemoveLinkEdge(const util::tString& link, tLinkEdge* edge)
{
  util::tLock lock1(obj_synch);
  tLinkEdge* current = link_edges.Get(link);
  if (current == edge)
  {
    if (current->GetNext() == NULL)    // remove entries for this link completely
    {
      link_edges.Remove(link);
    }
    else    // remove first element
    {
      link_edges.Put(link, current->GetNext());
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
    util::tSystem::out.Println(util::tStringBuilder("warning: Could not remove link edge for link: ") + link);
  }
}

void tRuntimeEnvironment::RemoveLinkEdge(const util::tString& link, tAbstractPort* partner_port)
{
  util::tLock lock1(obj_synch);
  for (tLinkEdge* current = link_edges.Get(link); current != NULL; current = current->GetNext())
  {
    if (current->GetPortHandle() == partner_port->GetHandle())
    {
      delete current;
      return;
    }
  }
}

void tRuntimeEnvironment::RuntimeChange(int8 change_type, tFrameworkElement* element)
{
  if (!ShuttingDown())
  {
    if (element->GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT))
    {
      if (change_type == tRuntimeListener::cADD)
      {
        alternative_link_roots.Add(element);
      }
      else if (change_type == tRuntimeListener::cREMOVE)
      {
        alternative_link_roots.RemoveElem(element);
      }
    }

    if (change_type == tRuntimeListener::cADD && element->IsPort())    // check links
    {
      tAbstractPort* ap = static_cast<tAbstractPort*>(element);
      for (size_t i = 0u; i < ap->GetLinkCount(); i++)
      {
        ap->GetQualifiedLink(temp_buffer, i);
        util::tString s = temp_buffer.ToString();
        tLinkEdge* le = link_edges.Get(s);
        while (le != NULL)
        {
          le->LinkAdded(this, s, ap);
          le = le->GetNext();
        }
      }

    }

    listeners.Notify(element, NULL, change_type);
  }
}

void tRuntimeEnvironment::Shutdown()
{
  util::tThread::StopThreads();
  if (active)
  {
    instance.reset();
  }

}

void tRuntimeEnvironment::UnregisterElement(tFrameworkElement* fe)
{
  util::tLock lock1(obj_synch);
  if (fe->IsPort())
  {
    ports->Remove(fe->GetHandle());
  }
  else
  {
    elements.Remove(fe->GetHandle());
  }
}

} // namespace finroc
} // namespace core

