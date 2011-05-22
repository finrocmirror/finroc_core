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
#include "rrlib/serialization/tInputStream.h"
#include "core/port/std/tPortDataManager.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "rrlib/serialization/tOutputStream.h"

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
    rrlib::serialization::tGenericObject* go = is.ReadObject(NULL, this);
    value = Lock(go);
    tPortDataManager* pdm = value.GetManagerT<tPortDataManager>();
    if (pdm != NULL)
    {
      pdm->lock_iD = is.ReadInt();
    }
  }
}

tPortDataPtr<rrlib::serialization::tGenericObject> tCallParameter::Lock(rrlib::serialization::tGenericObject* tmp)
{
  bool cc_type = tFinrocTypeInfo::IsCCType(tmp->GetType());

  if (cc_type)
  {
    tCCPortDataManager* mgr = (tCCPortDataManager*)tmp->GetManager();
    return tPortDataPtr<rrlib::serialization::tGenericObject>(tmp, mgr);
  }
  else
  {
    tPortDataManager* mgr = (tPortDataManager*)tmp->GetManager();
    mgr->GetCurrentRefCounter()->SetOrAddLocks(1);
    return tPortDataPtr<rrlib::serialization::tGenericObject>(tmp, mgr);
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
    oos.WriteObject(value.get());
    tPortDataManager* pdm = value.GetManagerT<tPortDataManager>();
    if (pdm != NULL)
    {
      oos.WriteInt(pdm->lock_iD);
    }
  }
}

} // namespace finroc
} // namespace core

