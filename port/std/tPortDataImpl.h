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

#ifndef CORE__PORT__STD__TPORTDATAIMPL_H
#define CORE__PORT__STD__TPORTDATAIMPL_H

#include "core/portdatabase/tTypedObjectImpl.h"

namespace finroc
{
namespace core
{
class tPortDataManager;
class tPortDataReference;
class tDataType;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for all data that is used in ports.
 *
 * There are diverse management tasks (these task are handled by the data's manager).
 *  - Keeping track of "users" (reference counting - read locks would be more precise)
 *  - Managing Timestamps
 *
 * By convention, port data is immutable while published/read-locked/referenced.
 */
class __attribute__((aligned(8))) tPortData : public tTypedObject
{
private:
// 4byte => 4byte

  /*! Manager of data */
  tPortDataManager* manager;

public:

  /*! Log domain for serialization */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "serialization");

protected:

  /*!
   * \return lookup object's data type - may be overriden by subclass
   */
  virtual tDataType* LookupDataType();

public:

  /*!
   * Constructor as base class
   */
  tPortData();

  inline tPortDataReference* GetCurReference() const;

  /*!
   * \return Manager of data (null for PortData not used in ports)
   */
  inline tPortDataManager* GetManager() const
  {
    return manager;
  }

  virtual void HandleRecycle()
  {
    // default: do nothing
  }

  /*!
   * initialize data type
   * (typically called by PortDataCreationInfo)
   */
  void InitDataType();

  // override toString to have it available in C++ for PortData
  virtual const util::tString ToString() const
  {
    return "some port data";
  }

};

} // namespace finroc
} // namespace core

#include "core/port/std/tPortDataReference.h"

namespace finroc
{
namespace core
{
tPortDataReference* tPortData::GetCurReference() const
{
  tPortDataManager* mgr = GetManager();
  return tCombinedPointerOps::Create<tPortDataReference>(this, mgr->reuse_counter & 0x3);
}

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTDATAIMPL_H
