//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    core/tAnnotatable.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief   Contains tAnnotatable
 *
 * \b tAnnotatable
 *
 * Abstract base class for classes that can be annotated with Finroc annotations
 * (classes derived from tAnnotation)
 *
 */
//----------------------------------------------------------------------
#ifndef __core__tAnnotatable_h__
#define __core__tAnnotatable_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/internal/tAnnotatableImplementation.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
class tAnnotatableBase {};

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Annotatable class
/*!
 * Abstract base class for classes that can be annotated with Finroc annotations
 * (classes derived from tAnnotation).
 *
 * The C++ data type is used to lookup annotations.
 * => max. one annotation of a specific C++ data type may be added.
 */
class tAnnotatable : public internal::tAnnotatableImplementation
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tAnnotatable()
  {}

  /*!
   * Attach annotation to this object
   *
   * \tparam TAnnotation Annotation type to add annotation with (used for lookup later; may also be base class)
   * \param ann Annotation to add. It will be automatically deleted when this tAnnotatable object is.
   */
  template <typename TAnnotation>
  void AddAnnotation(TAnnotation& ann)
  {
    static_assert(static_cast<void*>(&ann) == &static_cast<tAnnotation&>(ann), "tAnnotation must be first parent class when using multiple inheritance");
    internal::tAnnotatableImplementation::AddAnnotation(ann, typeid(TAnnotation).name());
  }

  /*!
   * Create annotation of specified type and attach it to this object.
   *
   * \tparam TAnnotation Annotation type to create and attach (type is used for lookup later)
   * \param constructor_arguments Constructor arguments that are forwarded to TAnnotation constructor
   * \return Annotation that was created. It will be automatically deleted when this tAnnotatable object is.
   */
  template <typename TAnnotation, typename... TArguments>
  TAnnotation& EmplaceAnnotation(TArguments && ... constructor_arguments)
  {
    TAnnotation* created_annotation = new TAnnotation(constructor_arguments...);
    AddAnnotation(*created_annotation);
    return *created_annotation;
  }

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \tparam TAnnotation Annotation type (with which annotation was added)
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  template <typename TAnnotation>
  TAnnotation* GetAnnotation() const
  {
    return static_cast<TAnnotation*>(internal::tAnnotatableImplementation::GetAnnotation(typeid(TAnnotation).name()));
  }

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \param rtti_name Annotation type name as obtained from C++ RTTI (typeid(...).name())
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  tAnnotation* GetAnnotation(const char* rtti_name) const
  {
    return internal::tAnnotatableImplementation::GetAnnotation(rtti_name);
  }

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  /*!
   * Notify annotations that object is to be deleted
   */
  void NotifyAnnotationsDelete()
  {
    internal::tAnnotatableImplementation::NotifyAnnotationsDelete();
  }

  /*!
   * Notify annotations that object has been initialized
   */
  void NotifyAnnotationsInitialized()
  {
    internal::tAnnotatableImplementation::NotifyAnnotationsInitialized();
  }

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
