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
#include "core/port/rpc/tInterfaceClientPort.h"

namespace finroc
{
namespace core
{
tInterfaceClientPort::tInterfaceClientPort(const util::tString& description, tFrameworkElement* parent, tDataType* type)
{
  this->wrapped = new tPortImpl(this, description, parent, type, tInterfacePort::eClient);
}

tInterfaceClientPort::tPortImpl::tPortImpl(tInterfaceClientPort* const outer_class_ptr_, const util::tString& description, tFrameworkElement* parent, tDataType* type, tInterfacePort::tType client) :
    tInterfacePort(description, parent, type, client),
    outer_class_ptr(outer_class_ptr_)
{
}

void tInterfaceClientPort::tPortImpl::ConnectionRemoved(tAbstractPort* partner)
{
  outer_class_ptr->ConnectionRemoved(partner);
}

void tInterfaceClientPort::tPortImpl::NewConnection(tAbstractPort* partner)
{
  outer_class_ptr->NewConnection(partner);
}

} // namespace finroc
} // namespace core

