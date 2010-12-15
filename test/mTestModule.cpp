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
#include "core/test/mTestModule.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<mTestModule> mTestModule::cCREATE_ACTION("Test Module", util::tTypedClass<mTestModule>());

mTestModule::mTestModule(tFrameworkElement* parent, const util::tString& name) :
    tModule(parent, name),
    si("SI", this->sensor_input, false),
    so("SO", this->sensor_output, true),
    ci("CI", this->controller_input, false),
    co("CO", this->controller_output, true)
{
}

void mTestModule::Control()
{
  FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, GetQualifiedName(), "::Control()");
  co.Publish(ci.GetIntRaw());
}

void mTestModule::Sense()
{
  FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, GetQualifiedName(), "::Sense()");
  so.Publish(si.GetIntRaw());
}

} // namespace finroc
} // namespace core

