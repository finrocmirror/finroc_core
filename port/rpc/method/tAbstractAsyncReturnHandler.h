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

#ifndef CORE__PORT__RPC__METHOD__TABSTRACTASYNCRETURNHANDLER_H
#define CORE__PORT__RPC__METHOD__TABSTRACTASYNCRETURNHANDLER_H

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Common base interface for all AsyncReturnHandlers
 * (can be cast to AsyncReturnHandler<R>)
 */
class tAbstractAsyncReturnHandler : public util::tInterface
{
};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__METHOD__TABSTRACTASYNCRETURNHANDLER_H
