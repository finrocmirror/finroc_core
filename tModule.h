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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__TMODULE_H
#define CORE__TMODULE_H

#include "core/port/tEdgeAggregator.h"
#include "core/tFrameworkElement.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Standard Module.
 * Equivalent to tModule class in MCA2.
 */
class tModule : public tFrameworkElement, public util::tTask
{
  /*! Task for periodic Sense()-call */
  class tSenseTask : public util::tObject, public util::tTask
  {
  private:

    // Outer class Module
    tModule* const outer_class_ptr;

  public:

    tSenseTask(tModule* const outer_class_ptr_) :
        outer_class_ptr(outer_class_ptr_)
    {}

    virtual void ExecuteTask();

  };

private:

  /*! Sense task instance */
  tSenseTask sense_task;

protected:

  /*! Sensor Input interface */
  tEdgeAggregator* sensor_input;

  /*! Sensor Output interface */
  tEdgeAggregator* sensor_output;

  /*! Controller Input interface */
  tEdgeAggregator* controller_input;

  /*! Controller output interface */
  tEdgeAggregator* controller_output;

  /*!
   * Control method
   */
  virtual void Control()
  {
  }

  /*!
   * Sense method
   */
  virtual void Sense()
  {
  }

public:

  /*!
   * \param name Module name
   * \param parent Parent of module
   */
  tModule(const util::tString& name, tFrameworkElement* parent);

  virtual void ExecuteTask()
  {
    Control();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TMODULE_H
