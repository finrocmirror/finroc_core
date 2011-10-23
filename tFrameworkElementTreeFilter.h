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

#ifndef core__tFrameworkElementTreeFilter_h__
#define core__tFrameworkElementTreeFilter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tCoreFlags.h"
#include "core/tFrameworkElement.h"
#include "rrlib/serialization/tSerializable.h"

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
/*!
 * \author Max Reichardt
 *
 * Filters framework elements by their flags and their qualified names.
 *
 * Can be used to efficiently traverse trees of framework elements.
 */
class tFrameworkElementTreeFilter : public rrlib::serialization::tSerializable
{
private:

  /*! Framework element's flags that are relevant */
  uint relevant_flags;

  /*! Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator) */
  uint flag_result;

  /*! Qualified names of framework elements need to start with one of these in order to be published */
  std::shared_ptr<util::tSimpleList<util::tString> > paths;

  /*! Constant for empty string - to allow this-constructor in c++ */
  static const util::tString& GetEmptyString();

public:

  tFrameworkElementTreeFilter();

  /*!
   * \param relevant_flags Framework element's flags that are relevant
   * \param flag_result Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator)
   */
  tFrameworkElementTreeFilter(uint relevant_flags_, uint flag_result_);

  /*!
   * \param relevant_flags Framework element's flags that are relevant
   * \param flag_result Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator)
   * \param paths Qualified names of framework elements need to start with one of these (comma-separated list of strings)
   */
  tFrameworkElementTreeFilter(uint relevant_flags_, uint flag_result_, const util::tString& paths_);

  /*!
   * \param element Framework element
   * \param tmp Temporary, currently unused string buffer
   * \param ignore_flags These flags are ignored when checking flags
   * \return Is framework element accepted by filter?
   */
  bool Accept(tFrameworkElement* element, util::tStringBuilder& tmp, int ignore_flags = 0) const;

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

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

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

  /*!
   * Traverse (part of) element tree.
   * Only follows primary links (no links - this way, we don't have duplicates)
   * (creates temporary StringBuilder => might not be suitable for real-time)
   *
   * \param <T> Type of callback class
   * \param root Root element of tree
   * \param callback Callback class instance (needs to have method 'TreeFilterCallback(tFrameworkElement* fe, P customParam)')
   * \param custom_param Custom parameter
   */
  template <typename T, typename P>
  inline void TraverseElementTree(tFrameworkElement* root, T* callback, const P& custom_param) const
  {
    util::tStringBuilder sb;
    TraverseElementTree(root, callback, custom_param, sb);
  }

  /*!
   * Traverse (part of) element tree.
   * Only follows primary links (no links - this way, we don't have duplicates)
   *
   * \param <T> Type of callback class
   * \param root Root element of tree
   * \param callback Callback class instance (needs to have method 'TreeFilterCallback(tFrameworkElement* fe, P customParam)')
   * \param custom_param Custom parameter
   * \param tmp Temporary StringBuilder buffer
   */
  template <typename T, typename P>
  inline void TraverseElementTree(tFrameworkElement* root, T* callback, const P& custom_param, util::tStringBuilder& tmp) const
  {
    if (Accept(root, tmp))
    {
      callback->TreeFilterCallback(root, custom_param);
    }
    const util::tArrayWrapper<tFrameworkElement::tLink*>* children = root->GetChildren();
    for (int i = 0, n = children->Size(); i < n; i++)
    {
      tFrameworkElement::tLink* link = children->Get(i);
      if (link != NULL && link->GetChild() != NULL && link->IsPrimaryLink())
      {
        TraverseElementTree(link->GetChild(), callback, custom_param, tmp);
      }
    }
  }

};

} // namespace finroc
} // namespace core

#endif // core__tFrameworkElementTreeFilter_h__
