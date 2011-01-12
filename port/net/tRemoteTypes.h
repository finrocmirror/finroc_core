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

#ifndef CORE__PORT__NET__TREMOTETYPES_H
#define CORE__PORT__NET__TREMOTETYPES_H

#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace finroc
{
namespace core
{
class tCoreInput;
class tDataTypeRegister;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * This class aggregates information about types used in remote runtime environments.
 */
class tRemoteTypes : public util::tLogUser
{
  /*! Entry in remote type register */
  class tEntry : public util::tObject
  {
    friend class tRemoteTypes;
  private:

    /*! update time for this data type */
    int16 update_time;

    /*! local data type that represents the same time - null if there is noch such type in local runtime environment */
    tDataType* local_data_type;

  public:

    tEntry();

    tEntry(int16 time, tDataType* local);

    bool operator==(void* x)
    {
      if (x == NULL)
      {
        return local_data_type == NULL;
      }
      return x == this;
    }

  };

private:

  /*! List with remote types - index is remote type id */
  ::finroc::util::tArrayWrapper<tEntry>* types;

  /*! List with remote types - index is local type id */
  ::finroc::util::tArrayWrapper<tEntry>* types_by_local_uid;

  /*! Remote Global default update time */
  int16 global_default;

public:

  /*! Log domain for edges */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "remote_types");

  tRemoteTypes();

  virtual ~tRemoteTypes()
  {
    delete types;
  }

  /*!
   * Init remote data type information from intput stream buffer.
   * (call only once!)
   *
   * \param ci Input Stream Buffer to read from
   */
  void Deserialize(tCoreInput* ci);

  /*!
   * \return Remote Global default update time
   */
  inline int16 GetGlobalDefault()
  {
    return global_default;
  }

  /*!
   * \param uid Remote type uid
   * \return Local data type - which is identical to remote type; or null if no such type exists
   */
  tDataType* GetLocalType(int16 uid);

  /*!
   * \param data_type Local Data Type
   * \return Remote default minimum network update interval for this type
   */
  inline int16 GetTime(tDataType* data_type)
  {
    assert(((Initialized())) && "Not initialized");
    return (*(types_by_local_uid))[data_type->GetUid()].update_time;
  }

  /*!
   * \return Has this object been initialized?
   */
  inline bool Initialized()
  {
    return types != NULL;
  }

  /*!
   * Serializes information about local data types
   *
   * \param dtr DataTypeRegister to serialize
   * \param co Output Stream to write information to
   */
  static void SerializeLocalDataTypes(tDataTypeRegister* dtr, tCoreOutput* co);

  /*!
   * Set new update time for specified Type
   *
   * \param type_uid Type uid
   * \param new_time new update time
   */
  void SetTime(int16 type_uid, int16 new_time);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__NET__TREMOTETYPES_H
