/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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

#ifndef core__port__rpc__method__tPortInterface_h__
#define core__port__rpc__method__tPortInterface_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"

namespace finroc
{
namespace core
{
class tAbstractMethod;

/*!
 * \author Max Reichardt
 *
 * Port Interface.
 * A set of methods that can be registered as a method data type at DataTypeRegister
 */
class tPortInterface : public util::tUncopyableObject
{
private:

  /*! List of methods in interface */
  ::std::auto_ptr<util::tSimpleList<tAbstractMethod*> > methods;

public:

  /*! Data type for this port interface - the last one in case there are multiple (e.g. for different types of blackboards) - set by DataTypeRegister */
  rrlib::serialization::tDataTypeBase my_type;

  /*! Name of port interface */
  util::tString name;

  tPortInterface(const util::tString& name_);

  void AddMethod(tAbstractMethod* m);

  /*!
   * \param method Method
   * \return Does port interface contain method?
   */
  inline bool ContainsMethod(tAbstractMethod* method)
  {
    return methods->Contains(method);
  }

  /*!
   * \return Data type of this port interface (must have been set before)
   */
  inline const rrlib::serialization::tDataTypeBase GetDataType()
  {
    assert((my_type != NULL));
    return my_type;
  }

  /*!
   * \param id Method id
   * \return Method with specified id in this interface
   */
  inline tAbstractMethod* GetMethod(size_t id)
  {
    assert((id < methods->Size()));
    return methods->Get(id);
  }

  /*!
   * (Should only be called by DataType class)
   *
   * \param data_type Data type that has this port interface
   */
  inline void SetDataType(const rrlib::serialization::tDataTypeBase& data_type)
  {
    assert((tFinrocTypeInfo::Get(data_type).GetPortInterface() == this));
    my_type = data_type;
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__method__tPortInterface_h__
