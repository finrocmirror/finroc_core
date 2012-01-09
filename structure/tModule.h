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

  finroc::core::tPortGroup *input;
  finroc::core::tPortGroup *output;
  UpdateTask update_task;

  /*! Has any input port changed since last cycle? */
  bool input_changed;

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
   * whether any input port has changed, since last call to Update().
   *
   * (Changed flags are reset automatically)
   */
  bool InputChanged()
  {
    return input_changed;
  }

  /**
   * Port classes to use in module.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, description needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * So...
   *
   * The first argument is interpreted as port name if it is a string. Any further string argument is interpreted as config entry (relevant for parameters only).
   * A framework element pointer is interpreted as parent.
   * unsigned int arguments are interpreted as flags.
   * int argument is interpreted as queue length.
   * tBounds<T> are port's bounds.
   * tUnit argument is port's unit.
   * int16/short argument is interpreted as minimum network update interval.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when port has numeric or string type.
   * There we have these rules:
   *
   * string type: The first argument is interpreted as port name if it is a string.
   *              A further string argument is interpreted as default_value. Another one as config entry.
   * numeric type: The first numeric argument is interpreted as default_value.
   */
  template < typename T = double >
  class tInput : public tConveniencePort < T, tModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tInput(const ARGS&... args)
        : tConveniencePort < T, tModule, tPort<T>>(tPortFlags::cINPUT_PORT, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tModule* module)
    {
      return module->input;
    }
  };

  template < typename T = double >
  class tOutput : public tConveniencePort < T, tModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tOutput(const ARGS&... args)
        : tConveniencePort < T, tModule, tPort<T>>(tPortFlags::cOUTPUT_PORT, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tModule* module)
    {
      return module->output;
    }
  };

  tModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name);

  /*!
   * \return Parent port group of all inputs
   */
  inline finroc::core::tPortGroup& GetInputs()
  {
    return *input;
  }

  /*!
   * \return Parent port group of all outputs
   */
  inline finroc::core::tPortGroup& GetOutputs()
  {
    return *output;
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
