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

#ifndef core__port__net__tFrameworkElementFilter_h__
#define core__port__net__tFrameworkElementFilter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tCoreFlags.h"
#include "core/tFrameworkElement.h"
#include "rrlib/serialization/serialization.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Specifies which framework elements and annotations to serialize in a remote runtime.
 * Filters framework elements by their flags.
 */
class tFrameworkElementFilter
{
private:

  friend rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tFrameworkElementFilter& filter);

  /*! Framework element's flags that are relevant */
  uint relevant_flags;

  /*! Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator) */
  uint flag_result;

  /*! Send tags of each framework element? (TODO: maybe we'll need a generic mechanism for annotations one day) */
  bool send_tags;

public:

  /*!
   * \param relevant_flags Framework element's flags that are relevant
   * \param flag_result Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator)
   * \param send_tags Send tags of each framework element? (TODO: maybe we'll need a generic mechanism for annotations one day)
   */
  tFrameworkElementFilter(uint relevant_flags = tCoreFlags::cSTATUS_FLAGS, uint flag_result = tCoreFlags::cREADY | tCoreFlags::cPUBLISHED, bool send_tags = false);

  /*!
   * \param element Framework element
   * \param ignore_flags These flags are ignored when checking flags
   * \return Is framework element accepted by filter?
   */
  bool Accept(const tFrameworkElement& element, int ignore_flags = 0) const;

  /*!
   * \return Accept function functor
   */
  std::function<bool (const tFrameworkElement&)> AcceptFunction()
  {
    return std::bind(&tFrameworkElementFilter::Accept, this, std::placeholders::_1, 0);
  }

  /*!
   * \return Is this a filter that accepts all framework elements?
   * (e.g. the finstruct one is)
   */
  inline bool IsAcceptAllFilter() const
  {
    return (relevant_flags & (~tCoreFlags::cSTATUS_FLAGS)) == 0;
  }

  /*!
   * \return Is this a filter that only lets ports through?
   */
  inline bool IsPortOnlyFilter() const
  {
    return (relevant_flags & flag_result & tCoreFlags::cIS_PORT) > 0;
  }

  /*!
   * \return Send tags of each framework element?
   */
  inline bool SendTags() const
  {
    return send_tags;
  }
};

rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tFrameworkElementFilter& filter);

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tFrameworkElementFilter& filter);

} // namespace finroc
} // namespace core

#endif // core__port__net__tFrameworkElementFilter_h__
