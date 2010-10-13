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

#ifndef CORE__PLUGIN__TCREATEMODULEACTION_H
#define CORE__PLUGIN__TCREATEMODULEACTION_H

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tStructureParameterList;

/*!
 * \author Max Reichardt
 *
 * Classes that implement this interface provide a generic method for
 * creating modules.
 */
class tCreateModuleAction : public util::tInterface
{
public:

  /*!
   * Create Module (or Group)
   *
   * \param name Name of instantiated module
   * \param parent Parent of instantiated module
   * \param parameters Parameters
   * \return Created Module (or Group)
   */
  virtual tFrameworkElement* CreateModule(const util::tString& name, tFrameworkElement* parent, tStructureParameterList* params) const = 0;

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
  virtual const tStructureParameterList* GetParameterTypes() const = 0;

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TCREATEMODULEACTION_H
