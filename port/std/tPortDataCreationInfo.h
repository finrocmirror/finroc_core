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

#ifndef CORE__PORT__STD__TPORTDATACREATIONINFO_H
#define CORE__PORT__STD__TPORTDATACREATIONINFO_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"

namespace finroc
{
namespace core
{
class tPortDataManager;
class tPortData;

/*!
 * \author Max Reichardt
 *
 * Contains information - for each thread - about how to construct
 * the next managed port data instance.
 *
 * This information should only be set by ports who manage data buffers.
 * Without this information, port data is created that cannot be used
 * in ports - only locally.
 *
 * Information is reset after the creation of every port.
 *
 * Using this class, not that many parameters need to be passed through
 * the constructors.
 */
class tPortDataCreationInfo : public util::tUncopyableObject
{
private:

  /*! Manager that will handle/manage currently created port data */
  tPortDataManager* manager;

  /*! List with port data objects whose data types need to be set */
  util::tSimpleList<tPortData*> uninitialized_port_data;

  /*!
   * Other data instance that may act as a prototype for this.
   * E.g. to allocate a buffer of the same size initially.
   * Must have the same type as the created data.
   * May be null (one reason: there must obviously be a first one).
   */
  const tPortData* prototype;

  /*! Stores info for each thread - wouldn't be thread-safe otherwise */
  static util::tThreadLocal<tPortDataCreationInfo> info;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_mutex;

  tPortDataCreationInfo() :
      manager(NULL),
      uninitialized_port_data(),
      prototype(NULL),
      obj_mutex()
  {}

  /*!
   * (Should only be called by PortData class)
   *
   * \param obj Uninitialized object
   */
  inline void AddUnitializedObject(tPortData* obj)
  {
    util::tLock lock2(this);
    uninitialized_port_data.Add(obj);
  }

  /*!
   * \return Thread specific info
   */
  static tPortDataCreationInfo* Get();

  inline tPortDataManager* GetManager()
  {
    return manager;
  }

  inline const tPortData* GetPrototype()
  {
    return prototype;
  }

  /*!
   * Initializes data types of queued objects.
   * (May be called anywhere except of constructors (binding problem)
   *  - typically by PortDataContainerPool class)
   */
  void InitUnitializedObjects();

  /*!
   * Resets all values to null - this is the neutral, initial state
   */
  void Reset();

  inline void SetManager(tPortDataManager* manager_)
  {
    this->manager = manager_;
  }

  inline void SetPrototype(const tPortData* port_data)
  {
    this->prototype = port_data;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTDATACREATIONINFO_H
