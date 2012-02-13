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
#include "core/tFrameworkElement.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/thread/tExecutionControl.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataTypeBase tExecutionControl::cTYPE = rrlib::rtti::tDataType<tExecutionControl>();

tExecutionControl::tExecutionControl(tStartAndPausable& implementation_) :
  implementation(&(implementation_))
{
}

tExecutionControl::tExecutionControl() :
  implementation(NULL)
{
  throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
}

namespace internal
{
struct tFindCallback
{
  void TreeFilterCallback(tFrameworkElement* fe, util::tSimpleList<tExecutionControl*>* const customParam)
  {
    tExecutionControl* ec = fe->GetAnnotation<tExecutionControl>();
    if (ec != NULL)
    {
      customParam->Add(ec);
    }
  }
};
}

void tExecutionControl::FindAll(util::tSimpleList<tExecutionControl*>& result, tFrameworkElement* fe)
{
  if (fe != NULL && (fe->IsReady()))
  {
    tFrameworkElementTreeFilter filter;
    internal::tFindCallback cb;
    filter.TraverseElementTree(fe, &cb, &result);
  }
}

void tExecutionControl::PauseAll(tFrameworkElement* fe)
{
  util::tSimpleList<tExecutionControl*> ecs;
  FindAll(ecs, fe);
  for (size_t i = 0; i < ecs.Size(); i++)
  {
    if (ecs.Get(i)->IsRunning())
    {
      ecs.Get(i)->Pause();
    }
  }
}

void tExecutionControl::StartAll(tFrameworkElement* fe)
{
  util::tSimpleList<tExecutionControl*> ecs;
  FindAll(ecs, fe);
  for (size_t i = 0; i < ecs.Size(); i++)
  {
    if (!ecs.Get(i)->IsRunning())
    {
      ecs.Get(i)->Start();
    }
  }
}

} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tExecutionControl>;
