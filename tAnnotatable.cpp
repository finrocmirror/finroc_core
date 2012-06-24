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
#include "core/tAnnotatable.h"
#include "core/tFinrocAnnotation.h"

namespace finroc
{
namespace core
{
void tAnnotatable::AddAnnotation(tFinrocAnnotation* ann)
{
  util::tLock lock1(simple_mutex);
  if (ann->GetType() == NULL)
  {
    ann->InitDataType();
    assert(((ann->GetType() != NULL)) && "Initializing data type failed");
  }
  assert(((ann->annotated == NULL)) && "Already used as annotation in other object. Not allowed (double deleteting etc.)");
  ann->annotated = this;
  if (first_annotation == NULL)
  {
    first_annotation = ann;
  }
  else
  {
    first_annotation->Append(ann);
  }
}

tAnnotatable::~tAnnotatable()
{
  // delete annotations
  tFinrocAnnotation* a = first_annotation;
  while (a != NULL)
  {
    tFinrocAnnotation* tmp = a;
    a = a->next_annotation;
    delete tmp;
  }
}

tFinrocAnnotation* tAnnotatable::GetAnnotation(const rrlib::rtti::tDataTypeBase& dt) const
{
  tFinrocAnnotation* ann = first_annotation;
  while (ann != NULL)
  {
    if (ann->GetType() == dt)
    {
      return ann;
    }
    ann = ann->next_annotation;
  }
  return NULL;
}

void tAnnotatable::NotifyAnnotationsDelete()
{
  tFinrocAnnotation* ann = first_annotation;
  while (ann != NULL)
  {
    ann->AnnotatedObjectToBeDeleted();
    ann = ann->next_annotation;
  }
}

void tAnnotatable::NotifyAnnotationsInitialized()
{
  tFinrocAnnotation* ann = first_annotation;
  while (ann != NULL)
  {
    ann->AnnotatedObjectInitialized();
    ann = ann->next_annotation;
  }
}

} // namespace finroc
} // namespace core

