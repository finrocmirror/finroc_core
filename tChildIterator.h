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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__TCHILDITERATOR_H
#define CORE__TCHILDITERATOR_H

#include "core/tFrameworkElement.h"
#include "core/port/tAbstractPort.h"
#include "core/tCoreFlags.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Used to iterate over a framework element's children.
 * (Abstract base class)
 *
 * Is in a separate class, because this avoids cyclic/problematic includes
 * with inlining.
 */
class tChildIterator : public util::tObject
{
private:

  // next element to check (in array)
  tFrameworkElement::tLink* const * next_elem;

  // last element in array
  tFrameworkElement::tLink* const * last;

  /*! Relevant flags */
  int flags;

  /*! Expected result when ANDing with flags */
  int result;

protected:

  /*! FrameworkElement that is currently iterated over */
  const tFrameworkElement* cur_parent;

public:

  tChildIterator(const tFrameworkElement* parent) :
      next_elem(NULL),
      last(NULL),
      flags(0),
      result(0),
      cur_parent(NULL)
  {
    Reset(parent);
  }

  /*!
   * \param parent Framework element over whose child to iterate
   * \param flags Flags that children must have in order to be considered
   */
  tChildIterator(const tFrameworkElement* parent, int flags_) :
      next_elem(NULL),
      last(NULL),
      flags(0),
      result(0),
      cur_parent(NULL)
  {
    Reset(parent, flags_);
  }

  /*!
   * \param parent Framework element over whose child to iterate
   * \param flags Relevant flags
   * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
   */
  tChildIterator(const tFrameworkElement* parent, int flags_, int result_) :
      next_elem(NULL),
      last(NULL),
      flags(0),
      result(0),
      cur_parent(NULL)
  {
    Reset(parent, flags_, result_);
  }

  /*!
   * \param parent Framework element over whose child to iterate
   * \param flags Relevant flags
   * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
   * \param include_non_ready Include children that are not fully initialized yet?
   */
  tChildIterator(const tFrameworkElement* parent, int flags_, int result_, bool include_non_ready) :
      next_elem(NULL),
      last(NULL),
      flags(0),
      result(0),
      cur_parent(NULL)
  {
    Reset(parent, flags_, result_, include_non_ready);
  }

  /*!
   * \return Next child - or null if there are no more children left
   */
  inline tFrameworkElement* Next()
  {
    while (next_elem <= last)
    {
      tFrameworkElement::tLink* nex = *next_elem;
      if (nex != NULL && (nex->GetChild()->GetAllFlags() & flags) == result)
      {
        next_elem++;
        return nex->GetChild();
      }
      next_elem++;
    }

    return NULL;
  }

  /*!
   * \return Next child that is a port - or null if there are no more children left
   */
  inline tAbstractPort* NextPort()
  {
    while (true)
    {
      tFrameworkElement* result = Next();
      if (result == NULL)
      {
        return NULL;
      }
      if (result->IsPort())
      {
        return static_cast<tAbstractPort*>(result);
      }
    }
  }

  /*!
   * Use iterator again on same framework element
   */
  inline void Reset()
  {
    Reset(cur_parent);
  }

  /*!
   * Use Iterator for different framework element
   * (or same and reset)
   *
   * \param parent Framework element over whose child to iterate
   */
  inline void Reset(const tFrameworkElement* parent)
  {
    Reset(parent, 0, 0);
  }

  /*!
   * Use Iterator for different framework element
   * (or same and reset)
   *
   * \param parent Framework element over whose child to iterate
   * \param flags Flags that children must have in order to be considered
   */
  inline void Reset(const tFrameworkElement* parent, int flags_)
  {
    Reset(parent, flags_, flags_);
  }

  /*!
   * Use Iterator for different framework element
   * (or same and reset)
   *
   * \param parent Framework element over whose child to iterate
   * \param flags Relevant flags
   * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
   */
  inline void Reset(const tFrameworkElement* parent, int flags_, int result_)
  {
    Reset(parent, flags_, result_, false);
  }

  /*!
   * Use Iterator for different framework element
   * (or same and reset)
   *
   * \param parent Framework element over whose child to iterate
   * \param flags Relevant flags
   * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
   * \param include_non_ready Include children that are not fully initialized yet?
   */
  inline void Reset(const tFrameworkElement* parent, int flags_, int result_, bool include_non_ready)
  {
    assert((parent != NULL));
    this->flags = flags_ | tCoreFlags::cDELETED;
    this->result = result_;
    if (!include_non_ready)
    {
      flags_ |= tCoreFlags::cREADY;
      result_ |= tCoreFlags::cREADY;
    }
    cur_parent = parent;

    const util::tArrayWrapper<tFrameworkElement::tLink*>* array = parent->GetChildren();
    next_elem = array->GetPointer();
    last = (next_elem + array->Size()) - 1;

  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TCHILDITERATOR_H
