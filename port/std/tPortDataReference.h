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

#ifndef CORE__PORT__STD__TPORTDATAREFERENCE_H
#define CORE__PORT__STD__TPORTDATAREFERENCE_H

#include "core/port/std/tPortDataImpl.h"
#include "core/port/std/tPortDataManager.h"

#include "core/port/tCombinedPointer.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Reference to port data.
 *
 * In Java, several of these are created to compensate
 * that reuse counters cannot be encoded in pointer of data as in C++.
 * "portratio" contains an explanation.
 *
 * In C++, this is a pointer that points slightly above the port data.
 * That means, the class is empty and no objects of this class actually exist.
 *
 * Is immutable
 */
class tPortDataReference : public tCombinedPointer<tPortData>
{
public:

  /*!
   * \return Referenced port data
   */
  inline tPortData* GetData() const
  {
    return GetPointer();
  }

  /*!
   * \return Container of referenced data
   */
  inline tPortDataManager* GetManager() const
  {
    return GetData()->GetManager();
  }

  /*!
   * \return Reference counter associated with this reference
   */
  inline tPortDataManager::tRefCounter* GetRefCounter() const
  {
    return &(GetManager()->ref_counters[GetInfo()]);
  }

  /*!
   * \return Is data currently locked?
   */
  inline bool IsLocked() const
  {
    return GetRefCounter()->IsLocked();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTDATAREFERENCE_H
