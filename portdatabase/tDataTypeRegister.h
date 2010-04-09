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

#ifndef CORE__PORTDATABASE__TDATATYPEREGISTER_H
#define CORE__PORTDATABASE__TDATATYPEREGISTER_H

#include "core/portdatabase/tDataType.h"
#include "core/port/std/tPortDataImpl.h"

#include "core/portdatabase/tDataTypeUtil.h"
#include <map>
#include "core/portdatabase/tCppStdFactory.h"
#include "core/portdatabase/tTransactionTypeFactory.h"

namespace finroc
{
namespace core
{
class tPortData;
class tPortInterface;

/*!
 * \author Max Reichardt
 *
 * All data types that are available in the current runtime environment are stored in this
 * class.
 *
 * Also implements a port set with default update times for every data type.
 */
class tDataTypeRegister : public util::tUncopyableObject
{
private:

  /*! Map with data types [Uid] = Type; size is 128KB (large, but very efficient compared to java.util Maps) */
  ::finroc::util::tArrayWrapper<tDataType*> data_types;

  /*! Map for initially looking up data type from object itself */
  std::map<const char*, tDataType*> initial_lookup;

  /*! Index of next dataType that is added (normal and cc) */
  int16 next_data_type_uid;

  int16 next_cCData_type_uid;

  tDataTypeRegister();

  /*!
   * (typically called by plugin manager)
   *
   * Add data type to register.
   * Needs to be done before it can be used in ports.
   *
   * If specific indices (uids) are required they must be
   * set before adding using setNextUids
   *
   * \param dt Data type to add
   * \return Same data type as dt
   */
  tDataType* AddDataType(tDataType* dt);

public:

  /*!
   * Get and possibly add (virtual) method data type to register that can be used in interface ports
   *
   * \param name Name of method data type
   * \param methods Interface (or method set)
   */
  inline tDataType* AddMethodDataType(const util::tString& name, tPortInterface* methods)
  {
    return AddDataType(new tDataType(name, methods));
  }

  template <typename T>
  tDataType* GetDataType(const util::tString& name)
  {
    tDataType* dt = tDataTypeLookup<T>::type;
    if (dt == NULL)
    {
      bool transaction = tDataTypeUtil::GetTransactionType((T*)1000);
      dt = AddDataType(new tDataType((T*)1000, name, transaction ? (tPortDataFactory*)new tTransactionTypeFactory<T>() : (tPortDataFactory*)new tCppStdFactory<T>()));
    }
    return dt;
  }

  template <typename T>
  tDataType* GetDataType()
  {
    tDataType* dt = tDataTypeLookup<T>::type;
    return (dt != NULL) ? dt : GetDataType<T>(GetCleanClassName<T>());
  }

  //! returns simple/class name as it would be in Java (gcc specific implementation)
  template <typename T>
  static util::tString GetCleanClassName()
  {
    util::tString s(__PRETTY_FUNCTION__);
    //printf("PRETTY_FUNCTION %s\n", s.toCString());
    s = s.Substring(s.IndexOf(" = ") + 3, s.LastIndexOf("]")); // should be our class name
    //printf("Class name: %s\n", s.toCString());

    // we don't want template arguments
    if (s.Contains("<"))
    {
      s = s.Substring(0, s.IndexOf("<"));
    }

    // remove namespace
    if (s.Contains("::"))
    {
      s = s.Substring(s.LastIndexOf("::") + 2);
    }

    // remove "t" prefix
    if (islower(s.CharAt(0)))
    {
      s = s.Substring(1);
    }

    //printf("Clean Class name: %s\n", s.toCString());
    return s;
  }

  /*!
   * \return Java-style simple, clean class name (no template parameters, no namespace, no prefix)
   */
  template <typename T>
  inline static util::tString GetCleanClassName(util::tTypedClass<T>& clazz)
  {
    return GetCleanClassName<T>();
  }

  /*!
   * Get (and possibly add) standard data type from/to data type register.
   *
   * \param clazz Class that represents this data type
   * \param custom_name In case data type is created, use this name instead of class name
   * \return Data type
   */
  template <typename T>
  inline tDataType* GetDataType(util::tTypedClass<T> clazz, const util::tString& custom_name)
  {
    return GetDataType<T>(custom_name);
  }

  /*!
   * Get (and possibly add) standard data type from/to data type register.
   *
   * \param clazz Class that represents this data type
   * \return Data type
   */
  template <typename T>
  inline tDataType* GetDataType(util::tTypedClass<T> clazz)
  {
    return GetDataType<T>();
  }

  //  /**
  //   * Add data type entry for class
  //   *
  //   * \param cl Class
  //   */
  //  private static DataType addClass(Class<?> cl) {
  //    try {
  //      if ((PortData.class.isAssignableFrom(cl) || CustomSerialization.class.isAssignableFrom(cl))) {
  //        Class<PortData> dataClass = (Class<PortData>)cl;
  //        short suid = getDataTypeUid(dataClass);
  //        if (dataTypes[suid] != null) {
  //          throw new RuntimeException("There are two PortData classes with the same Uid " + suid + " (" + cl.getName() + " and " + dataTypes[suid].dataType.getName() + ").");
  //        }
  //        dataTypes[suid] = new DataType(dataClass, suid);
  //
  //        return dataTypes[suid];
  //      }
  //      return null;
  //    } catch (Exception e) {
  //      e.printStackTrace();
  //    }
  //    return null;
  //  }

  /*!
   * Get Data Type for specified uid.
   *
   * \param uid Uid
   * \return Data Type
   */
  inline tDataType* GetDataType(int16 uid)
  {
    return data_types[uid];
  }

  /*!
   * Find data type with specified name
   *
   * \param name Unique name
   * \return Data type with specified - null if none with this name exists
   */
  tDataType* GetDataType(const util::tString& name);

  /*!
   * \return Singleton instance
   */
  inline static tDataTypeRegister* GetInstance()
  {
    static tDataTypeRegister instance;
    return &instance;
  }

  /*!
   * \return maximum type uid
   */
  inline int GetMaxTypeIndex()
  {
    return next_data_type_uid;
  }

  /*!
   * lookup data type
   * (typically only called by PortData)
   *
   * \param port_data Object whose data type pointer to look up
   * \return Data type
   */
  inline tDataType* LookupDataType(tPortData* port_data)
  {
    return initial_lookup[typeid(*port_data).name()];
  }

  virtual ~tDataTypeRegister();

  /*!
   * (typically called by plugin manager)
   *
   * Set uids of data types added next.
   * (they will be incremented for the following)
   *
   * \param cc_uid next uid for "cheap-copy" types
   * \param uid next uid for ordinary types
   */
  inline void SetNextUids(int16 cc_uid, int16 uid)
  {
    next_cCData_type_uid = cc_uid;
    next_data_type_uid = uid;
  }

  /*!
   * Returns serialVersionUID of specified class (if between 0 and 32K)
   *
   * \param cl Class
   * \return serialVersionUID
   */
//  private static short getDataTypeUid(Class<? extends PortData> cl) {
//    if (!Modifier.isAbstract(cl.getModifiers())) {
//      long uid = ObjectStreamClass.lookup(cl).getSerialVersionUID();  // hopefully this will work in applets
//      if (uid > Short.MAX_VALUE || uid < 0) {
//        throw new RuntimeException("The serialVersionUID of PortData classes must lie between 0 and " + Short.MAX_VALUE + " (Class: " + cl.getSimpleName() + ")");
//      }
//      return (short)uid;
//    } else {
//      PortData.InterfaceSpec i = cl.getAnnotation(PortData.InterfaceSpec.class);
//      if (i == null) {
//        throw new RuntimeException("Interfaces must implement PortData.InterfaceSpec. " + cl.getSimpleName() + " doesn't.");
//      }
//      return i.uid();
//    }
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TDATATYPEREGISTER_H
