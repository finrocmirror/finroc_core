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

#ifndef core__plugin__tCreateFrameworkElementAction_h__
#define core__plugin__tCreateFrameworkElementAction_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tConstructorParameters;
class tStaticParameterList;

/*!
 * \author Max Reichardt
 *
 * Classes that implement this interface provide a generic method for
 * creating modules.
 *
 * When they are instantiated, they are automatically added to list of constructible elements.
 */
class tCreateFrameworkElementAction : public util::tInterface
{
public:

  tCreateFrameworkElementAction();

  /*!
   * Create Module (or Group)
   *
   * \param name Name of instantiated module
   * \param parent Parent of instantiated module
   * \param params Parameters
   * \return Created Module (or Group)
   */
  virtual tFrameworkElement* CreateModule(tFrameworkElement* parent, const util::tString& name, tConstructorParameters* params = NULL) const = 0;

  /*!
   * \return Returns name of group to which this create module action belongs
   */
  virtual util::tString GetModuleGroup() const = 0;

  /*!
   * \return Name of module type to be created
   */
  virtual util::tString GetName() const = 0;

  /*!
   * \return Returns types of parameters that the create method requires
   */
  virtual const tStaticParameterList* GetParameterTypes() const = 0;

  /*!
   * \return Returns .so file in which address provided as argument is found by dladdr
   */
  util::tString GetBinary(void* addr);

};

} // namespace finroc
} // namespace core

#endif // core__plugin__tCreateFrameworkElementAction_h__
