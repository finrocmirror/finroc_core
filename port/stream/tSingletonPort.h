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

#ifndef core__port__stream__tSingletonPort_h__
#define core__port__stream__tSingletonPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPort.h"
#include "core/port/std/tPortBase.h"

namespace finroc
{
namespace core
{
class tPublishCache;

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
  /*! Special Port class to load value when initialized */
  template<typename T>
  class tPortImpl : public tPortBase
  {
  private:

    /*! Singleton value */
    T singleton_value;

  protected:

    virtual void NonStandardAssign(tPublishCache& pc);

  public:

    tPortImpl(tPortCreationInfo<T> pci, T& singleton);

  };

  /*!
   * modifies PortCreationInfo for SingletonPort
   *
   * \param pci old PortCreationInfo
   * \param default_value
   * \return new PortCreationInfo
   */
  static tPortCreationInfo<T> AdjustPci(tPortCreationInfo<T> pci);

public:

  /*!
   * \param pci Bundled creation information about port
   * \param singleton The Singleton object that is contained in this port
   */
  tSingletonPort(tPortCreationInfo<T> pci, T& singleton);

};

} // namespace finroc
} // namespace core

#include "core/port/stream/tSingletonPort.hpp"

#endif // core__port__stream__tSingletonPort_h__
