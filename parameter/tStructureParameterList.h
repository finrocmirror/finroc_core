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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TSTRUCTUREPARAMETERLIST_H
#define CORE__PARAMETER__TSTRUCTUREPARAMETERLIST_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/portdatabase/sSerializationHelper.h"
#include "core/plugin/tPlugins.h"
#include "core/tFinrocAnnotation.h"

namespace finroc
{
namespace core
{
class tDataType;
class tStructureParameterBase;
class tCoreInput;
class tFrameworkElement;
class tCoreOutput;
class tCreateModuleAction;

/*!
 * \author Max Reichardt
 *
 * List of structure parameters
 */
class tStructureParameterList : public tFinrocAnnotation
{
private:

  /*! List of parameters */
  util::tSimpleList<tStructureParameterBase*> parameters;

  /*!
   * Index of CreateModuleAction that was used to create framework element
   * (typically only set when created with finstruct)
   */
  int create_action;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  /*! Empty parameter list */
  static tStructureParameterList cEMPTY;

private:

  /*! Clear list */
  void Clear();

public:

  // slightly ugly... but safe
  tStructureParameterList(tStructureParameterBase* p1, tStructureParameterBase* p2 = NULL, tStructureParameterBase* p3 = NULL,
                          tStructureParameterBase* p4 = NULL, tStructureParameterBase* p5 = NULL, tStructureParameterBase* p6 = NULL,
                          tStructureParameterBase* p7 = NULL, tStructureParameterBase* p8 = NULL, tStructureParameterBase* p9 = NULL,
                          tStructureParameterBase* p10 = NULL, tStructureParameterBase* p11 = NULL, tStructureParameterBase* p12 = NULL,
                          tStructureParameterBase* p13 = NULL, tStructureParameterBase* p14 = NULL, tStructureParameterBase* p15 = NULL,
                          tStructureParameterBase* p16 = NULL, tStructureParameterBase* p17 = NULL, tStructureParameterBase* p18 = NULL,
                          tStructureParameterBase* p19 = NULL, tStructureParameterBase* p20 = NULL) :
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

  tStructureParameterList();

  /*!
   * Add parameter to list
   *
   * \param param Parameter
   */
  void Add(tStructureParameterBase* param);

  /*!
   * Clone parameter list - deep-copy without values
   *
   * \return Cloned list
   */
  tStructureParameterList* CloneList() const;

  virtual ~tStructureParameterList()
  {
    Clear();
    ;
  }

  virtual void Deserialize(tCoreInput& is);

  virtual void Deserialize(const util::tString& s)
  {
    sSerializationHelper::DeserializeFromHexString(this, s);
  }

  /*!
   * \param i Index
   * \return Parameter with specified index
   */
  inline tStructureParameterBase* Get(int i) const
  {
    return parameters.Get(i);
  }

  /*!
   * \return Index of CreateModuleAction that was used to create framework element
   */
  inline int GetCreateAction() const
  {
    return create_action;
  }

  /*!
   * Get or create StructureParameterList for Framework element
   *
   * \param fe Framework element
   * \return StructureParameterList
   */
  static tStructureParameterList* GetOrCreate(tFrameworkElement* fe);

  virtual void Serialize(tCoreOutput& os) const;

  virtual util::tString Serialize() const
  {
    return sSerializationHelper::SerializeToHexString(this);
  }

  /*!
   * \param create_action CreateModuleAction that was used to create framework element
   */
  inline void SetCreateAction(tCreateModuleAction* create_action_)
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

#endif // CORE__PARAMETER__TSTRUCTUREPARAMETERLIST_H
