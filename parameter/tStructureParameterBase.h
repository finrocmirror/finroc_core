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

#ifndef core__parameter__tStructureParameterBase_h__
#define core__parameter__tStructureParameterBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/portdatabase/sSerializationHelper.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/serialization/tSerializable.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Structure Parameter class
 * (Generic base class without template type)
 */
class tStructureParameterBase : public rrlib::serialization::tSerializable
{
  friend class tStructureParameterList;
private:

  /*! Name of parameter */
  util::tString name;

  /*! DataType of parameter */
  rrlib::serialization::tDataTypeBase type;

protected:

  /*! Current parameter value (in CreateModuleAction-prototypes this is null) - Standard type */
  tPortDataManager* value;

  /*! Current parameter value (in CreateModuleAction-prototypes this is null) - CC type */
  tCCPortDataManager* cc_value;

  /*! Index in parameter list */
  int list_index;

public:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "parameters");

private:

  /*!
   * Create buffer of specified type
   * (and delete old buffer)
   *
   * \param type Type
   */
  void CreateBuffer(rrlib::serialization::tDataTypeBase type_);

  /*!
   * \return Is this a remote parameter?
   */
  inline bool RemoteValue()
  {
    return false;
  }

protected:

  /*!
   * Delete port buffer
   */
  void DeleteBuffer();

public:

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param constructor_prototype Is this a CreteModuleActionPrototype (no buffer will be allocated)
   */
  tStructureParameterBase(const util::tString& name_, rrlib::serialization::tDataTypeBase type_, bool constructor_prototype);

  /*!
   * (should be overridden by subclasses)
   * \return Deep copy of parameter (without value)
   */
  virtual tStructureParameterBase* DeepCopy()
  {
    throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
  }

  virtual ~tStructureParameterBase()
  {
    DeleteBuffer();
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  const char* GetLogDescription()
  {
    return name.GetCString();
  }

  /*!
   * \return Name of parameter
   */
  inline util::tString GetName()
  {
    return name;
  }

  /*!
   * \return DataType of parameter
   */
  inline rrlib::serialization::tDataTypeBase GetType()
  {
    return type;
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  /*!
   * \return Value serialized as string (reverse operation to set)
   */
  inline util::tString SerializeValue()
  {
    return sSerializationHelper::TypedStringSerialize(type, ValPointer());
  }

  /*!
   * \param s serialized as string
   */
  virtual void Set(const util::tString& s);

  /*!
   * (Internal helper function to make expressions shorter)
   *
   * \return value or ccValue, depending on data type
   */
  inline rrlib::serialization::tGenericObject* ValPointer() const
  {
    return value != NULL ? value->GetObject() : cc_value->GetObject();
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameterBase_h__
