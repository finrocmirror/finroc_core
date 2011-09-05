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

#ifndef core__thread__tExecutionControl_h__
#define core__thread__tExecutionControl_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/tFinrocAnnotation.h"
#include "core/thread/tStartAndPausable.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Annotation for framework elements that can be started and paused (via finstruct)
 */
class tExecutionControl : public tFinrocAnnotation
{
public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  /*! Wrapped StartAndPausable */
  tStartAndPausable* implementation;

  tExecutionControl(tStartAndPausable& implementation_);

  /*!
   * Dummy constructor. Generic instantiation is not supported.
   */
  tExecutionControl();

  /*!
   * Find StartAndPausable that is responsible for executing specified object
   *
   * \param fe Element
   * \return StartAndPausable
   */
  inline static tExecutionControl* Find(tFrameworkElement* fe)
  {
    return static_cast<tExecutionControl*>(FindParentWithAnnotation(fe, cTYPE));
  }

  /*!
   * Returns all execution controls below and including specified element
   *
   * \param result Result buffer for list of execution controls (controls are added to list)
   * \param elementHandle Framework element that is root of subtree to search for execution controls
   */
  static void FindAll(util::tSimpleList<tExecutionControl*>& result, tFrameworkElement* fe);

  /*!
   * \return Is currently executing?
   */
  inline bool IsRunning()
  {
    return implementation->IsExecuting();
  }

  /*!
   * Stop/Pause execution
   */
  inline void Pause()
  {
    implementation->PauseExecution();
  }

  /*!
   * Pauses all execution controls below and possibly attached to specified element
   *
   * \param fe Framework element that is root of subtree to search for execution controls
   */
  static void PauseAll(tFrameworkElement* fe);

  /*!
   * Start/Resume execution
   */
  inline void Start()
  {
    implementation->StartExecution();
  }

  /*!
   * Starts all execution controls below and possibly attached to specified element
   *
   * \param fe Framework element that is root of subtree to search for execution controls
   */
  static void StartAll(tFrameworkElement* fe);

};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tExecutionControl>;

#endif // core__thread__tExecutionControl_h__
