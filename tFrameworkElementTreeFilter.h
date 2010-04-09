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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__TFRAMEWORKELEMENTTREEFILTER_H
#define CORE__TFRAMEWORKELEMENTTREEFILTER_H

#include "finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElement.h"
#include "core/buffers/tCoreInput.h"
#include "core/tCoreFlags.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tCoreSerializable.h"

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
class tFrameworkElementTreeFilter : public util::tObject, public tCoreSerializable
{
public:
  class tCallback; // inner class forward declaration
private:

  /*! Framework element's flags that are relevant */
  int relevant_flags;

  /*! Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator) */
  int flag_result;

  /*! Qualified names of framework elements need to start with one of these in order to be published */
  ::std::tr1::shared_ptr<util::tSimpleList<util::tString> > paths;

  /*! Constant for empty string - to allow this-constructor in c++ */
  static util::tString cEMPTY_STRING;

public:

  tFrameworkElementTreeFilter();

  /*!
   * \param relevant_flags Framework element's flags that are relevant
   * \param flag_result Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator)
   */
  tFrameworkElementTreeFilter(int relevant_flags_, int flag_result_);

  /*!
   * \param relevant_flags Framework element's flags that are relevant
   * \param flag_result Result that needs to be achieved when ANDing element's flags with relevant flags (see ChildIterator)
   * \param paths Qualified names of framework elements need to start with one of these (comma-separated list of strings)
   */
  tFrameworkElementTreeFilter(int relevant_flags_, int flag_result_, const util::tString& paths_);

  /*!
   * \param element Framework element
   * \param tmp Temporary, currently unused string buffer
   * \return Is framework element accepted by filter?
   */
  bool Accept(tFrameworkElement* element, util::tStringBuilder& tmp) const;

  virtual void Deserialize(tCoreInput& is);

  /*!
   * \return Is this a filter that only lets ports through?
   */
  inline bool IsPortOnlyFilter() const
  {
    return (relevant_flags & flag_result & tCoreFlags::cIS_PORT) > 0;
  }

  virtual void Serialize(tCoreOutput& os) const;

  /*!
   * Traverse (part of) element tree.
   * Only follows primary links (no links - this way, we don't have duplicates)
   *
   * \param <T> Type of callback class
   * \param root Root element of tree
   * \param callback Callback class instance
   * \param tmp Temporary StringBuilder buffer
   */
  template <typename T>
  inline void TraverseElementTree(tFrameworkElement* root, T* callback, util::tStringBuilder& tmp) const
  {
    if (Accept(root, tmp))
    {
      callback->TreeFilterCallback(root);
    }
    const util::tArrayWrapper<tFrameworkElement::tLink*>* children = root->GetChildren();
    for (int i = 0, n = children->Size(); i < n; i++)
    {
      tFrameworkElement::tLink* link = children->Get(i);
      if (link != NULL && link->GetChild() != NULL && link->IsPrimaryLink())
      {
        TraverseElementTree(link->GetChild(), callback, tmp);
      }
    }
  }

public:

  /*!
   * Classes that use FrameworkElementTreeFilter for traversing trees, should implement this interface.
   *
   * \author Max Reichardt
   */
  class tCallback : public util::tInterface
  {
  public:

    /*!
     * When traversing trees, called for every framework element that matches criteria
     * (Method is non-virtual for efficiency reasons)
     *
     * \param fe Framework element that is currently being visited
     */
    virtual void TreeFilterCallback(tFrameworkElement* fe) = 0;

  };

};

} // namespace finroc
} // namespace core

#endif // CORE__TFRAMEWORKELEMENTTREEFILTER_H
