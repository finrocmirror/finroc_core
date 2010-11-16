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
#include "core/plugin/tConstructorCreateModuleAction.h"

#ifndef CORE__PLUGIN__TFUNCTIONCREATEMODULEACTION_H
#define CORE__PLUGIN__TFUNCTIONCREATEMODULEACTION_H

namespace finroc
{
namespace core
{

template < typename P1 = tEmpty, typename P2 = tEmpty, typename P3 = tEmpty, typename P4 = tEmpty, typename P5 = tEmpty, typename P6 = tEmpty, typename P7 = tEmpty, typename P8 = tEmpty, typename P9 = tEmpty, typename P10 = tEmpty, typename P11 = tEmpty, typename P12 = tEmpty >
class tFunctionCreateModuleActionImpl : public tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>
{

public:

  typedef tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> base;

  typedef tFrameworkElement*(*tFunc12)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10, P11 t11, P12 t12);
  typedef tFrameworkElement*(*tFunc11)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10, P11 t11);
  typedef tFrameworkElement*(*tFunc10)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10);
  typedef tFrameworkElement*(*tFunc9)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9);
  typedef tFrameworkElement*(*tFunc8)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8);
  typedef tFrameworkElement*(*tFunc7)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7);
  typedef tFrameworkElement*(*tFunc6)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6);
  typedef tFrameworkElement*(*tFunc5)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5);
  typedef tFrameworkElement*(*tFunc4)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4);
  typedef tFrameworkElement*(*tFunc3)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3);
  typedef tFrameworkElement*(*tFunc2)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2);
  typedef tFrameworkElement*(*tFunc1)(const util::tString& name, tFrameworkElement* parent, P1 t1);
  typedef tFrameworkElement*(*tFunc0)(const util::tString& name, tFrameworkElement* parent);

  tFunc0 f0;
  tFunc1 f1;
  tFunc2 f2;
  tFunc3 f3;
  tFunc4 f4;
  tFunc5 f5;
  tFunc6 f6;
  tFunc7 f7;
  tFunc8 f8;
  tFunc9 f9;
  tFunc10 f10;
  tFunc11 f11;
  tFunc12 f12;

  tFunctionCreateModuleActionImpl(const util::tString& group, const util::tString& type, const util::tString& param_names) :
      tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>(group, type, param_names),
      f0(NULL), f1(NULL), f2(NULL), f3(NULL), f4(NULL), f5(NULL), f6(NULL), f7(NULL), f8(NULL), f9(NULL), f10(NULL), f11(NULL), f12(NULL)
  {
  }

  virtual tFrameworkElement* CreateModule(const util::tString& name, tFrameworkElement* parent, tConstructorParameters* p) const
  {
    if (f0 != NULL)
    {
      return (*f0)(name, parent);
    }
    else if (f1 != NULL)
    {
      return (*f1)(name, parent, base::GetP1(p));
    }
    else if (f2 != NULL)
    {
      return (*f2)(name, parent, base::GetP1(p), base::GetP2(p));
    }
    else if (f3 != NULL)
    {
      return (*f3)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p));
    }
    else if (f4 != NULL)
    {
      return (*f4)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p));
    }
    else if (f5 != NULL)
    {
      return (*f5)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p));
    }
    else if (f6 != NULL)
    {
      return (*f6)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p));
    }
    else if (f7 != NULL)
    {
      return (*f7)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p));
    }
    else if (f8 != NULL)
    {
      return (*f8)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p));
    }
    else if (f9 != NULL)
    {
      return (*f9)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p));
    }
    else if (f10 != NULL)
    {
      return (*f10)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p));
    }
    else if (f11 != NULL)
    {
      return (*f11)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p), base::GetP11(p));
    }
    else
    {
      assert(f12 != NULL);
      return (*f12)(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p), base::GetP11(p), base::GetP12(p));
    }
  }
};

/*!
 * CreateModuleAction to wrap functions that create module
 */
struct tFunctionCreateModuleAction
{

  std::auto_ptr<tCreateModuleAction> wrapped;

public:

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10, P11 t11, P12 t12))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> W;
    W* w = new W(group, type, param_names);
    w->f12 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10, P11 t11))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11> W;
    W* w = new W(group, type, param_names);
    w->f11 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9, P10 t10))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10> W;
    W* w = new W(group, type, param_names);
    w->f10 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8, P9 t9))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7, P8, P9> W;
    W* w = new W(group, type, param_names);
    w->f9 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7, P8 t8))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7, P8> W;
    W* w = new W(group, type, param_names);
    w->f8 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6, P7 t7))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6, P7> W;
    W* w = new W(group, type, param_names);
    w->f7 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5, P6 t6))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5, P6> W;
    W* w = new W(group, type, param_names);
    w->f6 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4, typename P5>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4, P5 t5))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4, P5> W;
    W* w = new W(group, type, param_names);
    w->f5 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3, typename P4>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3, P4 t4))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3, P4> W;
    W* w = new W(group, type, param_names);
    w->f4 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2, typename P3>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2, P3 t3))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2, P3> W;
    W* w = new W(group, type, param_names);
    w->f3 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1, typename P2>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1, P2 t2))
  {
    typedef tFunctionCreateModuleActionImpl<P1, P2> W;
    W* w = new W(group, type, param_names);
    w->f2 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  template <typename P1>
  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent, P1 t1))
  {
    typedef tFunctionCreateModuleActionImpl<P1> W;
    W* w = new W(group, type, param_names);
    w->f1 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }

  tFunctionCreateModuleAction(const util::tString& group, const util::tString& type, tFrameworkElement*(*func)(const util::tString& name, tFrameworkElement* parent))
  {
    typedef tFunctionCreateModuleActionImpl<tEmpty> W;
    W* w = new W(group, type, "");
    w->f0 = func;
    wrapped.reset(w);
    group = GetBinary((void*)func);
  }
};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TFUNCTIONCREATEMODULEACTION_H
