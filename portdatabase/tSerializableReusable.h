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

#ifndef CORE__PORTDATABASE__TSERIALIZABLEREUSABLE_H
#define CORE__PORTDATABASE__TSERIALIZABLEREUSABLE_H

#include "rrlib/finroc_core_utils/container/tReusable.h"
#include "core/portdatabase/tCoreSerializableImpl.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is the base class for some classes that are both Serializable and Reusable
 */
class tSerializableReusable : public util::tReusable, public tCoreSerializable
{
public:

  tSerializableReusable() {}

  /*!
   * Recycle call object - after calling this method, object is available in ReusablesPool it originated from
   *
   * (may be overridden by subclasses to perform custom cleanup)
   */
  virtual void GenericRecycle()
  {
    //responsibleThread = -1;
    ::finroc::util::tReusable::Recycle();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TSERIALIZABLEREUSABLE_H
