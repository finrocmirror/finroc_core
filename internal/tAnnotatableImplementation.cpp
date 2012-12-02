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
/*!\file    core/internal/tAnnotatableImplementation.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tAnnotatableImplementation.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/log_messages.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tAnnotatableImplementation::tAnnotatableImplementation() :
  first_annotation(NULL)
{}

tAnnotatableImplementation::~tAnnotatableImplementation()
{
  delete first_annotation;
}

void tAnnotatableImplementation::AddAnnotation(tAnnotation& ann, const char* rtti_name)
{
  if (!first_annotation)
  {
    first_annotation = &ann;
    ann.annotated = this;
    return;
  }
  tAnnotation* current_annotation = first_annotation;
  while (current_annotation)
  {
    if (typeid(*current_annotation).name() == rtti_name)
    {
      FINROC_LOG_PRINT(ERROR, "An annotation of type ", rrlib::rtti::Demangle(rtti_name), " was already added. Not adding another one.");
      return;
    }
    if (!current_annotation->next_annotation)
    {
      current_annotation->next_annotation = &ann;
      ann.annotated = this;
      return;
    }
    current_annotation = current_annotation->next_annotation;
  }
}

void tAnnotatableImplementation::NotifyAnnotationsDelete()
{
  tAnnotation* ann = first_annotation;
  while (ann)
  {
    ann->AnnotatedObjectToBeDeleted();
    ann = ann->next_annotation;
  }
}

void tAnnotatableImplementation::NotifyAnnotationsInitialized()
{
  tAnnotation* ann = first_annotation;
  while (ann)
  {
    ann->AnnotatedObjectInitialized();
    ann = ann->next_annotation;
  }
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
