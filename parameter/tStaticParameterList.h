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

#ifndef core__parameter__tStaticParameterList_h__
#define core__parameter__tStaticParameterList_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataType.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tPlugins.h"
#include "core/tFinrocAnnotation.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
class tStaticParameterBase;
class tFrameworkElement;
class tConstructorParameters;
class tCreateFrameworkElementAction;

/*!
 * \author Max Reichardt
 *
 * List of static parameters
 */
class tStaticParameterList : public tFinrocAnnotation
{
private:

  /*! List of parameters */
  util::tSimpleList<tStaticParameterBase*> parameters;

  /*!
   * Index of CreateModuleAction that was used to create framework element
   * (typically only set when created with finstruct)
   */
  int create_action;

public:

  /*! Data Type */
  static rrlib::serialization::tDataType<tStaticParameterList> cTYPE;

  /*! Empty parameter list */
  static tStaticParameterList cEMPTY;

private:

  /*! Clear list (deletes parameters) */
  void Clear();

public:

  // slightly ugly... but safe
  tStaticParameterList(tStaticParameterBase* p1, tStaticParameterBase* p2 = NULL, tStaticParameterBase* p3 = NULL,
                       tStaticParameterBase* p4 = NULL, tStaticParameterBase* p5 = NULL, tStaticParameterBase* p6 = NULL,
                       tStaticParameterBase* p7 = NULL, tStaticParameterBase* p8 = NULL, tStaticParameterBase* p9 = NULL,
                       tStaticParameterBase* p10 = NULL, tStaticParameterBase* p11 = NULL, tStaticParameterBase* p12 = NULL,
                       tStaticParameterBase* p13 = NULL, tStaticParameterBase* p14 = NULL, tStaticParameterBase* p15 = NULL,
                       tStaticParameterBase* p16 = NULL, tStaticParameterBase* p17 = NULL, tStaticParameterBase* p18 = NULL,
                       tStaticParameterBase* p19 = NULL, tStaticParameterBase* p20 = NULL) :
    tFinrocAnnotation(),
    parameters(),
    create_action(-1)
  {
    Add(p1);
    Add(p2);
    Add(p3);
    Add(p4);
    Add(p5);
    Add(p6);
    Add(p7);
    Add(p8);
    Add(p9);
    Add(p10);
    Add(p11);
    Add(p12);
    Add(p13);
    Add(p14);
    Add(p15);
    Add(p16);
    Add(p17);
    Add(p18);
    Add(p19);
    Add(p20);
  }

  tStaticParameterList();

  /*!
   * Add parameter to list
   *
   * \param param Parameter
   */
  void Add(tStaticParameterBase* param);

  virtual ~tStaticParameterList()
  {
    Clear();
    ;
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  // currently only used in FinstructableGroup
  void Deserialize(const rrlib::xml2::tXMLNode& node, bool finstruct_context);

  /*!
   * \param i Index
   * \return Parameter with specified index
   */
  inline tStaticParameterBase* Get(int i) const
  {
    return parameters.Get(i);
  }

  tFrameworkElement* GetAnnotated();

  /*!
   * \return Index of CreateModuleAction that was used to create framework element
   */
  inline int GetCreateAction() const
  {
    return create_action;
  }

  /*!
   * Get or create StaticParameterList for Framework element
   *
   * \param fe Framework element
   * \return StaticParameterList
   */
  static tStaticParameterList* GetOrCreate(tFrameworkElement* fe);

  /*!
   * If this is constructor parameter prototype: create instance that can be filled with values
   * (More or less clones parameter list (deep-copy without values))
   *
   * \return Cloned list
   */
  tConstructorParameters* Instantiate() const;

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  // currently only used in FinstructableGroup
  void Serialize(rrlib::xml2::tXMLNode& node, bool finstruct_context) const;

  /*!
   * \param create_action CreateModuleAction that was used to create framework element
   */
  inline void SetCreateAction(tCreateFrameworkElementAction* create_action_)
  {
    assert((this->create_action == -1));
    this->create_action = tPlugins::GetInstance()->GetModuleTypes().IndexOf(create_action_);
  }

  /*!
   * \return size of list
   */
  inline size_t Size() const
  {
    return parameters.Size();
  }

};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tStaticParameterList>;

#endif // core__parameter__tStaticParameterList_h__
