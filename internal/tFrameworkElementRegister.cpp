//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
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
/*!\file    core/internal/tFrameworkElementRegister.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2013-01-25
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tFrameworkElementRegister.h"

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
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

/*! Minimum duration to wait before reusing array slot */
static const rrlib::time::tDuration cMIN_SLOT_REUSE_DURATION = definitions::cHANDLE_UNIQUENESS_GUARANTEE_DURATION / tFrameworkElementRegister::cSTAMP_VALUES;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tFrameworkElementRegister::tFrameworkElementRegister() :
  array_chunks(),
  next_index(1),
  next_port_index(cFIRST_PORT_INDEX),
  unused_slot_queue()
{
  array_chunks.fill(NULL);
}

tFrameworkElementRegister::~tFrameworkElementRegister()
{
  for (auto it = array_chunks.begin(); it != array_chunks.end(); ++it)
  {
    delete *it;
  }
}

tFrameworkElementRegister::tHandle tFrameworkElementRegister::Add(tFrameworkElement& framework_element, bool is_port)
{
  std::queue<tUnusedSlot>& queue = is_port ? unused_port_slot_queue : unused_slot_queue;
  if (!queue.empty())
  {
    if (queue.front().delete_time + cMIN_SLOT_REUSE_DURATION < rrlib::time::Now())
    {
      tHandle ex_handle = queue.front().ex_handle;
      queue.pop();

      uint32_t new_stamp = (ex_handle + 1) & cSTAMP_MASK;
      uint32_t index = ex_handle >> cSTAMP_BIT_WIDTH;
      uint32_t primary_index = index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;
      assert(array_chunks[primary_index]->array[index & cSECONDARY_INDEX_MASK] == NULL);
      array_chunks[primary_index]->array[index & cSECONDARY_INDEX_MASK] = &framework_element;

      return (index << cSTAMP_BIT_WIDTH) | new_stamp;
    }
  }

  // put in new slot
  uint32_t use_index = 0;
  if (is_port)
  {
    use_index = next_port_index;
    next_port_index++;
    if (use_index > cMAX_PORT_INDEX)
    {
      FINROC_LOG_PRINT(ERROR, "Maximum number of ports exceeded (", cFIRST_PORT_INDEX, "). You can adjust definitions.h if you need that many ports.");
      throw std::runtime_error("Maximum number of ports exceeded. You can adjust definitions.h if you need that many ports.");
    }
  }
  else
  {
    use_index = next_index;
    next_index++;
    if (use_index >= cFIRST_PORT_INDEX)
    {
      FINROC_LOG_PRINT(ERROR, "Maximum number of non-port framework elements exceeded (", cFIRST_PORT_INDEX, "). You can adjust definitions.h if you need that many.");
      throw std::runtime_error("Maximum number of non-port framework elements exceeded. You can adjust definitions.h if you need that many.");
    }
  }

  uint32_t primary_index = use_index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;
  if (!array_chunks[primary_index])
  {
    array_chunks[primary_index] = new tSecondaryArray();
  }
  assert(array_chunks[primary_index]->array[use_index & cSECONDARY_INDEX_MASK] == NULL);
  array_chunks[primary_index]->array[use_index & cSECONDARY_INDEX_MASK] = &framework_element;
  tHandle handle = use_index << cSTAMP_BIT_WIDTH;
  return handle;
}

size_t tFrameworkElementRegister::GetAllElements(tFrameworkElement** result_buffer, size_t max_elements, tHandle start_from_handle)
{
  if (max_elements == 0)
  {
    return 0;
  }

  size_t result = 0;
  uint32_t start_index = start_from_handle >> cSTAMP_BIT_WIDTH;
  uint32_t last_index = (start_from_handle >= cFIRST_PORT_HANDLE) ? (next_port_index - 1) : (next_index - 1);
  uint32_t primary_start_index = start_index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;
  uint32_t primary_end_index = last_index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;

  for (size_t primary_index = primary_start_index; primary_index <= primary_end_index; primary_index++)
  {
    if (array_chunks[primary_index])
    {
      uint32_t secondary_start_index = 0;
      if (primary_index == primary_start_index)
      {
        secondary_start_index = start_index & cSECONDARY_INDEX_MASK;
        tFrameworkElement* framework_element = array_chunks[primary_index]->array[secondary_start_index];
        if (framework_element && framework_element->GetHandle() < start_from_handle)
        {
          secondary_start_index++;
        }
      }
      uint32_t secondary_end_index = (primary_index == primary_end_index) ? (last_index & cSECONDARY_INDEX_MASK) : cSECONDARY_INDEX_MASK;
      for (size_t secondary_index = secondary_start_index; secondary_index <= secondary_end_index; secondary_index++)
      {
        tFrameworkElement* framework_element = array_chunks[primary_index]->array[secondary_index];
        if (framework_element)
        {
          *result_buffer = framework_element;
          result_buffer++;
          result++;
          if (result == max_elements)
          {
            return result;
          }
        }
      }
    }
  }

  // If we have place left in result buffer, also return ports
  if (start_from_handle < cFIRST_PORT_HANDLE)
  {
    return GetAllElements(result_buffer, max_elements - result, cFIRST_PORT_HANDLE) + result;
  }

  return result;
}

void tFrameworkElementRegister::Remove(tHandle handle)
{
  uint32_t index = handle >> cSTAMP_BIT_WIDTH;
  uint32_t primary_index = index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;
  uint32_t secondary_index = index & cSECONDARY_INDEX_MASK;
  assert(array_chunks[primary_index] && array_chunks[primary_index]->array[secondary_index] && array_chunks[primary_index]->array[secondary_index]->GetHandle() == handle);
  array_chunks[primary_index]->array[secondary_index] = NULL;
  unused_slot_queue.push(tUnusedSlot(handle));
}


tFrameworkElementRegister::tSecondaryArray::tSecondaryArray() :
  array()
{
  array.fill(NULL);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
