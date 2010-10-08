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
#ifndef CORE__PORTDATABASE__TCPPSTDFACTORY_H
#define CORE__PORTDATABASE__TCPPSTDFACTORY_H

#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/portdatabase/tPortDataFactory.h"

namespace finroc
{
namespace core
{

template <typename T>
class tCppStdFactory : public tPortDataFactory
{
public:
  virtual tTypedObject* Create(tDataType* data_type, bool itc)
  {
    return static_cast<tTypedObject*>(new T());
  }
};

template <typename T>
class tCppCCFactory : public tPortDataFactory
{
public:
  virtual tTypedObject* Create(tDataType* data_type, bool itc)
  {
    return (itc ? static_cast<tTypedObject*>(new tCCInterThreadContainer<T>(data_type)) : static_cast<tTypedObject*>(new tCCPortDataContainer<T>(data_type)));
  }
};

class tNullFactory : public tPortDataFactory
{
public:
  virtual tTypedObject* Create(tDataType* data_type, bool itc)
  {
    return NULL;
  }
};

} // namespace finroc
} // namespace core


#endif // CORE__PORTDATABASE__TCPPSTDFACTORY_H
