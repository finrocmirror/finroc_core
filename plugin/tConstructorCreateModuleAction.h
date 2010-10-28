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

#ifndef CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTION_H
#define CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTION_H

#include "core/parameter/tConstructorParameters.h"
#include "core/parameter/tStructureParameterList.h"
#include "core/plugin/tCreateModuleAction.h"

#include "core/plugin/tParamType.h"

namespace finroc
{
namespace core
{
class tStructureParameterBase;

/*!
 * \author Max Reichardt
 *
 * Abstract base class for ConstructorCreateModuleAction
 */
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
class tConstructorCreateModuleActionBase : public util::tUncopyableObject, public tCreateModuleAction
{
protected:

  /*!
   * StructureParameterList
   */
  mutable tStructureParameterList spl;

public:

  typedef tParamType<P1> SP1;
  typedef typename SP1::t SPT1;
  typedef tParamType<P2> SP2;
  typedef typename SP2::t SPT2;
  typedef tParamType<P3> SP3;
  typedef typename SP3::t SPT3;
  typedef tParamType<P4> SP4;
  typedef typename SP4::t SPT4;
  typedef tParamType<P5> SP5;
  typedef typename SP5::t SPT5;
  typedef tParamType<P6> SP6;
  typedef typename SP6::t SPT6;
  typedef tParamType<P7> SP7;
  typedef typename SP7::t SPT7;
  typedef tParamType<P8> SP8;
  typedef typename SP8::t SPT8;
  typedef tParamType<P9> SP9;
  typedef typename SP9::t SPT9;
  typedef tParamType<P10> SP10;
  typedef typename SP10::t SPT10;
  typedef tParamType<P11> SP11;
  typedef typename SP11::t SPT11;
  typedef tParamType<P12> SP12;
  typedef typename SP12::t SPT12;

  /*!
   * Parameters
   */
  SPT1* p1;

  SPT2* p2;

  SPT3* p3;

  SPT4* p4;

  SPT5* p5;

  SPT6* p6;

  SPT7* p7;

  SPT8* p8;

  SPT9* p9;

  SPT10* p10;

  SPT11* p11;

  SPT12* p12;

  /*! Name and group of module */
  util::tString name, group;

private:

  void Add(tEmpty* param) const {}

  /*!
   * Adds parameter to parameter list
   *
   * \param param
   */
  inline void Add(tStructureParameterBase* param) const
  {
    if (param != NULL)
    {
      spl.Add(param);
    }
  }

  /*!
   * builds parameter list, if it's not built already
   */
  void CheckStructureParameterList() const;

public:

  tConstructorCreateModuleActionBase(const util::tString& group_, const util::tString& type_name, const util::tString& param_names);

  virtual util::tString GetModuleGroup() const
  {
    return group;
  }

  virtual util::tString GetName() const
  {
    return name;
  }

  P1 GetP1(tConstructorParameters* p) const
  {
    return SP1::Get(p->Get(0));
  }
  P2 GetP2(tConstructorParameters* p) const
  {
    return SP2::Get(p->Get(1));
  }
  P3 GetP3(tConstructorParameters* p) const
  {
    return SP3::Get(p->Get(2));
  }
  P4 GetP4(tConstructorParameters* p) const
  {
    return SP4::Get(p->Get(3));
  }
  P5 GetP5(tConstructorParameters* p) const
  {
    return SP5::Get(p->Get(4));
  }
  P6 GetP6(tConstructorParameters* p) const
  {
    return SP6::Get(p->Get(5));
  }
  P7 GetP7(tConstructorParameters* p) const
  {
    return SP7::Get(p->Get(6));
  }
  P8 GetP8(tConstructorParameters* p) const
  {
    return SP8::Get(p->Get(7));
  }
  P9 GetP9(tConstructorParameters* p) const
  {
    return SP9::Get(p->Get(8));
  }
  P10 GetP10(tConstructorParameters* p) const
  {
    return SP10::Get(p->Get(9));
  }
  P11 GetP11(tConstructorParameters* p) const
  {
    return SP11::Get(p->Get(10));
  }
  P12 GetP12(tConstructorParameters* p) const
  {
    return SP12::Get(p->Get(11));
  }

  virtual const tStructureParameterList* GetParameterTypes() const
  {
    CheckStructureParameterList();
    return &(spl);
  }

};

} // namespace finroc
} // namespace core

#include "core/plugin/tConstructorCreateModuleAction.hpp"

#include "core/plugin/tConstructorCreateModuleActionImpl.h"

#endif // CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTION_H
