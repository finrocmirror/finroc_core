/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
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

#ifndef core__plugin__runtime_construction_h__
#define core__plugin__runtime_construction_h__

#include "core/plugin/tCreateExternalConnectionAction.h"
#include "core/plugin/tCreateFrameworkElementAction.h"

namespace finroc
{
namespace runtime_construction
{
/*!
 * \author Max Reichardt
 *
 * This file contains functions to retrieve available framework
 * element classes that can be instantiated at runtime.
 */

/*!
 * \return List with modules for external connections
 */
const std::vector<core::tCreateExternalConnectionAction*>& GetConstructibleExternalConnections();

/*!
 * \return List with framework elements that can be instantiated in this runtime using the standard mechanism
 */
const std::vector<core::tCreateFrameworkElementAction*>& GetConstructibleElements();

/*!
 * Add creatable framework element type
 * (objects won't be deleted by this class)
 *
 * \param cma CreateFrameworkElementAction to add
 */
void RegisterCreatableElement(core::tCreateFrameworkElementAction* cma);

} // namespace
} // namespace


#endif
