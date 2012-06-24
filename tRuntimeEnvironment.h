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

#ifndef core__tRuntimeEnvironment_h__
#define core__tRuntimeEnvironment_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/container/tSimpleListWithMutex.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/util/patterns/singleton/policies/creation/CreateUsingNew.h"

#include "core/tCoreRegister.h"
#include "core/tFrameworkElement.h"
#include "core/tRuntimeListener.h"
#include "core/tLockOrderLevels.h"

namespace finroc
{
namespace core
{
class tThreadLocalCache;
class tLinkEdge;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * This is an application's main central class (root object). It contains groups and modules.
 */
class tRuntimeEnvironment : public tFrameworkElement
{
public:

  /*!
   * Contains diverse registers/lookup tables of runtime.
   * They are moved to extra class in order to be separately lockable
   * (necessary for systematic dead-lock avoidance)
   */
  class tRegistry : public boost::noncopyable
  {
    friend class tRuntimeEnvironment;
    friend class tFrameworkElement;

  private:

    /*! Global register of all ports. Allows accessing ports with simple handle. */
    std::shared_ptr<tCoreRegister<tAbstractPort*>> ports;

    /*! Global register of all framework elements (except of ports) */
    tCoreRegister<tFrameworkElement*> elements;

    /*! Edges dealing with linked ports */
    std::map<std::string, tLinkEdge*> link_edges;

    /*! List with runtime listeners */
    util::tListenerManager<tRuntimeListener, tMutexLockOrder> listeners;

    /*! Temporary buffer - may be used in synchronized context */
    std::string temp_buffer;

    /*! Alternative roots for links (usually remote runtime environments mapped into this one) */
    util::tSimpleList<tFrameworkElement*> alternative_link_roots;

    /*! Mutex */
    util::tMutexLockOrder mutex;

    tRegistry();
  };

  friend class tFrameworkElement;
  friend class tLinkEdge;
  friend class rrlib::util::singleton::CreateUsingNew<tRuntimeEnvironment>;
private:

  /*! Single final instance of above */
  tRegistry registry;

  /*! Raw pointer to above - that also exists during destruction */
  static tRuntimeEnvironment* instance_raw_ptr;

  /*! Timestamp when runtime environment was created */
  rrlib::time::tTimestamp creation_time;

  /*! Is RuntimeEnvironment currently active (and needs to be deleted?) */
  static bool active;

  /*! Mutex for static methods */
  static util::tMutexLockOrder static_class_mutex;

  /*! Command line arguments (used by parameters, for instance). Needs to be manually filled (usually in main()). */
  std::map<std::string, std::string> command_line_args;

  /*! Framework element that contains all framework elements that have no parent specified */
  ::finroc::core::tFrameworkElement* unrelated;

  tRuntimeEnvironment();

  virtual ~tRuntimeEnvironment();

  /*!
   * Initializes the runtime environment. Needs to be called before any
   * other operation (especially getInstance()) is called.
   */
  static void InitialInit();

  /*!
   * (usually only called by LinkEdge)
   * Add link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void AddLinkEdge(const util::tString& link, tLinkEdge* edge);

  /*!
   * Register framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to register
   * \param port Is framework element a port?
   * \return Handle of Framework element
   */
  int RegisterElement(tFrameworkElement* fe, bool port);

  /*!
   * (usually only called by LinkEdge)
   * Remove link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void RemoveLinkEdge(const util::tString& link, tLinkEdge* edge);

  /*!
   * Unregister framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to remove
   */
  void UnregisterElement(tFrameworkElement* fe);

public:

  /*!
   * (usually called from main())
   * Register any relevant command line arguments at runtime.
   * Can be retrieved by GetCommandLineArgument later.
   *
   * \param option Name of command line option.
   * \param value Value of option.
   */
  void AddCommandLineArgument(const util::tString& option, const util::tString& value)
  {
    command_line_args.insert(std::pair<std::string, std::string>(option, value));
  }

  /*!
   * Add runtime listener
   *
   * \param listener Listener to add
   */
  void AddListener(tRuntimeListener& listener);

  /*!
   * \return Timestamp when runtime environment was created
   */
  inline rrlib::time::tTimestamp GetCreationTime()
  {
    return creation_time;
  }

  /*/
   * \param name Name of command line argument
   * \return Value of argument - or "" if not set.
   */
  util::tString GetCommandLineArgument(const util::tString& name);

  /*!
   * Get Framework element from handle
   *
   * \param handle Handle of framework element
   * \return Pointer to framework element - or null if it has been deleted
   */
  ::finroc::core::tFrameworkElement* GetElement(int handle);

  /*!
   * (IMPORTANT: This should not be called during static initialization)
   *
   * \return Singleton instance of Runtime environment
   */
  static tRuntimeEnvironment* GetInstance();

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
  inline std::shared_ptr<const tCoreRegister<tAbstractPort*> > GetPorts()
  {
    return registry.ports;
  }

  /*!
   * \return Lock order of registry
   */
  inline const tRegistry* GetRegistryHelper()
  {
    return &(registry);
  }

  /*!
   * Mark element as (soon completely) deleted at RuntimeEnvironment
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to mark deleted
   */
  void MarkElementDeleted(tFrameworkElement* fe);

  /*!
   * Called before a framework element is initialized - can be used to create links etc. to this element etc.
   *
   * \param element Framework element that will be initialized soon
   */
  void PreElementInit(tFrameworkElement& element);

  /*!
   * Remove linked edges from specified link to specified partner port
   *
   * \param link Link
   * \param partner_port connected port
   */
  void RemoveLinkEdge(const util::tString& link, tAbstractPort* partner_port);

  /*!
   * Remove runtime listener
   *
   * \param listener Listener to remove
   */
  void RemoveListener(tRuntimeListener& listener);

  /*!
   * Called whenever a framework element was added/removed or changed
   *
   * \param change_type Type of change (see Constants in Transaction class)
   * \param element FrameworkElement that changed
   * \param edge_target Target of edge, in case of EDGE_CHANGE
   *
   * (Is called in synchronized (Runtime & Element) context in local runtime... so method should not block)
   * (should only be called by FrameworkElement class)
   */
  void RuntimeChange(int8 change_type, tFrameworkElement& element, tAbstractPort* edge_target = NULL);

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

};

} // namespace finroc
} // namespace core

#endif // core__tRuntimeEnvironment_h__
