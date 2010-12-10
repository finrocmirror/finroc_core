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
#include "rrlib/finroc_core_utils/tJCBase.h"
#include "core/portdatabase/tDataType.h"

#ifndef CORE__PORT__CC__TCCINTERTHREADCONTAINER_H
#define CORE__PORT__CC__TCCINTERTHREADCONTAINER_H

#include "core/port/cc/tCCPortData.h"
#include "core/portdatabase/tTypedObjectContainer.h"
#include "rrlib/finroc_core_utils/container/tReusable.h"
#include "core/port/cc/tCCContainerBase.h"

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
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * "Lightweight" container for "cheap copy" data.
 * This container can be shared among different threads.
 * It is also very simple - no lock counting.
 * It is mainly used for queueing CCPortData.
 */
template < typename T = tCCPortData >
class tCCInterThreadContainer : public util::tReusable, public tCCContainerBase
{
public:

  /*!
   * Actual data - important: last field in this class - so offset in
   * C++ is fixed and known - regardless of template parameter
   */
  tTypedObjectContainer<T> port_data;

  // object parameter only used in Java
  tCCInterThreadContainer(tDataType* type, util::tObject* object = NULL) :
      port_data(type, object)
  {
    //System.out.println("Creating lightweight container " + toString());

    this->type = type;
    assert((GetDataPtr() == (reinterpret_cast<tCCInterThreadContainer<>*>(this))->GetDataPtr()));  // for C++ class layout safety
  }

  /*! Assign other data to this container */
  inline void Assign(const tCCPortData* other)
  {
    port_data.Assign(other);
  }

  /*! Assign data in this container to other data */
  inline void AssignTo(tCCPortData* other) const
  {
    port_data.AssignTo(other);
  }

  /*! @return Is data in this container equal to data in other container? */
  inline bool ContentEquals(const tCCPortData* other) const
  {
    return port_data.ContentEquals(other);
  }

  virtual void Deserialize(tCoreInput& is)
  {
    port_data.Deserialize(is);
  }

  virtual void Deserialize(const util::tString& s)
  {
    port_data.Deserialize(s);
  }

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node)
  {
    port_data.Deserialize(node);
  }

  /*!
   * \return Actual data
   */
  inline const T* GetData() const
  {
    return port_data.GetData();
  }

  inline T* GetData()
  {
    return port_data.GetData();
  }

  /*!
   * \return Pointer to actual data (beginning of data - important for multiple inheritance memcpy)
   */
  inline const void* GetDataPtr() const
  {
    return port_data.GetData();
  }

  inline void* GetDataPtr()
  {
    return port_data.GetData();
  }

  virtual bool IsInterThreadContainer()
  {
    return true;
  }

  /*!
   * Recyle container
   */
  inline void Recycle2()
  {
    //System.out.println("Recycling interthread buffer " + this.hashCode());
    ::finroc::util::tReusable::Recycle();
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    port_data.Serialize(os);
  }

  virtual util::tString Serialize() const
  {
    return port_data.Serialize();
  }

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const
  {
    port_data.Serialize(node);
  }

  void SetData(const T& data)
  {
    SetData(&data);
  }

  /*!
   * Assign new value to container
   *
   * \param data new value
   */
  inline void SetData(const T* data)
  {
    port_data.Assign(reinterpret_cast<const tCCPortData*>(data));
  }

  virtual const util::tString ToString() const
  {
    return util::tStringBuilder("CCInterThreadContainer: ") + port_data.ToString();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCINTERTHREADCONTAINER_H
