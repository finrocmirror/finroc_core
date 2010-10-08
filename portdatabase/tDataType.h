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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORTDATABASE__TDATATYPE_H
#define CORE__PORTDATABASE__TDATATYPE_H

#include "finroc_core_utils/stream/tOutputStreamBuffer.h"
#include "core/port/std/tCCDataList.h"
#include "core/port/std/tPortDataList.h"
#include "core/portdatabase/tListTypeFactory.h"
#include "core/portdatabase/tPortDataFactory.h"
#include "core/portdatabase/tTypedObjectImpl.h"

#include "core/portdatabase/tDataTypeUtil.h"
#include <boost/type_traits/is_base_of.hpp>

namespace finroc
{
namespace core
{
class tPortInterface;

/*!
 * \author Max Reichardt
 *
 * This class represents a data type that can be used in ports.
 *
 * It is a single entry in the data type register.
 */
class tDataType : public util::tUncopyableObject
{
public:

  enum tType { eSTD, eCC, eMETHOD, eTRANSACTION, eSTD_LIST, eCC_LIST };

  friend class tDataTypeRegister;
private:

  /*! Uid of data type - set by DataTypeRegister */
  int16 data_type_uid;

  /*! Type of data type */
  tDataType::tType type;

  //Offset of data in CCPortDataContainer
  //static size_t ccPortDataOffset;

  const char* const rtti_name; // pointer to RTTI name - unambiguous according to gcc docs

  /*! Methods for serialization */
  //public enum SerializationMethod { Custom, Memcpy }

  /*! Is this a custom serializable data type (currently prerequisite) */
  //@Const private final SerializationMethod serialization = SerializationMethod.Custom;

  /*! Current default minimum network update interval for type im ms */
  int16 update_time;

  /*! Factory used to instantiate this data type */
  ::std::auto_ptr<tPortDataFactory> factory;

  /*! Name of data type */
  util::tString name;

  /*! Related data type - custom info for special purposes (template parameters, related method calls) */
  tDataType* related_type;

  /*! List type of this data type (STD and CC types only) */
  tDataType* list_type;

  /*! Element type of this type (STD_LIST and CC_LIST types only) */
  tDataType* element_type;

  /*! An integer to store some custom data in */
  int custom_int;

  /*! For interface data types: set of methods */
  tPortInterface* methods;

public:
  /*implements CoreSerializable*/

  /*! The first n entries are reserved for CHEAP_COPYABLE_TYPES */
  static const size_t cMAX_CHEAP_COPYABLE_TYPES = 200u;

  /*! This is the number of type entries - must be smaller the Short.MAX_VALUE */
  static const size_t cMAX_TYPES = 2000u;

  /*!
   * \param data_type UID of other datatype
   * \return Is value with other datatype assignable to variable with this type?
   */
  inline bool Accepts(tDataType* data_type)
  {
    return this == data_type;
  }

  //  @JavaOnly
  //  public DataType(/*short uid,*/ String name, @AutoPtr PortDataFactory factory, boolean ccType/*, SerializationMethod serialization*/) {
  //      //this.serialization = serialization;
  //      //dataTypeUid = uid;
  //      javaClass = null;
  //      this.factory = factory;
  //      this.ccType = ccType
  //      ;
  //  }

  /*!
   * \return Create new instance of data type
   */
  inline tTypedObject* CreateInstance()
  {
    if (IsListType())
    {
      assert((element_type != NULL));
      return factory->Create(element_type, false);
    }
    else
    {
      return factory->Create(this, false);
    }
  }

  /*!
   * \return Create new "inter thread" instance of data type
   */
  inline tTypedObject* CreateInterThreadInstance()
  {
    assert((IsCCType()));
    return factory->Create(this, true);
  }

  /*!
   * \return Create new "inter thread" instance of data type
   */
  inline tTypedObject* CreateTransactionInstance()
  {
    assert((type == tDataType::eTRANSACTION));
    return factory->Create(this, true);
  }

  inline static size_t EstimateDataSize(tTypedObject* data)
  {
    if (data->GetType()->IsCCType())
    {
      return data->GetType()->memcpy_size;
    }
    else
    {
      return 4096u;  // very imprecise... but doesn't matter currently
    }
  }

  inline int GetCustomInt()
  {
    return custom_int;
  }

  /*!
   * \return Element type (in list types)
   */
  inline tDataType* GetElementType()
  {
    return element_type;
  }

  /*!
   * \return List type (for standard and cc types)
   */
  inline tDataType* GetListType()
  {
    return list_type;
  }

  /*!
   * \return Unique name of data type
   */
  inline util::tString GetName()
  {
    return name;
  }

  /*!
   * \return PortInterface, if this is a method type - otherwise null
   */
  inline tPortInterface* GetPortInterface()
  {
    return methods;
  }

  //  @Override
  //  public void deserialize(CoreInput is) {
  //      throw new RuntimeException("Not intended");
  //  }
  //
  //  @Override
  //  public void serialize(CoreBuffer os) {
  //      PortInfo.serializeDataType(this, os);
  //  }

  /*!
   * \return Related data type - custom info for special purposes
   */
  inline tDataType* GetRelatedType()
  {
    return related_type;
  }

  // some additional c++-specific information
  const size_t virtual_offset; // virtual destructor? - offset (to not duplicate vtable-pointer with memcpy)
  const size_t sizeof_;
  //const size_t memcpyOffset;
  const size_t memcpy_size;
  //const size_t customSerializationOffset; // memory layout

  //DataType** staticLookup; // pointer to DataTypeLookup template instance for this type

  // Function for determining type of datatype (see enum above)
  template<typename T>
  static tType GetDataTypeType(T* dummy)
  {
    if (tDataTypeUtil::GetCCType(dummy))
    {
      return eCC;
    }
    else if (tDataTypeUtil::GetTransactionType(dummy))
    {
      return eTRANSACTION;
    }
    return eSTD;
  }

  template <typename T, bool cSTD>
  struct tListHelper
  {
    typedef tCCDataList<T> tListType;
  };

  template <typename T>
  struct tListHelper<T, true>
  {
    typedef tPortDataList<T> tListType;
  };

  template <typename T>
  tDataType(T* dummy, util::tString name_, tPortDataFactory* factory_) :
      data_type_uid(-1),
      type(GetDataTypeType(dummy)),
      //serializationMethod(getSerialization(dummy)),
      rtti_name(typeid(T).name()),
      update_time(-1),
      factory(factory_),
      name(name_),
      related_type(NULL),
      list_type(NULL),
      element_type(NULL),
      custom_int(0),
      methods(NULL),
      virtual_offset(tDataTypeUtil::HasVTable(dummy) ? sizeof(void*) : 0),
      sizeof_(sizeof(T)),
      //staticLookup(&(DataTypeLookup<T>::type))
      //memcpyOffset(virtualOffset + (isCCType() ? ccPortDataOffset : 0)),
      memcpy_size(sizeof(T) - virtual_offset)
      //customSerializationOffset(serializationMethod == Custom ? ((char*)static_cast<util::CustomSerialization>(dummy)) - ((char*)dummy) : -700000000)
  {
    tDataTypeLookup<T>::type = this;

    if (type == eCC || type == eSTD)
    {
      list_type = new tDataType(dummy, name_ + " List", this);
    }
  }

  // for "virtual"/method call data types
  tDataType(util::tString name_, tPortInterface* methods_) :
      data_type_uid(-1),
      type(eMETHOD),
      rtti_name(NULL),
      update_time(-1),
      factory(NULL),
      name(name_),
      related_type(NULL),
      list_type(NULL),
      element_type(NULL),
      custom_int(0),
      methods(methods_),
      virtual_offset(0),
      sizeof_(0),
      memcpy_size(0)
  {}

  // for list types
  template <typename T>
  tDataType(T* dummy, util::tString name_, tDataType* el_type) :
      data_type_uid(-1),
      type(eMETHOD),
      rtti_name(NULL),
      update_time(-1),
      factory(new tListTypeFactory<typename tListHelper<T, boost::is_base_of<tPortData, T>::value >::tListType>()),
      name(name_),
      related_type(NULL),
      list_type(NULL),
      element_type(el_type),
      custom_int(0),
      methods(NULL),
      virtual_offset(0),
      sizeof_(0),
      memcpy_size(0)
  {
    tDataTypeLookup<typename tListHelper<T, boost::is_base_of<tPortData, T>::value >::tListType>::type = this;
  }

  //  SerializationMethod getSerialization(CoreSerializable* cs) {
  //      return Custom;
  //  }
  //  SerializationMethod getSerialization(void* cs) {
  //      return Memcpy;
  //  }

  /*!
   * \return Uid of data type
   */
  inline int16 GetUid()
  {
    return data_type_uid;
  }

  /*!
   * \return Current default minimum network update interval for type im ms
   */
  inline int16 GetUpdateTime()
  {
    return update_time;
  }

  /*!
   * \return Is this a "cheap copy" data type
   */
  inline bool IsCCType() const
  {
    return type == tDataType::eCC;
  }

  /*!
   * Can object of this data type be converted to specified type?
   *
   * \param data_type Other type
   * \return Answer
   */
  inline bool IsConvertibleTo(tDataType* data_type)
  {
    return data_type == this;
  }

  /*!
   * \return Is this a list type?
   */
  inline bool IsListType()
  {
    return type == tDataType::eCC_LIST || type == tDataType::eSTD_LIST;
  }

  /*!
   * \return Is this a method/interface type
   */
  inline bool IsMethodType() const
  {
    return factory.get() == NULL;
  }

  /*!
   * \return Is this a "standard" type
   */
  inline bool IsStdType() const
  {
    return type == tDataType::eSTD || type == tDataType::eCC_LIST || type == tDataType::eSTD_LIST;
  }

  /*!
   * \return Is this a transaction data type?
   */
  inline bool IsTransactionType()
  {
    return type == tDataType::eTRANSACTION;
  }

  inline void SetCustomInt(int custom_int_)
  {
    assert((this->custom_int == 0));
    this->custom_int = custom_int_;
  }

  void DirectSerialize(void* ccdata, util::tOutputStreamBuffer* co)
  {
    assert(IsCCType());
    co->Write(((char*)ccdata) + virtual_offset, memcpy_size);
  }

  /*!
   * (May only be set once)
   *
   * \param related_type Related data type - custom info for special purposes
   */
  inline void SetRelatedType(tDataType* related_type_)
  {
    assert((this->related_type == NULL));
    this->related_type = related_type_;
  }

  /*!
   * (Should only be called by DataTypeRegister)
   *
   * \param uid Uid of data type
   */
  inline void SetUid(int16 uid)
  {
    assert((data_type_uid == -1));
    data_type_uid = uid;
  }

  /*!
   * \param new_update_time Current default minimum network update interval for type im ms
   */
  void SetUpdateTime(int16 new_update_time);

  virtual const util::tString ToString() const
  {
    return name;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TDATATYPE_H
