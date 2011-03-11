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

#ifndef core__thread__tStartAndPausable_h__
#define core__thread__tStartAndPausable_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Interface for objects whose execution can be started and stopped
 */
class tStartAndPausable : public util::tInterface
{
public:

  /*!
   * \return Whether element currently executing
   */
  virtual bool IsExecuting() = 0;

  /*!
   * Stops execution
   */
  virtual void PauseExecution() = 0;

  /*!
   * Starts execution
   */
  virtual void StartExecution() = 0;

};

} // namespace finroc
} // namespace core

#endif // core__thread__tStartAndPausable_h__
