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

#ifndef core__port__net__tRemoteTypes_h__
#define core__port__net__tRemoteTypes_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/tDataTypeBase.h"
#include "rrlib/serialization/serialization.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This class aggregates information about types used in remote runtime environments.
 */
class tRemoteTypes : public util::tLogUser, public rrlib::serialization::tTypeEncoder
{
  /*! Entry in remote type register */
  class tEntry : public util::tObject
  {
    friend class tRemoteTypes;
  private:

    /*! local data type that represents the same time - null if there is no such type in local runtime environment */
    rrlib::rtti::tDataTypeBase local_data_type;

    /*! Number of local types checked to resolve type */
    int16 types_checked;

    /*! name of remote type */
    util::tString name;

  public:

    tEntry();

    tEntry(rrlib::rtti::tDataTypeBase local);

  };

private:

  /*! List with remote types - index is remote type id (=> mapping: remote type id => local type id */
  util::tSafeConcurrentlyIterableList<tEntry, util::tNoMutex, 2> types;

  /*! List with remote type update times - index is local type id */
  util::tSafeConcurrentlyIterableList<int16, util::tNoMutex, 2> update_times;

  /*! Number (max index) of local types already sent to remote runtime */
  int16 local_types_sent;

  /*! Remote Global default update time */
  int16 global_default;

  /*!
   * Init remote data type information from input stream buffer.
   * (call only once!)
   *
   * \param ci Input Stream Buffer to read from
   */
  void Deserialize(rrlib::serialization::tInputStream& ci);

  /*!
   * Serializes information about local data types
   *
   * \param co Output Stream to write information to
   */
  void SerializeLocalDataTypes(rrlib::serialization::tOutputStream& co);

public:

  tRemoteTypes();

  /*!
   * \return Remote Global default update time
   */
  inline int16 GetGlobalDefault()
  {
    return global_default;
  }

  /*!
   * \param data_type Local Data Type
   * \return Remote default minimum network update interval for this type
   */
  int16 GetTime(const rrlib::rtti::tDataTypeBase& data_type);

  /*!
   * \return Has this object been initialized?
   */
  inline bool Initialized()
  {
    return types.Size() != 0;
  }

  virtual rrlib::rtti::tDataTypeBase ReadType(rrlib::serialization::tInputStream& is);

  /*!
   * Set new update time for specified Type
   *
   * \param type_uid Type uid
   * \param new_time new update time
   */
  void SetTime(rrlib::rtti::tDataTypeBase dt, int16 new_time);

  /*!
   * \return Have new types been added since last update?
   */
  inline bool TypeUpdateNecessary()
  {
    return rrlib::rtti::tDataTypeBase::GetTypeCount() > local_types_sent;
  }

  virtual void WriteType(rrlib::serialization::tOutputStream& os, rrlib::rtti::tDataTypeBase dt);

};

} // namespace finroc
} // namespace core

#endif // core__port__net__tRemoteTypes_h__
