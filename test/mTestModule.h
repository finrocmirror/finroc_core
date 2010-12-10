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

#ifndef CORE__TEST__TTESTMODULE_H
#define CORE__TEST__TTESTMODULE_H

#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/tModule.h"

namespace finroc
{
namespace core
{
class tNumberPort;

/*!
 * \author Max Reichardt
 *
 * Module for testing scheduling
 */
class mTestModule : public tModule
{
private:

  /*! Create Module Action for finstruct */
  static tStandardCreateModuleAction<mTestModule> cCREATE_ACTION;

public:

  /*! Test ports */
  tNumberPort* si, * so, * ci, * co;

protected:

  virtual void Control();

  virtual void Sense();

public:

  mTestModule(tFrameworkElement* parent, const util::tString& name);

};

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TTESTMODULE_H
