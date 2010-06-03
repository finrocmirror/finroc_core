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

#ifndef CORE__PLUGIN__TPLUGINMANAGER_H
#define CORE__PLUGIN__TPLUGINMANAGER_H

#include "core/portdatabase/tDataType.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/port/rpc/method/tPortInterface.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Provides interface for plugins to add modules, data types etc.
 * to central registry.
 */
class tPluginManager : public util::tUncopyableObject
{
public:

  tPluginManager() {}

  //  /**
  //   * Add data type that can be used in ports.
  //   *
  //   * \param dt Data type object
  //   */
  //  public DataType addDataType(DataType dt) {
  //      return DataTypeRegister.getInstance().addDataType(dt);
  //  }

  /*!
   * Add data type that can be used in ports
   *
   * \param java_class java class
   */
  template <typename T>
  inline tDataType* AddDataType(util::tTypedClass<T> java_class)
  {
    return tDataTypeRegister::GetInstance()->GetDataType(java_class);
  }

  /*!
   * Add method data type that can be used in interface ports
   *
   * \param name Name of data type
   * \param pi PortInterface for method type
   */
  inline tDataType* AddMethodDataType(const util::tString& name, tPortInterface* pi)
  {
    return tDataTypeRegister::GetInstance()->AddMethodDataType(name, pi);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TPLUGINMANAGER_H
