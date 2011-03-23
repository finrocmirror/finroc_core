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

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/portdatabase/tReusableGenericObjectManager.h"

namespace rrlib
{
namespace serialization
{
class tDataTypeBase;
} // namespace rrlib
} // namespace serialization

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
class tCCPortDataManager : public tReusableGenericObjectManager
{
public:

  tCCPortDataManager() {}

  //    /** Assign other data to this container */
  //    public void assign(@Const CCPortData other) {
  //        portData.assign(other);
  //    }
  //
  //    /** Assign data in this container to other data */
  //    @ConstMethod public void assignTo(CCPortData other) {
  //        portData.assignTo(other);
  //    }
  //

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
  inline static tCCPortDataManager* Create(const rrlib::serialization::tDataTypeBase& data_type)
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
    //System.out.println("Recycling interthread buffer " + this.hashCode());
    ::finroc::util::tReusable::Recycle();
  }

  virtual const util::tString ToString() const
  {
    return util::tStringBuilder("CCPortDataManager: ") + GetContentString();
  }

//
//    void setData(const T& data) {
//        setData(&data);
//    }
//
//
//    /**
//     * Assign new value to container
//     *
//     * \param data new value
//     */
//    public void setData(@Const @Ptr T data) {
//        portData.assign((CCPortData)data);
//    }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortDataManager_h__
