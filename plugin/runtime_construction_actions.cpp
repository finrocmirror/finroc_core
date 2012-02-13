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
#include "core/plugin/runtime_construction_actions.h"

namespace finroc
{
namespace runtime_construction
{
namespace internal
{

std::vector<core::tCreateExternalConnectionAction*>& GetConstructibleExternalConnections()
{
  static std::vector<core::tCreateExternalConnectionAction*> external_connections;
  return external_connections;
}

std::vector<core::tCreateFrameworkElementAction*>& GetConstructibleElements()
{
  static std::vector<core::tCreateFrameworkElementAction*> module_types;
  return module_types;
}

} // namespace


const std::vector<core::tCreateExternalConnectionAction*>& GetConstructibleExternalConnections()
{
  return internal::GetConstructibleExternalConnections();
}

const std::vector<core::tCreateFrameworkElementAction*>& GetConstructibleElements()
{
  return internal::GetConstructibleElements();
}

void RegisterCreatableElement(core::tCreateFrameworkElementAction* cma)
{
  internal::GetConstructibleElements().push_back(cma);
  core::tCreateExternalConnectionAction* ceca = dynamic_cast<core::tCreateExternalConnectionAction*>(cma);
  if (ceca)
  {
    internal::GetConstructibleExternalConnections().push_back(ceca);
  }
}

} // namespace finroc
} // namespace core

