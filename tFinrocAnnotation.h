/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef core__tFinrocAnnotation_h__
#define core__tFinrocAnnotation_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tTypedObject.h"

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
class tAnnotatable;
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Base class for all finroc element annotations.
 *
 * If annotation should be available over the net (e.g. in finstruct),
 * the serialization methods need to be overridden.
 */
class tFinrocAnnotation : public rrlib::serialization::tTypedObject
{
  friend class tFrameworkElement;
  friend class tAnnotatable;

public:

  /*! Next framework element annotation - used to build linked list - null if no more annotations */
  tFinrocAnnotation* next_annotation;

  /*! Object that is annotated - null if annotation is not attached to an object yet */
  tAnnotatable* annotated;

protected:

  /*!
   * Called when annotated object is initialized
   * (supposed to be overridden)
   */
  virtual void AnnotatedObjectInitialized()
  {
  }

  /*!
   * Called when annotated object is about to be deleted
   * (supposed to be overridden)
   */
  virtual void AnnotatedObjectToBeDeleted()
  {
  }

  /*!
   * Searches for parent with annotation of specified type
   *
   * \param fe Framework element to start searching at
   * \param type Data Type
   * \return Annotation of first parent that has one - or null
   */
  static tFinrocAnnotation* FindParentWithAnnotation(tFrameworkElement* fe, const rrlib::serialization::tDataTypeBase& type);

public:

  tFinrocAnnotation() :
      next_annotation(NULL),
      annotated(NULL)
  {}

  /*!
   * Add another annotation to framework element
   * (added to end of linked list)
   */
  void Append(tFinrocAnnotation* ann);

  virtual ~tFinrocAnnotation()
  {
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
  }

  /*!
   * \return Object that is annotated - null if annotation is not attached to an object yet
   */
  inline tAnnotatable* GetAnnotated()
  {
    return annotated;
  }

  /*!
   * initialize data type
   */
  void InitDataType();

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const
  {
    throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
  }

};

} // namespace finroc
} // namespace core

#endif // core__tFinrocAnnotation_h__
