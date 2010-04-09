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

#ifndef CORE__PORT__STREAM__TSINGLETONPORT_H
#define CORE__PORT__STREAM__TSINGLETONPORT_H

#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPublishCache.h"
#include "core/port/std/tPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Port whose data buffer is fixed.
 * This can be useful for complex data that is updated via transactions.
 * This port class does not handle concurrent access to the data buffer.
 * The data (buffer) has to take care of this issue.
 */
template<typename T>
class tSingletonPort : public tPort<T>
{
private:

  /*! Singleton value */
  T* singleton_value;

  /*!
   * modifies PortCreationInfo for SingletonPort
   *
   * \param pci old PortCreationInfo
   * \param default_value
   * \return new PortCreationInfo
   */
  static tPortCreationInfo AdjustPci(tPortCreationInfo pci);

protected:

  virtual void NonStandardAssign(tPublishCache& pc);

public:

  /*!
   * \param pci Bundled creation information about port
   * \param singleton The Singleton object that is contained in this port
   */
  tSingletonPort(tPortCreationInfo pci, T* singleton);

//  @Override
//  public PortDataContainer<T> browserGetData(Object locker) {
//    return singletonValue;
//  }
//
//
//  @Override
//  public void browserSet(PortDataContainer<T> newValue) {
//    if (newValue != singletonValue) {
//      throw new RuntimeException("Cannot change contents of Singleton-Port");
//    }
//  }
//
//  @Override
//  protected PortDataContainer<T> getInternal() {
//    return singletonValue;
//  }

};

} // namespace finroc
} // namespace core

#include "core/port/stream/tSingletonPort.hpp"

#endif // CORE__PORT__STREAM__TSINGLETONPORT_H
