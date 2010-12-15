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
#include "core/portdatabase/tDataType.h"

#ifndef CORE__PARAMETER__TSTRUCTUREPARAMETERBASE_H
#define CORE__PARAMETER__TSTRUCTUREPARAMETERBASE_H

#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/portdatabase/sSerializationHelper.h"
#include "core/portdatabase/tTypedObject.h"
#include "core/portdatabase/tCoreSerializable.h"

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
class tPortData;
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * Structure Parameter class
 * (Generic base class without template type)
 */
class tStructureParameterBase : public tCoreSerializable
{
  friend class tStructureParameterList;
private:

  /*! Name of parameter */
  util::tString name;

  /*! DataType of parameter */
  tDataType* type;

protected:

  /*! Current parameter value (in CreateModuleAction-prototypes this is null) - Standard type */
  tPortData* value;

  /*! Current parameter value (in CreateModuleAction-prototypes this is null) - CC type */
  tCCInterThreadContainer<>* cc_value;

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
  void CreateBuffer(tDataType* type_);

  /*!
   * \return Log description
   */
  inline util::tString GetLogDescription()
  {
    return name;
  }

  /*!
   * \return Is this a remote parameter?
   */
  inline bool RemoteValue()
  {
    return false;
  }

  /*!
   * (Internal helper function to make expressions shorter)
   *
   * \return value or ccValue, depending on data type
   */
  inline tTypedObject* ValPointer() const
  {
    return type->IsStdType() ? static_cast<tTypedObject*>(value) : static_cast<tTypedObject*>(cc_value);
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
  tStructureParameterBase(const util::tString& name_, tDataType* type_, bool constructor_prototype);

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

  virtual void Deserialize(tCoreInput& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

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
  inline tDataType* GetType()
  {
    return type;
  }

  virtual void Serialize(tCoreOutput& os) const;

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

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TSTRUCTUREPARAMETERBASE_H
