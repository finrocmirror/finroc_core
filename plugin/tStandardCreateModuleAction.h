/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef CORE__PLUGIN__TSTANDARDCREATEMODULEACTION_H
#define CORE__PLUGIN__TSTANDARDCREATEMODULEACTION_H

#include "core/plugin/tPlugins.h"
#include "core/plugin/tCreateModuleAction.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tConstructorParameters;
class tStructureParameterList;

/*!
 * \author Max Reichardt
 *
 * Default create module action for finroc modules
 *
 * Modules need to have a constructor taking name and parent
 */
template<typename T>
class tStandardCreateModuleAction : public util::tUncopyableObject, public tCreateModuleAction
{
private:

  /*! Name of module type */
  util::tString group;

  /*! Name of module type */
  util::tString type_name;

public:

  /*!
   * \param group Name of module group
   * \param type_name Name of module type
   */
  tStandardCreateModuleAction(const util::tString& type_name_) :
      group(),
      type_name(type_name_)
  {
    tPlugins::GetInstance()->AddModuleType(this);

    group = GetBinary((void*)CreateModuleImpl);
  }

  /*!
   * \param group Name of module group
   * \param type_name Name of module type
   * \param module_class Module class (only needed in Java)
   */
  tStandardCreateModuleAction(const util::tString& type_name_, const util::tTypedClass<T>& module_class) :
      group(),
      type_name(type_name_)
  {
    tPlugins::GetInstance()->AddModuleType(this);

    group = GetBinary((void*)CreateModuleImpl);
  }

  static tFrameworkElement* CreateModuleImpl(tFrameworkElement* parent, const util::tString& name)
  {
    return new T(parent, name);
  }

  virtual tFrameworkElement* CreateModule(tFrameworkElement* parent, const util::tString& name, tConstructorParameters* params) const
  {
    return CreateModuleImpl(parent, name);
  }

  virtual util::tString GetModuleGroup() const
  {
    return group;
  }

  virtual util::tString GetName() const
  {
    return type_name;
  }

  virtual const tStructureParameterList* GetParameterTypes() const
  {
    return NULL;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TSTANDARDCREATEMODULEACTION_H
