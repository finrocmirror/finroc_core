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

#ifndef core__port__stream__tOutputStreamPort_h__
#define core__port__stream__tOutputStreamPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPortBase.h"
#include "core/port/tPortWrapperBase.h"
#include "core/port/tPort.h"

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
  tOutputStreamPort(tPortCreationInfo<T> pci, tPullRequestHandler* listener) :
      tPort<T>(pci)
  {
    (static_cast<tPortBase*>(GetWrapped()))->SetPullRequestHandler(listener);
  }

  /*!
   * Write data buffer instantly to connected ports.
   * (only valid to call this on buffers that do not commit data deferred)
   *
   * \param data Data to write
   */
  inline void CommitDataBuffer(T* data)
  {
    Publish(tPortDataPtr<T>(data));
  }

  inline tPortDataPtr<T> GetUnusedBuffer()
  {
    T* result = ::finroc::core::tPort<T>::GetUnusedBuffer().get();
    result->Clear();
    return tPortDataPtr<T>(result);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__stream__tOutputStreamPort_h__
