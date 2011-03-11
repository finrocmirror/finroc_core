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

#ifndef core__buffers__tCoreInput_h__
#define core__buffers__tCoreInput_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/serialization/tInputStream.h"

#include "core/portdatabase/tSharedPtrDeleteHandler.h"

namespace finroc
{
namespace core
{
class tAbstractPort;
class tRemoteTypes;

/*!
 * \author Max Reichardt
 *
 * This is a specialized version of the StreamBuffer read view that is used
 * throughout the framework
 */
class tCoreInput : public rrlib::serialization::tInputStream
{
protected:

  /*! Source for any buffers that are needed */
  tAbstractPort* buffer_source;

  /*! RemoteTypes object that translates remote type uids in local type uids */
  tRemoteTypes* type_translation;

private:

  /*!
   * Deserialize object with variable type from stream
   *
   * \param in_inter_thread_container Deserialize "cheap copy" data in interthread container?
   * \return Buffer with read object
   */
  rrlib::serialization::tGenericObject* ReadObject(bool in_inter_thread_container);

public:

  template <typename T>
  tCoreInput(T t) :
      rrlib::serialization::tInputStream(t),
      buffer_source(NULL),
      type_translation(NULL)
  {
  }

  tCoreInput();

  /*!
   * \return Buffer Source for any buffers that are needed
   */
  inline tAbstractPort* GetBufferSource()
  {
    return buffer_source;
  }

  /*!
   * \return RemoteTypes object that translates remote type uids in local type uids
   */
  inline tRemoteTypes* GetTypeTranslation()
  {
    return type_translation;
  }

  /*!
   * Deserialize object with variable type from stream
   *
   * \return Buffer with read object (no locks)
   */
  inline rrlib::serialization::tGenericObject* ReadObject()
  {
    return ReadObject(false);
  }

  /*!
   * Deserialize object with variable type from stream - and place "cheap copy" data in "interthread container"
   *
   * \return Buffer with read object (no locks)
   */
  inline ::std::shared_ptr<rrlib::serialization::tGenericObject> ReadObjectInInterThreadContainer()
  {
    rrlib::serialization::tGenericObject* tmp = ReadObject(true);
    bool cc_type = tFinrocTypeInfo::IsCCType(tmp->GetType());

    if (cc_type)
    {
      tCCPortDataManager* mgr = (tCCPortDataManager*)tmp->GetManager();
      return std::shared_ptr<rrlib::serialization::tGenericObject>(tmp, tSharedPtrDeleteHandler<tCCPortDataManager>(mgr));
    }
    else
    {
      tPortDataManager* mgr = (tPortDataManager*)tmp->GetManager();
      mgr->GetCurrentRefCounter()->SetOrAddLocks(1);
      return std::shared_ptr<rrlib::serialization::tGenericObject>(tmp, tSharedPtrDeleteHandler<tPortDataManager>(mgr));
    }

  }

  virtual rrlib::serialization::tDataTypeBase ReadType();

  /*!
   * \param buffer_source Source for any buffers that are needed
   */
  inline void SetBufferSource(tAbstractPort* buffer_source_)
  {
    this->buffer_source = buffer_source_;
  }

  /*!
   * \param cRemoteTypes object that translates remote type uids in local type uids
   */
  inline void SetTypeTranslation(tRemoteTypes* type_translation_)
  {
    this->type_translation = type_translation_;
  }

};

} // namespace finroc
} // namespace core

#endif // core__buffers__tCoreInput_h__
