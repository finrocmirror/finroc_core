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
#include "core/port/rpc/tCallParameter.h"
#include "core/port/std/tPortDataManager.h"
#include "core/portdatabase/tFinrocTypeInfo.h"

namespace finroc
{
namespace core
{
const int8 tCallParameter::cNULLPARAM, tCallParameter::cNUMBER, tCallParameter::cOBJECT;

tCallParameter::tCallParameter() :
  number(),
  value(),
  type(0)
{
}

void tCallParameter::Deserialize(rrlib::serialization::tInputStream& is)
{
  type = is.ReadByte();
  if (type == cNUMBER)
  {
    number.Deserialize(is);
  }
  else if (type == cOBJECT)
  {
    assert((value == NULL));
    //value = (GenericObject)is.readObjectInInterThreadContainer(null);
    rrlib::rtti::tGenericObject* go = rrlib::rtti::ReadObject(is, NULL, this);
    if (!go)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Received object that could not be read. Throwing exception.");
      throw util::tException("Object could not be read.");
    }
    value = Lock(go);
    tPortDataManager* pdm = value.GetManagerT<tPortDataManager>();
    if (pdm != NULL)
    {
      pdm->lock_iD = is.ReadInt();
    }
  }
}

tPortDataPtr<rrlib::rtti::tGenericObject> tCallParameter::Lock(rrlib::rtti::tGenericObject* tmp)
{
  bool cc_type = tFinrocTypeInfo::IsCCType(tmp->GetType());

  if (cc_type)
  {
    tCCPortDataManager* mgr = (tCCPortDataManager*)tmp->GetManager();
    return tPortDataPtr<rrlib::rtti::tGenericObject>(tmp, mgr);
  }
  else
  {
    tPortDataManager* mgr = (tPortDataManager*)tmp->GetManager();
    mgr->GetCurrentRefCounter()->SetOrAddLocks(1);
    return tPortDataPtr<rrlib::rtti::tGenericObject>(tmp, mgr);
  }

}

void tCallParameter::Recycle()
{
  value.reset();
  type = cNULLPARAM;
}

void tCallParameter::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  oos.WriteByte(type);
  if (type == cNUMBER)
  {
    number.Serialize(oos);
  }
  else if (type == cOBJECT)
  {
    rrlib::rtti::WriteObject(oos, value.get());
    tPortDataManager* pdm = value.GetManagerT<tPortDataManager>();
    if (pdm != NULL)
    {
      oos.WriteInt(pdm->lock_iD);
    }
  }
}

} // namespace finroc
} // namespace core

