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

#ifndef core__tAnnotatable_h__
#define core__tAnnotatable_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace rrlib
{
namespace serialization
{
class tDataTypeBase;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tFinrocAnnotation;

/*!
 * \author Max Reichardt
 *
 * Abstract base class for classes that can be annotated with
 * an arbitrary number of FinrocAnnotations.
 */
class tAnnotatable : public util::tLogUser
{
private:

  /*!
   * First element of framework element annotation linked list
   * Annotations may be changed - but not deleted.
   */
  tFinrocAnnotation* first_annotation;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_mutex;

protected:

  /*!
   * Notify annotations that object is to be deleted
   */
  void NotifyAnnotationsDelete();

  /*!
   * Notify annotations that object has been initialized
   */
  void NotifyAnnotationsInitialized();

public:

  tAnnotatable() :
      first_annotation(NULL),
      obj_mutex()
  {}

  /*!
   * Add annotation to this framework element
   *
   * \param ann Annotation
   */
  void AddAnnotation(tFinrocAnnotation* ann);

  virtual ~tAnnotatable();

  /*!
   * Get annotation of specified type
   *
   * \param type Data type of annotation we're looking for
   */
  tFinrocAnnotation* GetAnnotation(const rrlib::serialization::tDataTypeBase& dt);

};

} // namespace finroc
} // namespace core

#endif // core__tAnnotatable_h__
