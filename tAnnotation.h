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
/*!\file    core/tAnnotation.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief   Contains tAnnotation
 *
 * \b tAnnotation
 *
 * Base class for all finroc element annotations.
 *
 * If annotation should be available over the net (e.g. in finstruct),
 * serialization operators need to be implemented.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__tAnnotation_h__
#define __core__tAnnotation_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tNoncopyable.h"
#include "rrlib/rtti/tIsListType.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

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
namespace internal
{
class tAnnotatableImplementation;
}

class tFrameworkElement;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Finroc element annotation base class
/*!
 * Base class for all finroc element annotations.
 *
 * If annotation should be available over the net (e.g. in finstruct),
 * serialization operators need to be implemented and
 * tDataType<...> object initialized.
 */
class tAnnotation : public rrlib::rtti::tIsListType<false, false>, private rrlib::util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tAnnotation();

  virtual ~tAnnotation();

  /*!
   * Searches for framework element annotation of specified type
   * The search includes the annotated framework element as well as all of its parent
   *
   * \param framework_element Framework element to start searching at
   * \param type Data Type
   * \return Annotation of first parent that has one - otherwise NULL
   */
  template <typename T>
  static T* FindParentWithAnnotation(core::tFrameworkElement& framework_element)
  {
    return static_cast<T*>(FindParentWithAnnotation(framework_element, typeid(T).name()));
  }

  /*!
   * \return Object that is annotated - null if annotation is not attached to an object yet
   */
  template <typename T>
  inline T* GetAnnotated() const
  {
    return static_cast<T*>(annotated);
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class internal::tAnnotatableImplementation;

  /*! Next framework element annotation - used to build linked list - null if no more annotations */
  tAnnotation* next_annotation;

  /*! Object that is annotated - null if annotation is not attached to an object yet */
  internal::tAnnotatableImplementation* annotated;

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
   * Implementation of above
   */
  static tAnnotation* FindParentWithAnnotation(tFrameworkElement& framework_element, const char* rtti_name);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
