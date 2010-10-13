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
#include "core/portdatabase/tDataType.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/buffers/tCoreInput.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/stream/tTransactionPacket.h"
#include "core/port/stream/tOutputTransactionStreamPort.h"
#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
template<typename K, typename B>
tTransactionalSet<K, B>::tTransactionalSet(const util::tString& description, bool input, bool output, bool local, tDataType* bclass, int commit_interval, bool input_shared, bool output_shared) :
    tTransactionalData<B>(description, input, output, local, bclass, commit_interval, input_shared, output_shared),
    set(NULL),
    temp_read_key(*static_cast<B*>(bclass->CreateTransactionInstance())),
    pending_commands(2u, 4u),
    last_pending_command_check(0),
    obj_mutex(),
    cPENDING_COMMAND_CHECK_INTERVAL(2000),
    cPENDING_COMMAND_TIMEOUT(10000)
{
}

template<typename K, typename B>
void tTransactionalSet<K, B>::Add(B elem)
{
  util::tLock lock1(this);
  elem.op_code = tTransaction::cADD;
  set.Put(elem.GetKey(), elem);
  CommitTransaction(elem);
}

template<typename K, typename B>
void tTransactionalSet<K, B>::CommitTransaction(B transaction)
{
  if (this->output != NULL)
  {
    try
    {
      CommitData(transaction);
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
    }
  }
}

template<typename K, typename B>
bool tTransactionalSet<K, B>::IncomingChange(B change)
{
  B entry = set.Get(change.GetKey());
  if (entry != NULL)
  {
    entry.HandleChange(change);
    return false;
  }
  return true;
}

template<typename K, typename B>
bool tTransactionalSet<K, B>::ProcessPacket(const tTransactionPacket* buffer)
{
  if (this->handling_new_connection && (!buffer->initial_packet))
  {
    return true;
  }

  tCoreInput* crv = &(tThreadLocalCache::GetFast()->input_packet_processor);
  crv->Reset(buffer);
  while (crv->MoreDataAvailable())
  {
    temp_read_key.Deserialize(*crv);
    bool defer = false;
    switch (temp_read_key.op_code)
    {
    case tTransaction::cADD:
      IncomingAdd(temp_read_key);
      break;
    case tTransaction::cREMOVE:
      defer = IncomingRemove(temp_read_key);
      break;
    case tTransaction::cCHANGE:
      defer = IncomingChange(temp_read_key);
      break;
    default:
      throw util::tRuntimeException("Corrupt Stream", CODE_LOCATION_MACRO);
    }
    if (defer)
    {
      B b = *static_cast<B*>(GetType()->CreateTransactionInstance());
      b.Assign(temp_read_key);
      pending_commands.Add(b, false);
    }
  }
  return false;
}

template<typename K, typename B>
void tTransactionalSet<K, B>::ProcessPendingCommands(int64 time)
{
  // check whether there is something to do
  if (time < last_pending_command_check + cPENDING_COMMAND_CHECK_INTERVAL)
  {
    return;
  }
  last_pending_command_check = time;

  // process commands
  util::tArrayWrapper<B>* b = pending_commands.GetIterable();
  for (int i = 0, n = b->Size(); i < n; i++)
  {
    B pc = b->Get(i);
    bool still_pending = false;
    if (pc.op_code == tTransaction::cREMOVE)
    {
      still_pending = IncomingRemove(pc);
    }
    else if (pc.op_code == tTransaction::cCHANGE)
    {
      still_pending = IncomingChange(pc);
    }
    if ((!still_pending) || time > pc.timestamp + cPENDING_COMMAND_TIMEOUT)
    {
      pending_commands.Remove(pc);
      i--;
    }
  }
}

template<typename K, typename B>
const ::finroc::core::tPortData* tTransactionalSet<K, B>::PullRequest(tPortBase* origin, int8 add_locks)
{
  util::tConcurrentMap<K, B>::tMapIterator it = set.GetIterator();
  tTransactionPacket* tp = this->output->GetUnusedBuffer();
  tp->initial_packet = true;
  while (it.Next())
  {
    B v = it.GetValue();
    tp->AddTransaction(v);
  }
  tp->GetManager()->GetCurrentRefCounter()->SetLocks(add_locks);
  return tp;
}

template<typename K, typename B>
void tTransactionalSet<K, B>::Remove(B elem)
{
  util::tLock lock1(this);
  set.Remove(elem.GetKey());
  elem.op_code = tTransaction::cREMOVE;
  CommitData(elem);
}

} // namespace finroc
} // namespace core

