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

#ifndef CORE__PORT__CC__TBOUNDEDNUMBERPORT_H
#define CORE__PORT__CC__TBOUNDEDNUMBERPORT_H

#include "core/datatype/tBounds.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/tPortFlags.h"
#include "core/port/cc/tNumberPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Number port with upper and lower bounds for values
 */
class tBoundedNumberPort : public tNumberPort
{
private:

  /*! Bounds of this port */
  tBounds bounds;

  /*!
   * Make sure non-standard assign flag is set
   */
  inline static tPortCreationInfo ProcessPciBNP(tPortCreationInfo pci)
  {
    pci.flags = pci.flags | tPortFlags::cNON_STANDARD_ASSIGN;
    return pci;
  }

protected:

  virtual void NonStandardAssign(tThreadLocalCache* tc);

public:

  /*!
   * \param pci Port Creation info
   * \param min Minimum Value
   * \param max Maximum Value
   * \param action Action to perform when index is out of bounds
   */
  tBoundedNumberPort(tPortCreationInfo pci, tBounds b);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TBOUNDEDNUMBERPORT_H
