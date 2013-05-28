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
/*!\file    core/internal/tAnnotatableImplementation.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief   Contains tAnnotatableImplementation
 *
 * \b tAnnotatableImplementation
 *
 * Implementation of tAnnotable class.
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tAnnotatableImplementation_h__
#define __core__internal__tAnnotatableImplementation_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tAnnotation.h"

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
class tAnnotatable;

namespace internal
{

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Implementation of tAnnotable class
/*!
 * Implementation of tAnnotable class.
 */
class tAnnotatableImplementation
{

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class finroc::core::tAnnotatable;

  tAnnotatableImplementation();

  ~tAnnotatableImplementation();

  /*!
   * Attach annotation to this object
   *
   * \param ann Annotation to add
   * \param rtti_name Rtti name of annotation type to add
   */
  void AddAnnotation(tAnnotation& ann, const char* rtti_name);

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \param rtti_name Annotation type name as obtained from C++ RTTI (typeid(...).name())
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  inline tAnnotation* GetAnnotation(const char* rtti_name) const
  {
    tAnnotation* ann = first_annotation;
    while (ann)
    {
      if (typeid(*ann).name() == rtti_name)
      {
        return ann;
      }
      ann = ann->next_annotation;
    }
    return NULL;
  }

  /*!
   * Notify annotations that object is to be deleted
   */
  void NotifyAnnotationsDelete();

  /*!
   * Notify annotations that object has been initialized
   */
  void NotifyAnnotationsInitialized();

  /*!
   * First element of framework element annotation linked list
   * Annotations may be changed - but not deleted.
   */
  tAnnotation* first_annotation;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
