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

#ifndef CORE__PLUGIN__TCREATEEXTERNALCONNECTIONACTION_H
#define CORE__PLUGIN__TCREATEEXTERNALCONNECTIONACTION_H

namespace finroc
{
namespace core
{
class tExternalConnection;

/*!
 * \author Max Reichardt
 *
 * Class to create Module using empty standard constructor
 */
class tCreateExternalConnectionAction : public util::tInterface
{
public:

  virtual tExternalConnection* CreateExternalConnection() = 0;

  virtual const util::tString ToString() const = 0;

//  /** Constructor to invoke */
//  private Class<? extends ExternalConnection> c;
//
//  /** Default object when returning empty parameter list */
//  private static final Class<?>[] params = new Class<?>[]{String.class};
//
//  /**
//   * \param m wrapped method
//   * \param group method's group
//   */
//  public CreateExternalConnectionAction(Class<? extends ExternalConnection> c) {
//      this.c = c;
//  }
//
//  @Override
//  public ExternalConnection createModule(Object... params) throws Exception {
//      return createModule(params[0].toString());
//  }
//
//  public ExternalConnection createModule(String address) throws Exception {
//      ExternalConnection m = (ExternalConnection)c.newInstance();
//      m.connect(address);
//      return m;
//  }
//
//  /**
//   * Create Connection Module. Window will pop up and ask for address.
//   *
//   * \return Created Connection Module
//   */
//  public ExternalConnection createModule() throws Exception {
//      ExternalConnection m = (ExternalConnection)c.newInstance();
//      m.connect(null);
//      return m;
//  }
//
//  @Override
//  public Class<?>[] getParameterTypes() {
//      return params;
//  }
//
//  @Override
//  public String toString() {
//      return c.getSimpleName();
//  }
//
//  @Override
//  public String getModuleGroup() {
//      return ExternalConnection.GROUP_NAME;
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TCREATEEXTERNALCONNECTIONACTION_H
