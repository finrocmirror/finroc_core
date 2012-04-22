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

#ifndef core__port__std__tPortDataBufferPool_h__
#define core__port__std__tPortDataBufferPool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/tDataTypeBase.h"
#include "core/port/std/tPortDataManager.h"
#include "rrlib/finroc_core_utils/container/tReusablesPoolCR.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Buffer pool for specific port and thread.
 * In order to be real-time-capable, enough buffers need to be initially allocated... otherwise the application
 * becomes real-time-capable later - after enough buffers have been allocated.
 */
class tPortDataBufferPool : public util::tReusablesPoolCR<tPortDataManager>
{
public:

  /*! Data Type of buffers in pool */
  const rrlib::rtti::tDataTypeBase data_type;

private:

  /*!
   * \return Create new buffer/instance of port data and add to pool
   */
  tPortDataManager* CreateBuffer();

  /*!
   * \return Create new buffer/instance of port data
   */
  tPortDataManager* CreateBufferRaw();

  /*!
   * Helper for above
   */
  void PrintElement(int indent, const tPortDataManager* pdm, std::stringstream& output) const;

protected:

  // destructor is intentionally protected: call controlledDelete() instead
  virtual ~tPortDataBufferPool() {}

  /*!
   * only for derived MultiTypeDataBufferPool
   */
  tPortDataBufferPool();

public:

  /*!
   * \param data_type Type of buffers in pool
   */
  tPortDataBufferPool(const rrlib::rtti::tDataTypeBase& data_type_, int initial_size);

  /*!
   * Is final so it is not used polymorphically -
   * merely for efficiency reasons (~factor 4) -
   * which is critical here.
   *
   * \return Returns unused buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tPortDataManager* GetUnusedBuffer()
  {
    tPortDataManager* pc = GetUnused();
    if (pc != NULL)
    {
      pc->SetUnused(true);
      return pc;
    }
    return CreateBuffer();
  }

  /*!
   * Prints info about all elements in pool to console
   *
   * \param indent Indentation
   * \param output
   */
  void PrintStructure(int indent, std::stringstream& output) const;

};

} // namespace finroc
} // namespace core

#endif // core__port__std__tPortDataBufferPool_h__
