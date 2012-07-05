/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011-2012 Max Reichardt,
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

#ifndef core__portdatabase__tAbstractPortDataManager_h__
#define core__portdatabase__tAbstractPortDataManager_h__

#include "rrlib/finroc_core_utils/tGarbageDeleter.h"
#include "rrlib/rtti/rtti.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Base class for all port data manager classes used in data ports.
 * Contains things like time stamp that are common to these classes.
 *
 * <TReusable should either be util::tReusable or util::tReusableTL>
 */
template <typename TReusable>
class tAbstractPortDataManager : public TReusable, public rrlib::rtti::tGenericObjectManager
{
  /*! Timestamp for currently managed data */
  rrlib::time::tTimestamp timestamp;

protected:

  virtual void DeleteThis()
  {
    util::tGarbageDeleter::DeleteRT(this);
  }

  /*!
   * Recycles port data buffer
   */
  inline void RecyclePortDataBuffer()
  {
    timestamp = rrlib::time::cNO_TIME;
    TReusable::Recycle();
  }

public:

  tAbstractPortDataManager() : timestamp(rrlib::time::cNO_TIME) {}

  virtual void CustomDelete(bool called_from_gc)
  {
    this->~tAbstractPortDataManager();
    delete GetObject();
  }

  /*!
   * Release lock
   */
  virtual void GenericLockRelease() = 0;

  /*!
   * \return String containing content type and
   */
  inline util::tString GetContentString() const
  {
    std::ostringstream os;
    os << GetObject()->GetType().GetName() << " (" << GetObject()->GetRawDataPtr() << ")";
    return os.str();
  }

  /*!
   * \return Timestamp for currently managed data
   */
  inline rrlib::time::tTimestamp GetTimestamp() const
  {
    return timestamp;
  }

  /*!
   * \param timestamp New timestamp for currently managed data
   */
  inline void SetTimestamp(const rrlib::time::tTimestamp& timestamp)
  {
    this->timestamp = timestamp;
  }

};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tAbstractPortDataManager_h__
