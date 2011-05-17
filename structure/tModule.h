//
// You received this file as part of Finroc
// A framework for innovative robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    tModule.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2010-12-17
 *
 * \brief Contains tModule
 *
 * \b tModule
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tModule_h_
#define _core__structure__tModule_h_

#include "core/structure/tModuleBase.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace structure
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//!
/*!
 *
 */
class tModule : public tModuleBase
{
  class UpdateTask : public finroc::util::tTask
  {
    tModule *const module;
  public:
    UpdateTask(tModule *module);
    virtual void ExecuteTask();
  };

  finroc::core::tEdgeAggregator *input;
  finroc::core::tEdgeAggregator *output;
  UpdateTask update_task;

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual void Update();

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  /*!
   * May be called in Update() method to check
   * whether any input port has changed.
   *
   * (Changed flags are reset after Update() is called)
   */
  bool InputChanged()
  {
    return HasAnyPortChanged(input);
  }

  template < typename T = double >
  struct tInput : public tPort<T>, tConveniencePort<tModule>
  {
    tInput()
        : tPort<T>(this->GetPortName(), this->FindParent()->input, false)
    {}

    tInput(const finroc::util::tString &name)
        : tPort<T>(name, this->FindParent()->input, false)
    {
      this->UpdateCurrentPortNameIndex();
    }
  };

  template < typename T = double >
  struct tOutput : public tPort<T>, tConveniencePort<tModule>
  {
    tOutput()
        : tPort<T>(this->GetPortName(), this->FindParent()->output, true)
    {}

    tOutput(const finroc::util::tString &name)
        : tPort<T>(name, this->FindParent()->output, true)
    {
      this->UpdateCurrentPortNameIndex();
    }
  };

  tModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
