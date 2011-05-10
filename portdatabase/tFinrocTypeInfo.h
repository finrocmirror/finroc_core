/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef core__portdatabase__tFinrocTypeInfo_h__
#define core__portdatabase__tFinrocTypeInfo_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"

namespace rrlib
{
namespace serialization
{
class tGenericObject;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tPortInterface;

/*!
 * \author Max Reichardt
 *
 * Additional info finroc stores for each data type
 */
class tFinrocTypeInfo : public util::tObject
{
public:

  enum tType { eSTD, eCC, eMETHOD, eTRANSACTION, eUNKNOWN };

private:

  /*! Type of data type */
  tFinrocTypeInfo::tType type;

  /*! Current default minimum network update interval for type im ms */
  int16 update_time;

  /*! "Cheap copy" index */
  int16 cc_index;

  /*! Pointer to port interface in case of method type */
  tPortInterface* port_interface;

public:

  /*! Maximum number of types */
  static const int cMAX_TYPES = 2000;

  /*! Maximum number of "cheap copy" types */
  static const int cMAX_CCTYPES = 50;

private:

  /*!
   * \return Global storage for finroc type info
   */
  static tFinrocTypeInfo* InfoArray();

public:

  tFinrocTypeInfo() :
      type(eSTD),
      update_time(-1),
      cc_index(-1),
      port_interface(NULL)
  {}

  /*!
   * Estimate data size
   *
   * \param data Transferred data
   * \return estimated size
   */
  static int EstimateDataSize(rrlib::serialization::tGenericObject* data);

  /*!
   * \param type Data Type
   * \return Finroc type info for type
   */
  inline static tFinrocTypeInfo& Get(const rrlib::serialization::tDataTypeBase& type_)
  {
    return InfoArray()[type_.GetUid()];
  }

  /*!
   * \param type Data Type uid
   * \return Finroc type info for type
   */
  inline static tFinrocTypeInfo& Get(int16 uid_)
  {
    return InfoArray()[uid_];
  }

  /*!
   * \return "Cheap copy" index
   */
  inline int16 GetCCIndex()
  {
    return cc_index;
  }

  /*!
   * \return DataType this info is about
   */
  inline rrlib::serialization::tDataTypeBase GetDataType()
  {
    short uid = static_cast<short>(this - InfoArray());
    return rrlib::serialization::tDataTypeBase::GetType(uid);
  }

  /*!
   * \param cc_type_index CC Index
   * \return Data type with this index
   */
  static rrlib::serialization::tDataTypeBase GetFromCCIndex(int16 cc_type_index);

  /*!
   * \return the portInterface
   */
  inline tPortInterface* GetPortInterface()
  {
    return port_interface;
  }

  /*!
   * \return Type of data type
   */
  inline tFinrocTypeInfo::tType GetType()
  {
    return type;
  }

  /*!
   * \return Current default minimum network update interval for type im ms
   */
  inline int16 GetUpdateTime()
  {
    return update_time;
  }

  /*!
   * Initialize finroc type info
   */
  void Init(tFinrocTypeInfo::tType type_);

  /*!
   * Initialize Type Info for RPC interface
   *
   * \param methods RPC interface
   */
  inline void Init(tPortInterface* methods)
  {
    type = eMETHOD;
    port_interface = methods;
  }

  /*!
   * \param dt Data type to look this up for
   * \return is this a "cheap copy" port data type?
   */
  inline static bool IsCCType(const rrlib::serialization::tDataTypeBase& dt)
  {
    return Get(dt).GetType() == eCC;
  }

  /*!
   * \param dt Data type to look this up for
   * \return is this a RPC interface port data type?
   */
  inline static bool IsMethodType(const rrlib::serialization::tDataTypeBase& dt)
  {
    return Get(dt).GetType() == eMETHOD;
  }

  /*!
   * \param dt Data type to look this up for
   * \return is this a standard port data type?
   */
  inline static bool IsStdType(const rrlib::serialization::tDataTypeBase& dt)
  {
    return Get(dt).GetType() == eSTD;
  }

  /*!
   * \param new_update_time Current default minimum network update interval for type im ms
   */
  void SetUpdateTime(int16 new_update_time);

};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tFinrocTypeInfo_h__
