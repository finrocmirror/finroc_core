//
// You received this file as part of Finroc
// A framework for integrated robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    tStructureElementRegister.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2011-04-13
 *
 */
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "rrlib/util/patterns/singleton.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/structure/tStructureElementRegister.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// tStructureElementRegister constructors
//----------------------------------------------------------------------

namespace finroc
{
namespace core
{
namespace structure
{
namespace internal
{
struct tStructureElementStorage
{
  util::tMutex mutex;
  std::vector<tInstantiatedModule> reg;
  std::vector<tModulePorts> module_type_reg;
};
static inline unsigned int GetLongevity(internal::tStructureElementStorage*)
{
  return 20; // can be deleted after runtime environment
}
}

typedef rrlib::util::tSingletonHolder<internal::tStructureElementStorage, rrlib::util::singleton::Longevity> tStructureElementStorageInstance;

util::tMutex& tStructureElementRegister::GetMutex()
{
  return tStructureElementStorageInstance::Instance().mutex;
}

std::vector<tInstantiatedModule>& tStructureElementRegister::GetRegister()
{
  return tStructureElementStorageInstance::Instance().reg;
}

std::vector<tModulePorts>& tStructureElementRegister::GetModuleTypeRegister()
{
  return tStructureElementStorageInstance::Instance().module_type_reg;
}

void tStructureElementRegister::AddMemoryBlock(void* address, size_t size)
{
  util::tLock(GetMutex());
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Adding memory block at ", address, " with size ", size);
  tInstantiatedModule m = {(char*)address, size, NULL };
  GetRegister().push_back(m);
}

void tStructureElementRegister::AddModule(tFrameworkElement* module)
{
  util::tLock(GetMutex());
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Adding module ", module, "...");
  std::vector<tInstantiatedModule>& reg = tStructureElementRegister::GetRegister();
  void* ptr = module;
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module usually is at the end (the most recently added)
  {
    if (ptr >= reg[i].address && ptr < reg[i].address + reg[i].size)
    {
      assert(reg[i].module == NULL);
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Module resides in memory block ", reg[i].address);
      reg[i].module = module;
    }
  }
}

void tStructureElementRegister::RemoveModule(tFrameworkElement* module)
{
  util::tLock(GetMutex());
  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Removing module ", module);
  std::vector<tInstantiatedModule>& reg = tStructureElementRegister::GetRegister();
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module is possibly rather at the end
  {
    if (module == reg[i].module)
    {
      reg.erase(reg.begin() + i);
      return;
    }
  }
}

tFrameworkElement* tStructureElementRegister::FindParent(void* ptr)
{
  util::tLock(GetMutex());
  std::vector<tInstantiatedModule>& reg = tStructureElementRegister::GetRegister();
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module usually is at the end (the most recently added)
  {
    if (ptr >= reg[i].address && ptr < reg[i].address + reg[i].size)
    {
      assert(reg[i].module != NULL);
      return reg[i].module;
    }
  }
  assert(false && "Could not find parent for Port/Parameter. Please use constructor that takes parent as parameter for dynamically created ports.");
  return NULL;
}

finroc::util::tString& tStructureElementRegister::GetAutoGeneratedPortName(tFrameworkElement* parent, size_t port_index)
{
  static util::tString unresolved("(unresolved port name)");

  util::tLock(GetMutex());
  std::vector<tModulePorts>& reg = tStructureElementRegister::GetModuleTypeRegister();

  // normalize type String (remove any template arguments)
  util::tString s(rrlib::rtti::Demangle(typeid(*parent).name()));
  if (s.find('<') != std::string::npos)
  {
    s = s.substr(0, s.find('<'));
  }

  for (size_t i = 0; i < reg.size(); i++)
  {
    if (boost::equals(s, reg[i].name))
    {
      if (port_index < reg[i].ports.size())
      {
        return reg[i].ports[port_index];
      }
      else
      {
        break;
      }
    }
  }

  FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_WARNING, "Cannot resolve port name for module type ", s, " index ", port_index);
  return unresolved;
}

/*!
 * Add port names for a module type
 * (typically called by auto-generated code)
 */
void tStructureElementRegister::AddPortNamesForModuleType(const util::tString& name, const std::vector<util::tString>& names)
{
  util::tLock(GetMutex());
  std::vector<tModulePorts>& reg = tStructureElementRegister::GetModuleTypeRegister();

  tModulePorts mp;
  mp.name = name;
  mp.ports = names;
  reg.push_back(mp);
}

}
}
}

