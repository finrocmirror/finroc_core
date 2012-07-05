/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2011 Max Reichardt,
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

#ifndef core__port__cc__tCCPortDataManager_h__
#define core__port__cc__tCCPortDataManager_h__

#include "rrlib/finroc_core_utils/container/tReusable.h"

#include "core/portdatabase/tAbstractPortDataManager.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Manager for "cheap copy" data.
 * GenericObject managed by this class can be shared among different threads.
 * Manager is also very simple - no lock counting.
 * It is mainly used for queueing CCPortData.
 */
class tCCPortDataManager : public tAbstractPortDataManager<util::tReusable>
{
public:

  tCCPortDataManager() {}

  /*!
   * \param other Other object
   * \return Is data in this container equal to data provided as parameter?
   * (In C++ this is a heuristic for efficiency reasons. Objects are compared via memcmp.
   *  If classes contain pointers, this only guarantees that classes identified as equal really are.
   *  This is typically sufficient for "cheap copy" types though.)
   */
  inline bool ContentEquals(const void* other) const
  {
    return memcmp(GetObject()->GetRawDataPtr(), other, GetObject()->GetType().GetSize()) == 0;
  }

  inline void HandlePointerRelease()
  {
    Recycle2();
  }

  /*!
   * Create object of specified type managed by CCPortDataManager
   *
   * \param data_type Data type
   * \return Manager
   */
  inline static tCCPortDataManager* Create(const rrlib::rtti::tDataTypeBase& data_type)
  {
    return static_cast<tCCPortDataManager*>(data_type.CreateInstanceGeneric<tCCPortDataManager>()->GetManager());
  }

  virtual bool GenericHasLock()
  {
    return true;
  }

  virtual void GenericLockRelease()
  {
    Recycle2();
  }

  /*!
   * Recyle container
   */
  inline void Recycle2()
  {
    RecyclePortDataBuffer();
  }

  virtual const util::tString ToString() const
  {
    return std::string("CCPortDataManager: ") + GetContentString();
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortDataManager_h__
