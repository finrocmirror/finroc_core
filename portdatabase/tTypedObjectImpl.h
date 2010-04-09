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

#ifndef CORE__PORTDATABASE__TTYPEDOBJECTIMPL_H
#define CORE__PORTDATABASE__TTYPEDOBJECTIMPL_H

#include "core/portdatabase/tCoreSerializable.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for any object that has additional
 * type information as provided in this package.
 *
 * Such classes can be cleanly serialized to the network
 *
 * C++ issue: Typed objects are not automatically jc objects!
 */
class tTypedObject : public tCoreSerializable
{
protected:

  /*! Type of object */
  tDataType* type;

public:

  tTypedObject() :
      type(NULL)
  {}

  /*!
   * \return Type of object
   */
  inline tDataType* GetType() const
  {
    return type;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TTYPEDOBJECTIMPL_H
