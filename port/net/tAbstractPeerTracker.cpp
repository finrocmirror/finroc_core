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
#include "core/port/net/tAbstractPeerTracker.h"
#include "core/tLockOrderLevels.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
const int8 tAbstractPeerTracker::cDISCOVERED, tAbstractPeerTracker::cREMOVED;
std::shared_ptr<util::tSimpleListWithMutex<tAbstractPeerTracker*>> tAbstractPeerTracker::instances(new util::tSimpleListWithMutex<tAbstractPeerTracker*>(tLockOrderLevels::cINNER_MOST - 1));

tAbstractPeerTracker::tAbstractPeerTracker(int lock_order) :
  tRecursiveMutex("Abstract Peer Tracker", lock_order),
  instances_lock(instances),
  listeners()
{
  rrlib::thread::tLock lock2(*instances);
  instances->Add(this);
}

void tAbstractPeerTracker::AddListener(tListener& listener)
{
  // make sure: listener list can only be modified, while there aren't any other connection events being processed
  rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance()->GetRegistryLock());
  rrlib::thread::tLock lock3(*this);
  listeners.AddListener(listener);
}

tAbstractPeerTracker::~tAbstractPeerTracker()
{
  rrlib::thread::tLock lock2(*instances_lock);
  instances_lock->RemoveElem(this);
}

void tAbstractPeerTracker::RegisterServer(const util::tString& network_name, const util::tString& name, int port)
{
  rrlib::thread::tLock lock2(*instances);
  for (size_t i = 0u; i < instances->Size(); i++)
  {
    instances->Get(i)->RegisterServerImpl(network_name, name, port);
  }
}

void tAbstractPeerTracker::RemoveListener(tListener& listener)
{
  // make sure: listener list can only be modified, while there aren't any other connection events being processed
  rrlib::thread::tLock lock2(tRuntimeEnvironment::GetInstance()->GetRegistryLock());
  rrlib::thread::tLock lock3(*this);
  listeners.RemoveListener(listener);
}

void tAbstractPeerTracker::UnregisterServer(const util::tString& network_name, const util::tString& name)
{
  rrlib::thread::tLock lock2(*instances);
  for (size_t i = 0u; i < instances->Size(); i++)
  {
    instances->Get(i)->UnregisterServerImpl(network_name, name);
  }
}

} // namespace finroc
} // namespace core

