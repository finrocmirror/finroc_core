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
/*!\file    tGroup.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2010-12-09
 *
 * \brief Contains tGroup
 *
 * \b tGroup
 *
 */
//----------------------------------------------------------------------
#ifndef _core__tGroup_h_
#define _core__tGroup_h_

#include "core/finstructable/tFinstructableGroup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"
#include "core/port/tPort.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/structure/tConveniencePort.h"
#include "core/structure/tStructureElementRegister.h"

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
class tGroup : public tFinstructableGroup
{
  template <typename T>
  friend class tConveniencePortBase;

  finroc::core::tPortGroup* controller_input;
  finroc::core::tPortGroup* controller_output;

  finroc::core::tPortGroup* sensor_input;
  finroc::core::tPortGroup* sensor_output;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

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
  class tControllerInput : public tConveniencePort < T, tGroup, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    tControllerInput(const ARGS&... args)
        : tConveniencePort < T, tGroup, tPort<T> > (tPortFlags::cINPUT_PROXY, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tGroup* module)
    {
      return module->controller_input;
    }
  };

  template < typename T = double >
  class tControllerOutput : public tConveniencePort < T, tGroup, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    tControllerOutput(const ARGS&... args)
        : tConveniencePort < T, tGroup, tPort<T> > (tPortFlags::cOUTPUT_PROXY, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tGroup* module)
    {
      return module->controller_output;
    }
  };

  template < typename T = double >
  class tSensorInput : public tConveniencePort < T, tGroup, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    tSensorInput(const ARGS&... args)
        : tConveniencePort < T, tGroup, tPort<T> >(tPortFlags::cINPUT_PROXY, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tGroup* module)
    {
      return module->sensor_input;
    }
  };

  template < typename T = double >
  class tSensorOutput : public tConveniencePort < T, tGroup, tPort<T> >
  {
  public:
    template<typename ... ARGS>
    tSensorOutput(const ARGS&... args)
        : tConveniencePort < T, tGroup, tPort<T>>(tPortFlags::cOUTPUT_PROXY, GetContainer, args...)
    {}

  private:
    static tFrameworkElement* GetContainer(tGroup* module)
    {
      return module->sensor_output;
    }
  };

//  void SetParameter(size_t index, const finroc::util::tString &new_value);
//  void SetParameter(const finroc::util::tString &name, const finroc::util::tString &new_value);

  template < typename T = double >
  class tStructureParameter : public finroc::core::tStructureParameter<T>, tConveniencePortBase<tGroup>
  {
  public:

    // constructors taking description as tString
    tStructureParameter(const util::tString& description)
        : finroc::core::tStructureParameter<T>(description, this->FindParent())
    {
      this->UpdateCurrentPortNameIndex();
    }

    tStructureParameter(const util::tString& description, const util::tString& default_value)
        : finroc::core::tStructureParameter<T>(description, this->FindParent(), default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NOSTRING = !std::is_same<T, util::tString>::value >
    tStructureParameter(typename std::enable_if<NOSTRING, const util::tString&>::type description, const T& default_value)
        : finroc::core::tStructureParameter<T>(description, this->FindParent(), default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const util::tString&>::type description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, this->FindParent(), default_value, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const util::tString&>::type description, const T& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, this->FindParent(), default_value, b, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    // constructors taking description as const char*
    tStructureParameter(const char* description)
        : finroc::core::tStructureParameter<T>(description, this->FindParent())
    {
      this->UpdateCurrentPortNameIndex();
    }

    tStructureParameter(const char* description, const util::tString& default_value)
        : finroc::core::tStructureParameter<T>(description, this->FindParent(), default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NOSTRING = !std::is_same<T, util::tString>::value >
    tStructureParameter(typename std::enable_if<NOSTRING, const char*>::type description, const T& default_value)
        : finroc::core::tStructureParameter<T>(description, this->FindParent(), default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const char*>::type description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, this->FindParent(), default_value, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const char*>::type description, const T& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, this->FindParent(), default_value, b, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    // constructors when relying on auto-generated descriptions in initializer list
    tStructureParameter()
        : finroc::core::tStructureParameter<T>(this->GetPortName(), this->FindParent())
    {
    }

    template < bool NOSTRING = !std::is_same<T, util::tString>::value >
    tStructureParameter(typename std::enable_if<NOSTRING, const T>::type& default_value)
        : finroc::core::tStructureParameter<T>(this->GetPortName(), this->FindParent(), default_value)
    {
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const T>::type& default_value, tUnit* unit)
        : finroc::core::tParameter<T>(this->GetPortName(), this->FindParent(), default_value, unit)
    {
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(typename std::enable_if<NUMERIC, const T>::type& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(this->GetPortName(), this->FindParent(), default_value, b, unit)
    {
    }

    // constructors taking parent and description as tString
    tStructureParameter(tGroup* parent, const util::tString& description)
        : finroc::core::tStructureParameter<T>(description, parent)
    {
      this->UpdateCurrentPortNameIndex();
    }

    tStructureParameter(tGroup* parent, const util::tString& description, const util::tString& default_value)
        : finroc::core::tStructureParameter<T>(description, parent, default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NOSTRING = !std::is_same<T, util::tString>::value >
    tStructureParameter(tGroup* parent, typename std::enable_if<NOSTRING, const util::tString&>::type description, const T& default_value)
        : finroc::core::tStructureParameter<T>(description, parent, default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(tGroup* parent, typename std::enable_if<NUMERIC, const util::tString&>::type description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, parent, default_value, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(tGroup* parent, typename std::enable_if<NUMERIC, const util::tString&>::type description, const T& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(description, parent, default_value, b, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    // constructors taking parent and description as const char*
    tStructureParameter(tGroup* parent, const char* description)
        : finroc::core::tStructureParameter<T>(this->GetPortName(), parent)
    {
      this->UpdateCurrentPortNameIndex();
    }

    tStructureParameter(tGroup* parent, const char* description, const util::tString& default_value)
        : finroc::core::tStructureParameter<T>(this->GetPortName(), parent, default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NOSTRING = !std::is_same<T, util::tString>::value >
    tStructureParameter(tGroup* parent, typename std::enable_if<NOSTRING, const char*>::type description, const T& default_value)
        : finroc::core::tStructureParameter<T>(this->GetPortName(), parent, default_value)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(tGroup* parent, typename std::enable_if<NUMERIC, const char*>::type description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(this->GetPortName(), parent, default_value, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

    template < bool NUMERIC = tPortTypeMap<T>::numeric >
    tStructureParameter(tGroup* parent, typename std::enable_if<NUMERIC, const char*>::type description, const T& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT))
        : finroc::core::tParameter<T>(this->GetPortName(), parent, default_value, b, unit)
    {
      this->UpdateCurrentPortNameIndex();
    }

  };

  tGroup(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name, const finroc::util::tString &structure_config_file);

  virtual ~tGroup();

  void* operator new(size_t size)
  {
    void* result = ::operator new(size);
    tStructureElementRegister::AddMemoryBlock(result, size);
    return result;
  }

  void* operator new[](size_t size)
  {
    assert(false && "Allocating (non-pointer) array of framework elements is not allowed.");
    throw std::bad_alloc();
  }

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
