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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STREAM__TOUTPUTTRANSACTIONSTREAMPORT_H
#define CORE__PORT__STREAM__TOUTPUTTRANSACTIONSTREAMPORT_H

#include "core/port/stream/tTransactionPacket.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/stream/tStreamCommitThread.h"
#include "core/port/std/tPort.h"

namespace finroc
{
namespace core
{
class tPullRequestHandler;
class tCoreSerializable;

/*!
 * \author Max Reichardt
 *
 * This is a port that provides an output stream to it's user and to the outside.
 *
 * Typically, packets are pushed across the network.
 * Pulling doesn't make much sense for a stream.
 * Therefore, pulling typically provides some general/initial info about the stream - or simply nothing.
 *
 * (Implementation of this class is non-blocking... that's why it's slightly verbose)
 */
template<typename T>
class tOutputTransactionStreamPort : public tPort<tTransactionPacket>, public tStreamCommitThread::tCallback
{
private:

  /*! ChunkedBuffer for signalling only: SIGNAL that writer currently writes to chunk */
  static tTransactionPacket cLOCK;

  /*! ChunkedBuffer for signalling only: SIGNAL that writer should commit data after writing */
  static tTransactionPacket cCOMMIT;

protected:

  /*!
   * interval in which new data is commited
   * higher values may be useful for grouping transactions.
   */
  int commit_interval;

  /*!
   * last time stamp of commit
   */
  int64 last_commit;

  /*!
   * Stream packet that is currently written.
   * When it is written to, this variable contains lock to this packet.
   * When packet has been committed, variable contains null
   */
  util::tAtomicPtr<tTransactionPacket> current_packet;

  virtual void PrepareDelete()
  {
    tStreamCommitThread::GetInstance()->Unregister(this);
    ::finroc::core::tAbstractPort::PrepareDelete();
  }

public:

  /*!
   * \param pci Port Creation Info
   * \param commit_interval Interval in which new data is commited - higher values may be useful for grouping transactions.
   * \param listener Listener for pull requests
   */
  tOutputTransactionStreamPort(tPortCreationInfo pci, int commit_interval_, tPullRequestHandler* listener);

  /*!
   * Writes data to stream
   * (may not be called concurrently)
   *
   * \param data Data to write (is copied and can instantly be reused)
   */
  void CommitData(tCoreSerializable& data);

  /* (non-Javadoc)
   * @see core.port4.Port#getUnusedBuffer()
   */
  tTransactionPacket* GetUnusedBuffer();

  virtual void StreamThreadCallback(int64 cur_time);

};

} // namespace finroc
} // namespace core

#include "core/port/stream/tOutputTransactionStreamPort.hpp"

#endif // CORE__PORT__STREAM__TOUTPUTTRANSACTIONSTREAMPORT_H
