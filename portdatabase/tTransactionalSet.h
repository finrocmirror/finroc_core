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

#ifndef CORE__PORTDATABASE__TTRANSACTIONALSET_H
#define CORE__PORTDATABASE__TTRANSACTIONALSET_H

#include "finroc_core_utils/container/tConcurrentMap.h"
#include "core/portdatabase/tTransactionalData.h"
#include "core/port/stream/tTransaction.h"

namespace finroc
{
namespace core
{
class tDataType;
class tTransactionPacket;
class tPortBase;

/*!
 * \author Max Reichardt
 *
 * This class implements a set of objects as TransactionalData.
 * It does not contain elements twice.
 *
 * It allows reading and modifying the set in different runtime environments concurrently.
 * The keys of the entries, however, need to be unique across the network.
 *
 * It handles pending commands (e.g. Remove command before the add command is received)
 */
template<typename K, typename B>
class tTransactionalSet : public tTransactionalData<B>
{
public:
  class tEntry; // inner class forward declaration

protected:

  /*! Wrapped map */
  util::tConcurrentMap<K, B> set;

  /*! For reading keys from buffer - reused */
  B temp_read_key;

  /*! List with commands that could not yet be executed */
  tSafeConcurrentlyIterableList<B, 4, true> pending_commands;

  /*! Time of Last check if pending commands can be executed */
  int64 last_pending_command_check;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_mutex;

  /*! Frequency of checks if pending commands can be executed */
  int cPENDING_COMMAND_CHECK_INTERVAL;

  /*! After this period pending commands will be ignored */
  int cPENDING_COMMAND_TIMEOUT;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "stream_ports");

protected:

  /*!
   * Commit transaction and handle any expcetions
   *
   * \param transaction Transaction to commit
   */
  void CommitTransaction(B transaction);

  /*!
   * \param add Entry to add
   */
  inline void IncomingAdd(B add)
  {
    util::tLock lock2(this);
    set.Put(add.GetKey(), add);
  }

  /*!
   * \param change Entry containing changes
   * \return Defer operation? (not possible yet)
   */
  bool IncomingChange(B change);

  /*!
   * \param remove Entry to remove (only needs to contain key) (will be reused so do not store)
   * \return Defer operation? (not possible yet)
   */
  inline bool IncomingRemove(B remove)
  {
    B removed = set.Remove(remove.GetKey());
    return removed == NULL;
  }

  void ProcessPendingCommands(int64 time);

  virtual void Update(int64 time)
  {
    ProcessPendingCommands(time);
  }

public:

  tTransactionalSet(const util::tString& description, bool input, bool output, bool local, tDataType* bclass, int commit_interval, bool input_shared, bool output_shared);

  /*!
   * Add element to set.
   *
   * \param elem Element to add (opCode does not need to be set)
   */
  void Add(B elem);

  virtual bool ProcessPacket(const tTransactionPacket* buffer);

  virtual const ::finroc::core::tPortData* PullRequest(tPortBase* origin, int8 add_locks);

  /*!
   * Remove element from set.
   *
   * \param elem Key of element to remove
   */
  inline void Remove(K& key)
  {
    Remove(set.Get(key));
  }

  /*!
   * Remove element from set.
   *
   * \param elem Element to remove
   */
  void Remove(B elem);

public:

  /*!
   * Entry in set. Is transaction at the same time - makes handling pending commands simpler
   */
  template<typename K>
  class tEntry : public tTransaction
  {
  public:

    tEntry() {}

    /*!
     * \return Unique key in network
     */
    inline K& GetKey() = 0;

    /*!
     * Change command has been received
     *
     * \param change Transaction/Entry containing change
     */
    inline void HandleChange(tTransactionalSet<K, null>::tEntry<K> change) = 0;

  };

};

} // namespace finroc
} // namespace core

#include "core/portdatabase/tTransactionalSet.hpp"

#endif // CORE__PORTDATABASE__TTRANSACTIONALSET_H
