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
#include "core/structure/tStructureElementRegister.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

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

util::tMutex& tStructureElementRegister::GetMutex()
{
  static util::tMutex mutex;
  return mutex;
}

std::vector<tInstantiatedModule>& tStructureElementRegister::GetRegister()
{
  static std::vector<tInstantiatedModule> reg;
  return reg;
}

std::vector<tModulePorts>& tStructureElementRegister::GetModuleTypeRegister()
{
  static std::vector<tModulePorts> reg;
  return reg;
}

void tStructureElementRegister::AddMemoryBlock(void* address, size_t size)
{
  util::tLock(GetMutex());
  FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1) << "Adding memory block at " << address << " with size " << size;
  tInstantiatedModule m = {(char*)address, size, NULL };
  GetRegister().push_back(m);
}

void tStructureElementRegister::AddModule(tFrameworkElement* module)
{
  util::tLock(GetMutex());
  FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1) << "Adding module " << module << "...";
  std::vector<tInstantiatedModule>& reg = tStructureElementRegister::GetRegister();
  void* ptr = module;
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module usually is at the end (the most recently added)
  {
    if (ptr >= reg[i].address && ptr < reg[i].address + reg[i].size)
    {
      assert(reg[i].module == NULL);
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1) << "Module resides in memory block " << reg[i].address;
      reg[i].module = module;
    }
  }
}

void tStructureElementRegister::RemoveModule(tFrameworkElement* module)
{
  util::tLock(GetMutex());
  FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1) << "Removing module " << module;
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
  util::tString s(rrlib::serialization::sSerialization::Demangle(typeid(*parent).name()));
  if (s.Contains("<"))
  {
    s = s.Substring(0, s.IndexOf("<"));
  }

  for (size_t i = 0; i < reg.size(); i++)
  {
    if (s.Equals(reg[i].name))
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

  FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING) << "Cannot resolve port name for module type " << s.GetStdString() << " index " << port_index;
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

