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
#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
tPortDataManager tPortDataManager::cPROTOTYPE;
size_t tPortDataManager::cREF_COUNTERS_OFFSET = ((char*)&(tPortDataManager::cPROTOTYPE.ref_counters[0])) - ((char*)&(tPortDataManager::cPROTOTYPE));

const size_t tPortDataManager::cNUMBER_OF_REFERENCES;
const size_t tPortDataManager::cREF_INDEX_MASK;

tPortDataManager::tPortDataManager() :
    ref_counters(),
    unused(true),
    derived_from(NULL),
    reuse_counter(0),
    lock_iD(0)
{
  //log(LogLevel.LL_DEBUG_VERBOSE_1, logDomain, "Creating PortDataManager"); //<" + dt.getName() + "> - data: " + data);
}

void tPortDataManager::DangerousDirectRecycle()
{
  if (derived_from != NULL)
  {
    derived_from->GetCurrentRefCounter()->ReleaseLock();
    derived_from = NULL;

    //TODO:
    //type = null;
    //data = null;
  }
  else
  {
    GetObject()->Clear();
  }
  reuse_counter++;
  ::finroc::util::tReusable::Recycle();
}

const util::tString tPortDataManager::ToString() const
{
  return util::tStringBuilder("PortDataManager for ") + GetContentString() + " (Locks: " + GetCurrentRefCounter()->GetLocks() + ")";
}

} // namespace finroc
} // namespace core

