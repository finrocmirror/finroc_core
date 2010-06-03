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

#ifndef CORE__BUFFERS__TCOREINPUT_H
#define CORE__BUFFERS__TCOREINPUT_H

#include "core/portdatabase/tDataType.h"
#include "finroc_core_utils/stream/tSource.h"
#include "core/portdatabase/tTypedObjectImpl.h"
#include "finroc_core_utils/stream/tInputStreamBuffer.h"

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
class tCoreInput : public util::tInputStreamBuffer
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
  tTypedObject* ReadObject(bool in_inter_thread_container);

public:

  tCoreInput();

  tCoreInput(::std::tr1::shared_ptr<const util::tConstSource> source);

  tCoreInput(const util::tConstSource* source);

  tCoreInput(::std::tr1::shared_ptr<util::tSource> source);

  tCoreInput(util::tSource* source);

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

  //  /**
  //   * Deserialize object from stream with fixed type
  //   *
  //   * \param to Object to read
  //   */
  //  public void readObject(TypedObject to) {
  //      readSkipOffset();
  //      if (to == null) {
  //          throw new RuntimeException("Provided object is null");
  //      } else {
  //          to.deserialize(this);
  //      }
  //  }

  /*!
   * Deserialize object with variable type from stream
   *
   * \return Buffer with read object (no locks)
   */
  inline tTypedObject* ReadObject()
  {
    return ReadObject(false);
  }

  /*!
   * Deserialize object with variable type from stream - and place "cheap copy" data in "interthread container"
   *
   * \return Buffer with read object (no locks)
   */
  inline tTypedObject* ReadObjectInInterThreadContainer()
  {
    return ReadObject(true);
  }

  /*!
   * \return Deserialized data type (using type translation lookup table)
   */
  tDataType* ReadType();

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

#endif // CORE__BUFFERS__TCOREINPUT_H
