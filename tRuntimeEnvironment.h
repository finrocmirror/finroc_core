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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__TRUNTIMEENVIRONMENT_H
#define CORE__TRUNTIMEENVIRONMENT_H

#include "core/tCoreRegister.h"
#include "core/port/tAbstractPort.h"
#include "core/tFrameworkElement.h"
#include "finroc_core_utils/container/tConcurrentMap.h"
#include "core/tRuntimeListener.h"
#include "finroc_core_utils/container/tSimpleList.h"

namespace finroc
{
namespace core
{
class tLinkEdge;
class tThreadLocalCache;

/*!
 * \author Max Reichardt
 *
 * This is an application's main central class (root object). It contains groups and modules.
 */
class tRuntimeEnvironment : public tFrameworkElement
{
  friend class tLinkEdge;
private:

  //
  //  // Static elements to delete after all elements in runtime environment
  //  std::tr1::shared_ptr<util::SimpleList<finroc::util::Object*> > deleteLastList;
  //

  /*! Loop threads in this runtime */
  //private final SimpleList<CoreLoopThread> loopThreads = new SimpleList<CoreLoopThread>();

  /*! Event threads in this runtime */
  //private final SimpleList<CoreEventThread> eventThreads = new SimpleList<CoreEventThread>();

  /*! Global list of all ports (also place holders for those that are currently not available) */
  //private final Map<String, PortContainer> ports = new FastMap<String, PortContainer>();

  /*!
   * Global list of all ports (also place holders for those that are currently not available)
   * allows accessing ports with simple index. This index is unique during runtime.
   * List will grow... somewhat memory leak like... but still with 100.000 different port uids it's only 400kb
   *
   * Thread-safe for iteration.
   */
  //private final FastTable<PortContainer> indexedPorts = new FastTable<PortContainer>();

  /*! List with Ports that actually exists - thread-safe for iteration - may contain null entries */
  //private final SafeArrayList<Port<?>> existingPorts = new SafeArrayList<Port<?>>();

  /*! Global list of all modules (also place holders for those that are currently not available) */
  //private final FastMap<String, ModuleContainer> modules = new FastMap<String, ModuleContainer>();

  /*! Number of active modules */
  //private int activeModuleCount;

  /*! All edges in runtime environment */
  //private final List<Edge> edges = new ArrayList<Edge>();

  /*! Flexible edges in runtime environment */
  //private final List<FlexEdge> flexEdges = new ArrayList<FlexEdge>();

  /*! Singleton instance of Runtime environment - shared pointer so that is cleanly deleted at shutdown */
  static ::std::tr1::shared_ptr<tRuntimeEnvironment> instance;

  /*! Raw pointer to above - that also exists during destruction */
  static tRuntimeEnvironment* instance_raw_ptr;

  /*! Runtime settings */
  //private final RuntimeSettings settings;

  /*! Runtime listeners and Parameters */
  //private final ListenerManager listeners = new ListenerManager();
  //private final Byte ADD = 1, REMOVE = 2;

  /*! Global register of all ports. Allows accessing ports with simple handle. */
  ::std::tr1::shared_ptr<tCoreRegister<tAbstractPort*> > ports;

  /*! Global register of all framework elements (except of ports) */
  tCoreRegister< ::finroc::core::tFrameworkElement*> elements;

  //  /** Links to framework elements */
  //  private final ConcurrentMap<String, AbstractPort> links = new ConcurrentMap<String, AbstractPort>();

  /*! Edges dealing with linked ports */
  util::tConcurrentMap<util::tString, tLinkEdge*> link_edges;

  /*! List with runtime listeners */
  tRuntimeListenerManager listeners;

  //  /** True, when Runtime environment is shutting down */
  //  public static boolean shuttingDown = false;

  /*! Temporary buffer - may be used in synchronized context */
  util::tStringBuilder temp_buffer;

  /*! Timestamp when runtime environment was created */
  int64 creation_time;

  /*! Is RuntimeEnvironment currently active (and needs to be deleted?) */
  static bool active;

  /*! Lock to thread local cache list */
  ::std::tr1::shared_ptr<util::tSimpleList<tThreadLocalCache*> > infos_lock;

  /*! Alternative roots for links (usually remote runtime environments mapped into this one) */
  util::tSimpleList< ::finroc::core::tFrameworkElement*> alternative_link_roots;

public:

  // for static synchronization in this class' methods
  static util::tMutex static_obj_synch;

  /*! Framework element that contains all framework elements that have no parent specified */
  ::finroc::core::tFrameworkElement* unrelated;

private:

  //  @SuppressWarnings("unused")
  //  @InCppFile
  //  private void stopStreamThread() {
  //    StreamCommitThread.staticStop();
  //  }

  //@Init("deleteLastList(new util::SimpleList<finroc::util::Object*>())")
  tRuntimeEnvironment();

protected:

  //  /**
  //   * (Should only be called by AbstractPort)
  //   * Create link to port
  //   *
  //   * \param port Port
  //   * \param linkName Name of link
  //   */
  //  public synchronized void link(AbstractPort port, String linkName) {
  //    assert(!links.contains(linkName));
  //    links.put(linkName, port);
  //
  //    // notify link listeners
  //
  //    for (@SizeT int i = 0; i < listeners.size(); i++) {
  //      listeners.get(i).linkAdded(linkName, port);
  //    }
  //
  //    // notify edges
  //    LinkEdge interested = linkEdges.getPtr(linkName);
  //    while(interested != null) {
  //      interested.linkAdded(this, linkName, port);
  //    }
  //  }

  //  /**
  //   * (Should only be called by AbstractPort)
  //   * Remove link to port
  //   *
  //   * \param linkName Name of link
  //   */
  //  public synchronized void removeLink(String linkName) {
  //    AbstractPort ap = links.remove(linkName);
  //
  //    // notify link listeners
  //    for (@SizeT int i = 0; i < linkListeners.size(); i++) {
  //      linkListeners.get(i).linkRemoved(linkName, ap);
  //    }
  //  }

  /*!
   * (usually only called by LinkEdge)
   * Add link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void AddLinkEdge(const util::tString& link, tLinkEdge* edge);

  /*!
   * (usually only called by LinkEdge)
   * Remove link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void RemoveLinkEdge(const util::tString& link, tLinkEdge* edge);

  /*!
   * Remove linked edges from specified link to specified partner port
   *
   * \param link Link
   * \param partner_port connected port
   */
  void RemoveLinkEdge(const util::tString& link, tAbstractPort* partner_port);

public:

  /*!
   * Add runtime listener
   *
   * \param listener Listener to add
   */
  inline void AddListener(tRuntimeListener* listener)
  {
    util::tLock lock2(obj_synch);
    listeners.Add(listener);
  }

  /*!
   * \return Timestamp when runtime environment was created
   */
  inline int64 GetCreationTime()
  {
    return creation_time;
  }

  /*!
   * Get Framework element from handle
   *
   * \param handle Handle of framework element
   * \return Pointer to framework element - or null if it has been deleted
   */
  inline ::finroc::core::tFrameworkElement* GetElement(int handle)
  {
    ::finroc::core::tFrameworkElement* fe = handle >= 0 ? ports->Get(handle) : elements.Get(handle);
    if (fe == NULL)
    {
      return NULL;
    }
    return fe->IsReady() ? fe : NULL;
  }

  virtual ~tRuntimeEnvironment()
  {
    active = false;
    util::tThread::StopThreads();
    //shuttingDown = true;
    //util::GarbageCollector::deleteGarbageCollector(); // safer, this way
    DeleteChildren();
    instance_raw_ptr = NULL;

    //    // delete thread local caches mainly
    //    for (size_t i = 0; i < deleteLastList->size(); i++) {
    //      delete deleteLastList->get(i);
    //    }
    //    deleteLastList->clear();
    //    deleteLastList._reset();
    // stopStreamThread();
    // instance = NULL; will happen automatically
  }

  /*!
   * (IMPORTANT: This should not be called during static initialization)
   *
   * \return Singleton instance of Runtime environment
   */
  static tRuntimeEnvironment* GetInstance();

  //  /**
  //   * (should only be called by thread local-cache)
  //   * get port by raw index
  //   *
  //   * \param i Raw index
  //   * \return Port
  //   */
  //  public AbstractPort getPortByRawIndex(int i) {
  //    return ports.getByRawIndex(i);
  //  }

  /*!
   * get Port by handle
   *
   * \param port_handle port handle
   * \return Port - if port with such handle exists - otherwise null
   */
  tAbstractPort* GetPort(int port_handle);

  /*!
   * \param link_name (relative) Fully qualified name of port
   * \return Port with this name - or null if it does not exist
   */
  tAbstractPort* GetPort(const util::tString& link_name);

  /*!
   * (Should only be called by ThreadLocalCache class - needed for clean cleanup - port register needs to exists longer than runtime environment)
   * \return Port register
   */
  inline ::std::tr1::shared_ptr<const tCoreRegister<tAbstractPort*> > GetPorts()
  {
    return ports;
  }

  /*!
   * Initializes the runtime environment. Needs to be called before any
   * other operation (especially getInstance()) is called.
   *
   * @parem conffile Config file (see etc directory)
   * \return Singleton instance of Runtime environment
   */
  static tRuntimeEnvironment* InitialInit();

  //  /**
  //   * \return Iterator to iterate over links
  //   */
  //  public ConcurrentMap<String, AbstractPort>.MapIterator getLinkIterator() {
  //    return links.getIterator();
  //  }

  //  /**
  //   * Delete this object after all Framework elements
  //   */
  //  static void deleteLast(@Ptr Object t) {
  //
  //    if (instance == NULL || instance->deleteLastList._get() == NULL) {
  //      delete t;
  //    } else {
  //      instance->deleteLastList->add(t);
  //    }
  //
  //  }

  /*!
   * Called before a framework element is initialized - can be used to create links etc. to this element etc.
   *
   * \param element Framework element that will be initialized soon
   */
  inline void PreElementInit(tFrameworkElement* element)
  {
    listeners.Notify(element, NULL, tRuntimeListener::cPRE_INIT);
  }

  //  @Override
  //  protected void serializeUid(CoreOutputStream oos, boolean firstCall) throws IOException {
  //    return;  // do not include Runtime Description in UIDs
  //  }
  //
  //  /**
  //   * \return Returns unmodifiable list of Ports.
  //   */
  //  public List<PortContainer> getPorts() {
  //    return indexedPorts.unmodifiable();
  //  }
  //
  //  /**
  //   * \param index Port Index
  //   * \return Returns PortContainer with specified global index
  //   */
  //  public PortContainer getPort(int index) {
  //    return indexedPorts.get(index);
  //  }

  //  /**
  //   * \return Runtime Settings module
  //   */
  //  public RuntimeSettings getSettings() {
  //    return settings;
  //  }
  //
  //
  //  private class ListenerManager extends WeakRefListenerManager<RuntimeListener> {
  //
  //    @Override
  //    protected void notifyObserver(RuntimeListener observer, Object... param) {
  //      if (param[0] == ADD) {
  //        observer.portAdded((Port<?>)param[1]);
  //      } else {
  //        observer.portRemoved((Port<?>)param[1]);
  //      }
  //    }
  //  }
  //
  //  /**
  //   * \param l
  //   * @see core.util.WeakRefListenerManager#addListener(java.util.EventListener)
  //   */
  //  public synchronized void addListener(RuntimeListener l, boolean applyExistingPorts) {
  //    listeners.addListener(l);
  //    if (applyExistingPorts) {
  //      for (int i = 0, n = indexedPorts.size(); i < n; i++) {
  //        Port<?> p = indexedPorts.get(i).getPort();
  //        if (p != null) {
  //          l.portAdded(p);
  //        }
  //      }
  //    }
  //  }
  //
  //  /**
  //   * \param l
  //   * @see core.util.WeakRefListenerManager#removeListener(java.util.EventListener)
  //   */
  //  public synchronized void removeListener(RuntimeListener l) {
  //    listeners.removeListener(l);
  //  }
  //
  //  /**
  //   * \return Unmodifiable List with Ports that actually exists - thread-safe for iteration - may contain null entries
  //   */
  //  public List<Port<?>> getExistingPorts() {
  //    return existingPorts.getFastUnmodifiable();
  //  }
  //
  //  /**
  //   * \return the activeModuleCount
  //   */
  //  public int getActiveModuleCount() {
  //    return activeModuleCount;
  //  }
  //
  //  public void resetLoopThreads() {
  //      for (FastMap.Entry<String, ModuleContainer> e = modules.head(), end = modules.tail(); (e = e.getNext()) != end;) {
  //        e.getValue().setLoopThreadInfo(null);
  //      }
  //  }

  /*!
   * Register framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to register
   * \return Handle of Framework element
   */
  inline int RegisterElement(tFrameworkElement* fe)
  {
    util::tLock lock2(obj_synch);
    return fe->IsPort() ? ports->Add(static_cast<tAbstractPort*>(fe)) : elements.Add(fe);
  }

  /*!
   * Remove runtime listener
   *
   * \param listener Listener to remove
   */
  inline void RemoveListener(tRuntimeListener* listener)
  {
    util::tLock lock2(obj_synch);
    listeners.Remove(listener);
  }

  /*!
   * Called whenever a framework element was added/removed or changed
   *
   * \param change_type Type of change (see Constants in Transaction class)
   * \param element FrameworkElement that changed
   *
   * (Is called in synchronized (Runtime & Element) context in local runtime... so method should not block)
   * (should only be called by FrameworkElement class)
   */
  void RuntimeChange(int8 change_type, tFrameworkElement* element);

  /*!
   * Using only the basic constructs from this framework - things should shutdown
   * cleanly without calling anything.
   *
   * However, if things like the TCP server are used, this method should be called
   * at the end of the program in order to shut everything down cleanly.
   */
  static void Shutdown();

  /*!
   * (irrelevant for Java - we need no memory cleanup there)
   *
   * \return Is runtime environment currently shutting down?
   */
  inline static bool ShuttingDown()
  {
    return util::tThread::StoppingThreads();
  }

  /*!
   * Unregister framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to remove
   */
  void UnregisterElement(tFrameworkElement* fe);

  //! Can be placed in classes in order to ensure that RuntimeEnvironment will be deleted before its static members
  class tStaticDeleter
  {
  public:
    ~tStaticDeleter();
  };

};

} // namespace finroc
} // namespace core

#endif // CORE__TRUNTIMEENVIRONMENT_H
