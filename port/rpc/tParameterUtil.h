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
#ifndef CORE__PORT__RPC__TPARAMETERUTIL_H
#define CORE__PORT__RPC__TPARAMETERUTIL_H

#include "core/port/rpc/tCallParameter.h"
#include "core/port/std/tPortData.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include <boost/type_traits/is_base_of.hpp>

namespace finroc
{
namespace core
{

namespace tParamaterUtilHelper
{
tCCInterThreadContainer<>* GetInterThreadBuffer(tDataType* dt);
}

/*!
 * This class contains classes with static helper functions
 * that allow handling parameters of 'AbstractCall's in a
 * uniform way.
 */
template <typename T>
class tParameterUtil
{
  // empty dummy
};

template <typename T>
class tElementaryParamUtil
{
public:
  static void Cleanup(T t)
  {
    // do nothing
  }

  static bool HasLock(T t)
  {
    return true;
  }
};

template<>
class tParameterUtil<int> : public tElementaryParamUtil<int>
{
public:
  static void GetParam(tCallParameter* p, int& val)
  {
    val = p->ival;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, int val)
  {
    p->type = tCallParameter::cINT;
    p->ival = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, int val)
  {
    os.WriteByte(tCallParameter::cINT);
    os.WriteInt(val);
  }
};

template<>
class tParameterUtil<int64> : public tElementaryParamUtil<int64>
{
public:
  static void GetParam(tCallParameter* p, int64& val)
  {
    val = p->lval;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, int64 val)
  {
    p->type = tCallParameter::cLONG;
    p->lval = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, int64 val)
  {
    os.WriteByte(tCallParameter::cLONG);
    os.WriteLong(val);
  }
};

template<>
class tParameterUtil<int16> : public tElementaryParamUtil<int16>
{
public:
  static void GetParam(tCallParameter* p, int16& val)
  {
    val = p->sval;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, int16 val)
  {
    p->type = tCallParameter::cSHORT;
    p->sval = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, int16 val)
  {
    os.WriteByte(tCallParameter::cSHORT);
    os.WriteShort(val);
  }
};

template<>
class tParameterUtil<int8> : public tElementaryParamUtil<int8>
{
public:
  static void GetParam(tCallParameter* p, int8& val)
  {
    val = p->bval;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, int8 val)
  {
    p->type = tCallParameter::cBYTE;
    p->bval = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, int8 val)
  {
    os.WriteByte(tCallParameter::cBYTE);
    os.WriteByte(val);
  }
};

template<>
class tParameterUtil<double> : public tElementaryParamUtil<double>
{
public:
  static void GetParam(tCallParameter* p, double& val)
  {
    val = p->dval;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, double val)
  {
    p->type = tCallParameter::cDOUBLE;
    p->dval = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, double val)
  {
    os.WriteByte(tCallParameter::cDOUBLE);
    os.WriteDouble(val);
  }
};

template<>
class tParameterUtil<float> : public tElementaryParamUtil<float>
{
public:
  static void GetParam(tCallParameter* p, float& val)
  {
    val = p->fval;
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, float val)
  {
    p->type = tCallParameter::cFLOAT;
    p->fval = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, float val)
  {
    os.WriteByte(tCallParameter::cFLOAT);
    os.WriteFloat(val);
  }
};

template <bool PORTDATA, bool CCCONTAINER>
class tParameterPtrUtil
{
public:
  // empty dummy
};

// PortData variant
template<>
class tParameterPtrUtil<true, false>
{
public:
  // cleanup function
  static void Cleanup(const tPortData* t)
  {
    if (t == NULL)
    {
      return;
    }
    t->GetManager()->ReleaseLock();
  }

  static bool HasLock(const tPortData* pd)
  {
    if (pd == NULL)
    {
      return true;
    }
    return pd->GetManager()->IsLocked();
  }

  template <typename T>
  static void GetParam(tCallParameter* p, const T*& val)
  {
    val = static_cast<const T*>(p->value);
    p->Clear();
  }

  template <typename T>
  static void GetParam(tCallParameter* p, T*& val)
  {
    val = static_cast<T*>(const_cast<tPortData*>(p->value)); // not entirely clean... but everything else seems a lot of work (what's const and what isn't should be defined in methods)
    p->Clear();
  }

  static void AddParamForLocalCall(tCallParameter* p, const tPortData* val)
  {
    if (val == NULL)
    {
      p->type = tCallParameter::cNULLPARAM;
    }
    p->type = tCallParameter::cPORTDATA;
    p->value = val;
  }

  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, const tPortData* val)
  {
    if (val == NULL)
    {
      os.WriteByte(tCallParameter::cNULLPARAM);
    }
    os.WriteByte(tCallParameter::cPORTDATA);
    responsibilities.Add(val);
  }
};

// CCInterThreadContainer variant
template<>
class tParameterPtrUtil<false, true>
{
public:
  // cleanup function
  template <typename T>
  static void Cleanup(T* t)
  {
    t->Recycle2();
  }

  static bool HasLock(void* t)
  {
    return true;
  }

  template <typename T>
  static void GetParam(tCallParameter* p, T*& val)
  {
    val = p->ccval;
    p->Clear();
  }

  template <typename T>
  static void AddParamForLocalCall(tCallParameter* p, T* val)
  {
    if (val == NULL)
    {
      p->type = tCallParameter::cNULLPARAM;
    }
    p->type = tCallParameter::cCCCONTAINER;
    p->ccval = val;
  }

  template <typename T>
  static void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, T* val)
  {
    if (val == NULL)
    {
      os.WriteByte(tCallParameter::cNULLPARAM);
    }
    //assert(container->isInterThreadContainer() && "only interthread container allowed in method calls");
    os.WriteByte(tCallParameter::cCCCONTAINER);
    os.WriteObject(val);
    val->Recycle2();
  }
};


// CC-Data variant
template<>
class tParameterPtrUtil<false, false>
{
public:
  // cleanup function
  static void Cleanup(void* t)
  {
    // do nothing
  }

  static bool HasLock(void* t)
  {
    return true;
  }

  template <typename T>
  static void GetParam(tCallParameter* p, T*& c)
  {
    c = static_cast<T*>(p->ccval->GetDataPtr());
  }

  template <typename T>
  static void AddParamForLocalCall(tCallParameter* p, T* val)
  {
    if (val == NULL)
    {
      p->type = tCallParameter::cNULLPARAM;
    }
    tDataType* dt = tDataTypeRegister::GetInstance()->GetDataType<T>();
    assert(dt != NULL && dt->IsCCType());
    tCCInterThreadContainer<T>* cc = (tCCInterThreadContainer<T>*)tParamaterUtilHelper::GetInterThreadBuffer(dt);
    cc->Assign(val);
    p->type = tCallParameter::cCCDATA;
    p->ccval = cc;
  }

  template <typename T>
  void AddParamForSending(util::tSimpleList<const tPortData*>& responsibilities, tCoreOutput& os, const T* c)
  {
    if (c == NULL)
    {
      os.WriteByte(tCallParameter::cNULLPARAM);
    }
    tDataType* dt = tDataTypeRegister::GetInstance()->GetDataType<T>();
    assert(dt != NULL && dt->IsCCType());
    os.WriteByte(tCallParameter::cCCDATA);
    os.WriteShort(dt->GetUid());
    WriteParam(c, dt);
  }
};


// variant for PortData, CCInterThreadContainer<T>, and CCPortData
template <typename T>
class tParameterUtil<T*> : public tParameterPtrUtil<boost::is_base_of<tPortData, T>::value, boost::is_base_of<tCCContainerBase, T>::value >
{
};

} // namespace finroc
} // namespace core


#endif // CORE__PORT__RPC__TPARAMETERUTIL_H
