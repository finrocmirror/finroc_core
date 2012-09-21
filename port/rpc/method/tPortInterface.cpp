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
#include "core/port/rpc/method/tPortInterface.h"
#include "core/port/rpc/method/tAbstractMethod.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
tPortInterface::tPortInterface(const util::tString& name_, bool shutdown_runtime_on_delete_) :
  methods(),
  my_type(NULL),
  name(name_),
  shutdown_runtime_on_delete(shutdown_runtime_on_delete_)
{
}

void tPortInterface::AddMethod(tAbstractMethod* m)
{
  assert(((methods.size() <= 127)) && "too many methods");
  if (ContainsMethod(*m))
  {
    // should only happen in C++ (with certain static initialization order)
    m->method_id = static_cast<int8>(std::find(methods.begin(), methods.end(), m) - methods.begin());
    m->type = this;
  }
  else
  {
    m->method_id = static_cast<int8>(methods.size());
    m->type = this;
    methods.push_back(m);
  }
}

tPortInterface::~tPortInterface()
{
  if (shutdown_runtime_on_delete)
  {
    tRuntimeEnvironment::Shutdown();
  }
}

} // namespace finroc
} // namespace core

