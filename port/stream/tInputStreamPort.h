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

#ifndef CORE__PORT__STREAM__TINPUTSTREAMPORT_H
#define CORE__PORT__STREAM__TINPUTSTREAMPORT_H

#include "core/port/std/tPortQueueFragment.h"
#include "core/port/stream/tInputPacketProcessor.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPublishCache.h"
#include "core/port/std/tPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is a port that provides an input stream to it's user
 * and to the outside.
 * Actually the incoming data comes in small packets as sent by
 * the sender. This is easier to handle than setting up a thread
 * for blocking IO.
 */
template<typename T>
class tInputStreamPort : public tPort<T>
{
private:

  /*!
   * Used for dequeueing data
   */
  tPortQueueFragment<T> dequeue_buffer;

  /*!
   * User of input stream
   */
  tInputPacketProcessor<T>* user;

public:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "stream_ports");

private:

  static tPortCreationInfo ProcessPCI(tPortCreationInfo pci, const util::tString& description);

protected:

  virtual void NonStandardAssign(tPublishCache& pc);

  bool ProcessPacket(T* data);

public:

  tInputStreamPort(const util::tString& description, tPortCreationInfo pci, tInputPacketProcessor<T>* user_);

  /*!
   * Process any packet currently in queue (method only for convenience)
   */
  void ProcessPackets();

};

} // namespace finroc
} // namespace core

#include "core/port/stream/tInputStreamPort.hpp"

#endif // CORE__PORT__STREAM__TINPUTSTREAMPORT_H
