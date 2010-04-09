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

namespace finroc
{
namespace core
{
util::tString tAbstractMethod::cNO_PARAM = "NO_PARAMETER";

tAbstractMethod::tAbstractMethod(tPortInterface& port_interface, const util::tString& name_, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread_) :
    name(name_),
    parameter_names(4),
    parameter_count(4),
    handle_in_extra_thread(handle_in_extra_thread_),
    method_id(0),
    type(NULL)
{
  parameter_names[0] = p1_name;
  parameter_names[1] = p2_name;
  parameter_names[2] = p3_name;
  parameter_names[3] = p4_name;
  for (int i = 0; i < 4; i++)
  {
    if (parameter_names[i].Equals(cNO_PARAM))
    {
      parameter_count = i;
    }
  }
  port_interface.AddMethod(this);
}

tAbstractVoidMethod::tAbstractVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, p1_name, p2_name, p3_name, p4_name, handle_in_extra_thread)
{
}

const int tAbstractNonVoidMethod::cDEFAULT_NET_TIMEOUT;

tAbstractNonVoidMethod::tAbstractNonVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread, int default_net_timeout_) :
    tAbstractMethod(port_interface, name, p1_name, p2_name, p3_name, p4_name, handle_in_extra_thread),
    default_net_timeout(default_net_timeout_)
{
}

} // namespace finroc
} // namespace core

