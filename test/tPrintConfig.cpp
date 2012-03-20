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
#include "core/tRuntimeEnvironment.h"
#include "core/port/std/tPortDataManager.h"
#include "core/datatype/tNumber.h"
#include "core/datatype/tCoreString.h"
#include "core/port/std/tPortBase.h"
#include "core/port/cc/tCCPortBase.h"
#include <bitset>

int main(int argc__, char **argv__)
{
  printf("Finroc Configuration:\n");

#ifdef __JC_BASIC_REUSABLE_TRACING_ENABLED__
  printf(" __JC_BASIC_REUSABLE_TRACING_ENABLED__: true\n");
#else
  printf(" __JC_BASIC_REUSABLE_TRACING_ENABLED__: false\n");
#endif

#ifdef __JC_DETAILED_REUSABLE_TRACING_ENABLED__
  printf(" __JC_DETAILED_REUSABLE_TRACING_ENABLED__: true\n");
#else
  printf(" __JC_DETAILED_REUSABLE_TRACING_ENABLED__: false\n");
#endif

#ifdef __JC_ENFORCE_LOCK_ORDER__
  printf(" __JC_ENFORCE_LOCK_ORDER__: true\n");
#else
  printf(" __JC_ENFORCE_LOCK_ORDER__: false\n");
#endif

#ifdef __JC_CREATE_JNI_WRAPPERS__
  printf(" __JC_CREATE_JNI_WRAPPERS__: true\n");
#else
  printf(" __JC_CREATE_JNI_WRAPPERS__: false\n");
#endif

  printf(" sizeof(tPortDataManager): %zd\n", sizeof(finroc::core::tPortDataManager));
  printf(" sizeof(tNumber): %zd\n", sizeof(finroc::core::tNumber));
  printf(" sizeof(tCoreString): %zd\n", sizeof(finroc::core::tCoreString));
  printf(" sizeof(tSafeConcurrentlyIterableList<void*, finroc::util::tNoMutex>): %zd\n", sizeof(finroc::util::tSafeConcurrentlyIterableList<void*, finroc::util::tNoMutex>));
  printf(" sizeof(tSafeConcurrentlyIterableList<void*, finroc::util::tMutexLockOrder>): %zd\n", sizeof(finroc::util::tSafeConcurrentlyIterableList<void*, finroc::util::tMutexLockOrder>));
  printf(" sizeof(tArrayWrapper<void*>): %zd\n", sizeof(finroc::util::tArrayWrapper<void*>));
  printf(" sizeof(bitset<1>): %zd\n", sizeof(std::bitset<1>));
  printf(" sizeof(tMutex): %zd\n", sizeof(finroc::util::tMutex));
  printf(" sizeof(tMutexLockOrder): %zd\n", sizeof(finroc::util::tMutexLockOrder));
  printf(" sizeof(tFrameworkElement): %zd\n", sizeof(finroc::core::tFrameworkElement));
  printf(" sizeof(tAbstractPort): %zd\n", sizeof(finroc::core::tAbstractPort));
  printf(" sizeof(tPortBase): %zd\n", sizeof(finroc::core::tPortBase));
  printf(" sizeof(tCCPortBase): %zd\n", sizeof(finroc::core::tCCPortBase));
  return 0;
}
