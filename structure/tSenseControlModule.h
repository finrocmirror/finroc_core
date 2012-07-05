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
/*!\file    tSenseControlModule.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2010-12-09
 *
 * \brief Contains tSenseControlModule
 *
 * \b tSenseControlModule
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tSenseControlModule_h_
#define _core__structure__tSenseControlModule_h_

#include "core/structure/tModuleBase.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tTask.h"

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
class tSenseControlModule : public tModuleBase
{
  class ControlTask : public rrlib::thread::tTask
  {
    tSenseControlModule *const module;
  public:
    ControlTask(tSenseControlModule *module);
    virtual void ExecuteTask();
  };

  class SenseTask : public rrlib::thread::tTask
  {
    tSenseControlModule *const module;
  public:
    SenseTask(tSenseControlModule *module);
    virtual void ExecuteTask();
  };

  finroc::core::tPortGroup *controller_input;
  finroc::core::tPortGroup *controller_output;
  ControlTask control_task;

  /*! Has any controller input port changed since last cycle? */
  bool controller_input_changed;

  finroc::core::tPortGroup *sensor_input;
  finroc::core::tPortGroup *sensor_output;
  SenseTask sense_task;

  /*! Has any sensor input port changed since last cycle? */
  bool sensor_input_changed;

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual void Control();

  virtual void Sense();

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  /*!
   * May be called in Sense() method to check
   * whether any sensor input port has changed, since last call to Sense().
   *
   * (Changed flags are reset automatically)
   */
  bool SensorInputChanged()
  {
    return sensor_input_changed;
  }

  /*!
   * May be called in Control() method to check
   * whether any controller input port has changed, since last call to Control().
   *
   * (Changed flags are reset automatically)
   */
  bool ControllerInputChanged()
  {
    return controller_input_changed;
  }

  /**
   * Port classes to use in module.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
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
  class tControllerInput : public tConveniencePort < T, tSenseControlModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tControllerInput(const ARGS&... args)
      : tConveniencePort < T, tSenseControlModule, tPort<T>>(GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tSenseControlModule* module)
    {
      return module->controller_input;
    }
  };

  template < typename T = double >
  class tControllerOutput : public tConveniencePort < T, tSenseControlModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tControllerOutput(const ARGS&... args)
      : tConveniencePort < T, tSenseControlModule, tPort<T>>(GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tSenseControlModule* module)
    {
      return module->controller_output;
    }
  };

  template < typename T = double >
  class tSensorInput : public tConveniencePort < T, tSenseControlModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tSensorInput(const ARGS&... args)
      : tConveniencePort < T, tSenseControlModule, tPort<T>>(GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tSenseControlModule* module)
    {
      return module->sensor_input;
    }
  };

  template < typename T = double >
  class tSensorOutput : public tConveniencePort < T, tSenseControlModule, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tSensorOutput(const ARGS&... args)
      : tConveniencePort < T, tSenseControlModule, tPort<T>>(GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tSenseControlModule* module)
    {
      return module->sensor_output;
    }
  };

  /*!
   * \param parent Parent
   * \param name Name of module
   * \param share_so_and_ci_ports Share sensor output and controller input ports so that they can be accessed from other runtime environments?
   */
  tSenseControlModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name, bool share_so_and_ci_ports = false);

  /*!
   * \return Parent port group of all controller inputs
   */
  inline finroc::core::tPortGroup& GetControllerInputs()
  {
    return *controller_input;
  }

  /*!
   * \return Parent port group of all controller outputs
   */
  inline finroc::core::tPortGroup& GetControllerOutputs()
  {
    return *controller_output;
  }

  /*!
   * \return Parent port group of all sensor inputs
   */
  inline finroc::core::tPortGroup& GetSensorInputs()
  {
    return *sensor_input;
  }

  /*!
   * \return Parent port group of all sensor outputs
   */
  inline finroc::core::tPortGroup& GetSensorOutputs()
  {
    return *sensor_output;
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
