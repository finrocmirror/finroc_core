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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__THREAD__TEXECUTIONCONTROL_H
#define CORE__THREAD__TEXECUTIONCONTROL_H

#include "core/tFinrocAnnotation.h"
#include "core/thread/tStartAndPausable.h"

namespace finroc
{
namespace core
{
class tDataType;
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
  static tDataType* cTYPE;

  /*! Wrapped StartAndPausable */
  tStartAndPausable* implementation;

  tExecutionControl(tStartAndPausable& implementation_);

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
   * Start/Resume execution
   */
  inline void Start()
  {
    implementation->StartExecution();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__THREAD__TEXECUTIONCONTROL_H