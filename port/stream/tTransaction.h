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

#ifndef CORE__PORT__STREAM__TTRANSACTION_H
#define CORE__PORT__STREAM__TTRANSACTION_H

#include "core/tRuntimeListener.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tTypedObjectImpl.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Abstract base class for transactions.
 *
 * Defines some basic opcodes
 *
 * Design decisions:
 * Transactions are passed by value.
 * Single transactions may be allocated on the stack - then they are real-time-capable.
 * For storing multiple transactions - memory needs to be allocated - using object pools did not seem appropriate (yet?).
 */
class tTransaction : public tTypedObject
{
public:

  /*! Some basic opcodes */
  static const int8 cADD = tRuntimeListener::cADD, cCHANGE = tRuntimeListener::cCHANGE, cREMOVE = tRuntimeListener::cREMOVE;

  /*! Op code of transaction */
  int8 op_code;

  /*! Timestamp of transaction - to identify outdated transactions */
  int64 timestamp;

  tTransaction() :
      op_code(0),
      timestamp(0)
  {}

  /*! Assign values of other transaction to this one (copy operator in C++) */
  inline void Assign(const tTransaction& other)
  {
    this->op_code = other.op_code;
    this->timestamp = other.timestamp;
  }

  virtual void Deserialize(tCoreInput& is)
  {
    op_code = is.ReadByte();
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    os.WriteByte(op_code);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STREAM__TTRANSACTION_H
