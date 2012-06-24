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

#ifndef core__port__net__tAbstractPeerTracker_h__
#define core__port__net__tAbstractPeerTracker_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleListWithMutex.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/finroc_core_utils/tListenerManager.h"
#include "rrlib/finroc_core_utils/net/tIPSocketAddress.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for "peer trackers".
 * Peer trackers look for other systems on the network that can be connected to.
 */
class tAbstractPeerTracker : public util::tLogUser, public util::tMutexLockOrder
{
public:
  class tListener; // inner class forward declaration

private:

  /*! callIDs */
  static const int8 cDISCOVERED = 0, cREMOVED = 1;

  /*! Peer tracker instances that are used - can be multiple */
  static std::shared_ptr<util::tSimpleListWithMutex<tAbstractPeerTracker*> > instances;

  /*! "Lock" to above - for safe deinitialization */
  std::shared_ptr<util::tSimpleListWithMutex<tAbstractPeerTracker*> > instances_lock;

protected:

  /*! Tracker listeners */
  util::tListenerManager<tListener, util::tMutexLockOrder> listeners;

  /*!
   * Called by subclass when TCP node has been discovered
   *
   * \param isa Node's network address
   * \param name Node's name
   */
  inline void NotifyDiscovered(const util::tIPSocketAddress& isa, const util::tString& name)
  {
    listeners.Notify([&](tListener & l)
    {
      l.NodeDiscovered(isa, name);
    });
  }

  /*!
   * Called by subclass when TCP node has been stopped/removed/deleted
   *
   * \param isa Node's network address
   * \param name Node's name
   */
  inline void NotifyRemoved(const util::tIPSocketAddress& isa, const util::tString& name)
  {
    listeners.Notify([&](tListener & l)
    {
      l.NodeRemoved(isa, name);
    });
  }

  /*!
   * Register/publish server
   *
   * \param network_name Network name
   * \param name Name
   * \param port Port
   */
  inline void RegisterServerImpl(const util::tString& network_name, const util::tString& name, int port)
  {
  }

  /*!
   * UnRegister/unpublish server
   *
   * \param network_name Network name
   * \param name Name
   */
  inline void UnregisterServerImpl(const util::tString& network_name, const util::tString& name)
  {
  }

public:

  /*!
   * \param lock_order Lock order of tracker
   */
  tAbstractPeerTracker(int lock_order);

  /*!
   * \param listener Listener to add
   */
  void AddListener(tListener& listener);

  /*!
   * Delete tracker
   */
  virtual ~tAbstractPeerTracker();

  /*!
   * Register/publish server
   *
   * \param network_name Network name
   * \param name Name
   * \param port Port
   */
  void RegisterServer(const util::tString& network_name, const util::tString& name, int port);

  /*!
   * \param listener Listener to remove
   */
  void RemoveListener(tListener& listener);

  /*!
   * Unregister/unpublish server
   *
   * \param network_name Network name
   * \param name Name
   */
  void UnregisterServer(const util::tString& network_name, const util::tString& name);

public:

  /*!
   * Listens to discovery and removal of TCP nodes
   */
  class tListener : public util::tInterface
  {
  public:

    /*!
     * Called when TCP node has been discovered
     *
     * \param isa Node's network address
     * \param name Node's name
     */
    virtual void NodeDiscovered(const util::tIPSocketAddress& isa, const util::tString& name) = 0;

    /*!
     * Called when TCP node has been stopped/deleted
     *
     * \param isa Node's network address
     * \param name Node's name
     * \return Object to post-process without any locks (null if no post-processing necessary)
     *
     * (Called with runtime and Peer Tracker lock)
     */
    virtual util::tObject* NodeRemoved(const util::tIPSocketAddress& isa, const util::tString& name) = 0;

    /*!
     * Called when TCP node has been deleted - and object for post-processing has been returned in method above
     *
     * \param obj Object to post-process
     *
     * (Called without/after runtime and Peer Tracker lock)
     */
    virtual void NodeRemovedPostLockProcess(util::tObject* obj) = 0;

  };

};

} // namespace finroc
} // namespace core

#endif // core__port__net__tAbstractPeerTracker_h__
