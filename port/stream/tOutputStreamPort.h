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

#ifndef CORE__PORT__STREAM__TOUTPUTSTREAMPORT_H
#define CORE__PORT__STREAM__TOUTPUTSTREAMPORT_H

#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPort.h"

namespace finroc
{
namespace core
{
class tPullRequestHandler;

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
class tOutputStreamPort : public tPort<T>
{
public:

  /*!
   * \param pci Port Creation Info
   * \param listener Listener for pull requests
   */
  tOutputStreamPort(tPortCreationInfo pci, tPullRequestHandler* listener) :
      tPort<T>(pci)
  {
    SetPullRequestHandler(listener);
  }

  /*
   * Write data buffer instantly to connected ports.
   * (only valid to call this on buffers that do not commit data deferred)
   *
   * \param data Data to write
   */
  inline void CommitDataBuffer(T* data)
  {
    Publish(data);
  }

  /* (non-Javadoc)
   * @see core.port4.Port#getUnusedBuffer()
   */
  inline T* GetUnusedBuffer()
  {
    T* result = ::finroc::core::tPort<T>::GetUnusedBuffer();
    result->Clear();
    return result;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STREAM__TOUTPUTSTREAMPORT_H
