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

#ifndef CORE__TEST__TQUEUETEST_H
#define CORE__TEST__TQUEUETEST_H

#include "rrlib/finroc_core_utils/container/tBoundedQElementContainer.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueBounded.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 */
class tQueueTest : public util::tUncopyableObject
{
public:
  class tNamedQueueable; // inner class forward declaration
public:
  class tTestWonderQueueBounded; // inner class forward declaration

public:

  tQueueTest() {}

  static void Main(::finroc::util::tArrayWrapper<util::tString>& args);

  static void Test();

  static void TestBounded();

  static void TestFast();

  static void TestFastCR();

  static void TestTL();

  class tNamedQueueable : public util::tBoundedQElementContainer
  {
  public:

    util::tString name;

  protected:

    inline void Recycle(bool recycle_content)
    {
      ::finroc::util::tBoundedQElementContainer::Recycle();
    }

    inline void RecycleContent()
    {
    }

  public:

    tNamedQueueable(const util::tString& name_);

    inline void RecycleContent(void* content)
    {
    }

    virtual const util::tString ToString() const
    {
      return name;
    }

  };

  class tTestWonderQueueBounded : public util::tWonderQueueBounded<tQueueTest::tNamedQueueable, tQueueTest::tNamedQueueable>
  {
  protected:

    virtual tQueueTest::tNamedQueueable* GetEmptyContainer()
    {
      return new tQueueTest::tNamedQueueable("anonymous");
    }

  public:

    tTestWonderQueueBounded() {}

  };

};

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TQUEUETEST_H
