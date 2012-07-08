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
#include "core/thread/tPeriodicFrameworkElementTask.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataTypeBase tPeriodicFrameworkElementTask::cTYPE = rrlib::rtti::tDataType<tPeriodicFrameworkElementTask>();

tPeriodicFrameworkElementTask::tPeriodicFrameworkElementTask(tEdgeAggregator& incoming_ports, tEdgeAggregator& outgoing_ports, rrlib::thread::tTask& task) :
  task(task),
  incoming(),
  outgoing(),
  previous_tasks(),
  next_tasks()
{
  incoming.push_back(&incoming_ports);
  outgoing.push_back(&outgoing_ports);
}

tPeriodicFrameworkElementTask::tPeriodicFrameworkElementTask(const std::vector<tEdgeAggregator*>& incoming_ports, const std::vector<tEdgeAggregator*>& outgoing_ports, rrlib::thread::tTask& task):
  task(task),
  incoming(incoming_ports),
  outgoing(outgoing_ports),
  previous_tasks(),
  next_tasks()
{
}

tPeriodicFrameworkElementTask::tPeriodicFrameworkElementTask() :
  task(*((rrlib::thread::tTask*)NULL)),
  incoming(),
  outgoing(),
  previous_tasks(),
  next_tasks()
{
  throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
}

bool tPeriodicFrameworkElementTask::IsSenseTask()
{
  for (auto it = outgoing.begin(); it < outgoing.end(); it++)
  {
    if ((*it)->GetFlag(tEdgeAggregator::cSENSOR_DATA))
    {
      return true;
    }
  }
  for (auto it = incoming.begin(); it < incoming.end(); it++)
  {
    if ((*it)->GetFlag(tEdgeAggregator::cSENSOR_DATA))
    {
      return true;
    }
  }
  return false;
}

} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tPeriodicFrameworkElementTask>;
