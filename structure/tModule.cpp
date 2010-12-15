/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010
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
#include "core/structure/tModule.h"
#include "core/thread/tPeriodicFrameworkElementTask.h"

namespace finroc
{
namespace core
{
tModule::tModule(tFrameworkElement* parent, const util::tString& name) :
    tFrameworkElement(parent, name),
    sense_task(this),
    sensor_input(new tEdgeAggregator(this, "Sensor Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    sensor_output(new tEdgeAggregator(this, "Sensor Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    controller_input(new tEdgeAggregator(this, "Controller Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),
    controller_output(new tEdgeAggregator(this, "Controller Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA))
{
  sensor_input->AddAnnotation(new tPeriodicFrameworkElementTask(sensor_input, sensor_output, &(sense_task)));
  controller_input->AddAnnotation(new tPeriodicFrameworkElementTask(controller_input, controller_output, this));
}

void tModule::tSenseTask::ExecuteTask()
{
  outer_class_ptr->Sense();
}

} // namespace finroc
} // namespace core

