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

#ifndef core__port__stream__tInputPacketProcessor_h__
#define core__port__stream__tInputPacketProcessor_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Processes incoming packets from input stream directly
 */
template<typename T>
class tInputPacketProcessor : public util::tInterface
{
public:

  /*!
   * Process single packet from stream.
   *
   * \param buffer Buffer that is processed
   * \param initial_packet Special/Initial packet?
   * \return (Still) enqueue packet in port queue? (despite processing it)
   */
  virtual bool ProcessPacket(const T* buffer) = 0;

};

} // namespace finroc
} // namespace core

#endif // core__port__stream__tInputPacketProcessor_h__
