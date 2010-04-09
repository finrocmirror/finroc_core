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

namespace finroc
{
namespace core
{
template<typename T>
tTransactionPacket tOutputTransactionStreamPort<T>::cLOCK;

template<typename T>
tTransactionPacket tOutputTransactionStreamPort<T>::cCOMMIT;

template<typename T>
tOutputTransactionStreamPort<T>::tOutputTransactionStreamPort(tPortCreationInfo pci, int commit_interval_, tPullRequestHandler* listener) :
    tPort<tTransactionPacket>(pci),
    commit_interval(commit_interval_),
    last_commit(0),
    current_packet()
{
  assert((commit_interval_ > 0));
  SetPullRequestHandler(listener);
  tStreamCommitThread::GetInstance()->Register(this);
}

template<typename T>
void tOutputTransactionStreamPort<T>::CommitData(tCoreSerializable& data)
{
  // Lock packet
  tTransactionPacket* cb = NULL;
  tTransactionPacket* expect = NULL;
  while (true)
  {
    cb = current_packet.Get();
    assert((cb != &(cLOCK)));  // concurrent write
    assert((cb != &(cCOMMIT)));  // concurrent write
    if (cb == NULL)
    {
      cb = GetUnusedBuffer();
      expect = NULL;
      break;
    }
    else
    {
      if (current_packet.CompareAndSet(cb, &(cLOCK)))
      {
        expect = &(cLOCK);
        break;
      }
    }
  }

  // copy data to packet
  cb->Add(data);
  //data.serialize(cb.serializer);

  // Unlock packet
  if (!current_packet.CompareAndSet(expect, cb))
  {
    // okay... we need to commit
    assert((current_packet.Get() == &(cCOMMIT)));
    Publish(cb);
    current_packet.Set(NULL);
  }
}

template<typename T>
tTransactionPacket* tOutputTransactionStreamPort<T>::GetUnusedBuffer()
{
  tTransactionPacket* result = ::finroc::core::tPort<tTransactionPacket>::GetUnusedBuffer();
  result->Reset();
  return result;
}

template<typename T>
void tOutputTransactionStreamPort<T>::StreamThreadCallback(int64 cur_time)
{
  if (cur_time >= last_commit + commit_interval)
  {
    // while loop to retry when atomic-compare-and-swap fails
    while (true)
    {
      tTransactionPacket* cb = current_packet.Get();
      if (cb == &(cCOMMIT))
      {
        break;  // none of our business
      }
      if (cb == &(cLOCK))
      {
        if (current_packet.CompareAndSet(&(cLOCK), &(cCOMMIT)))
        {
          break;
        }
      }
      else if (cb == NULL)
      {
        break;  // none of our business
      }
      else
      {
        // we commit
        if (current_packet.CompareAndSet(cb, NULL))
        {
          Publish(cb);
        }
      }
    }

    last_commit = cur_time;
  }
}

} // namespace finroc
} // namespace core

