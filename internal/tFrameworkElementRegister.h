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
/*!\file    core/internal/tFrameworkElementRegister.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tFrameworkElementRegister
 *
 * \b tFrameworkElementRegister
 *
 * Efficient lookup table that maps framework element handles to framework
 * elements - using nested arrays.
 * Furthermore assigns and manages handles.
 *
 * For its implementation, this class uses nested arrays:
 * The primary array has 2^cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH entries
 * and is created immediately.
 * Its entries point to secondary arrays, which have
 * 2^cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH entries - which contain the actual values.
 * Secondary are created when they are needed. This way, the register can "grow".
 *
 * For documentation on framework element handle format - see definitions.h
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tFrameworkElementRegister_h__
#define __core__internal__tFrameworkElementRegister_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <queue>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

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
// Class declaration
//----------------------------------------------------------------------
//! Framework element register
/*!
 * Efficient lookup table that maps framework element handles to framework
 * elements - using nested arrays.
 * Furthermore assigns and manages handles.
 *
 * For its implementation, this class uses nested arrays:
 * The primary array has 2^cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH entries
 * and is created immediately.
 * Its entries point to secondary arrays, which have
 * 2^cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH entries - which contain the actual values.
 * Secondary are created when they are needed. This way, the register can "grow".
 *
 * For documentation on framework element handle format - see definitions.h
 */
class tFrameworkElementRegister : public boost::noncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef uint32_t tHandle;

  /*! Number of elements in primary array */
  enum { cPRIMARY_ARRAY_SIZE = 1 << definitions::cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH };

  /*! Number of elements in secondary array */
  enum { cSECONDARY_ARRAY_SIZE = 1 << definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH };

  /*! Mask to extract secondary index */
  enum { cSECONDARY_INDEX_MASK = cSECONDARY_ARRAY_SIZE - 1 };

  /*! Bit width of stamp */
  enum { cSTAMP_BIT_WIDTH = (sizeof(tHandle) * 8) - (definitions::cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH + definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH) };

  /*! Number of possible stamp values */
  enum { cSTAMP_VALUES = 1 << cSTAMP_BIT_WIDTH };

  /*! Mask to extract stamp from handle */
  enum { cSTAMP_MASK = cSTAMP_VALUES - 1 };

  /*! First port index */
  enum { cFIRST_PORT_INDEX = 1 << (definitions::cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH + definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH - 1) };

  /*! First port handle */
  static constexpr tHandle cFIRST_PORT_HANDLE = cFIRST_PORT_INDEX << cSTAMP_BIT_WIDTH;

  /*! Maximum port index */
  enum { cMAX_PORT_INDEX = (1 << (definitions::cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH + definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH)) - 1 };


  /*!
   * \param positive_indices Positive handles? (or rather negative??)
   */
  tFrameworkElementRegister();

  ~tFrameworkElementRegister();

  /*!
   * Add element to this register
   *
   * \param framework_element Element to add
   * \param is_port Is this framework element a port?
   * \return Handle of element. This handle can be used to retrieve it later.
   */
  tHandle Add(tFrameworkElement& framework_element, bool is_port);

  /*!
   * \param handle Handle of element
   * \return Element (NULL if no element with this handle exists)
   */
  tFrameworkElement* Get(tHandle handle)
  {
    uint32_t index = handle >> cSTAMP_BIT_WIDTH;
    uint32_t primary_index = index >> definitions::cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH;
    if (array_chunks[primary_index])
    {
      tFrameworkElement* result = array_chunks[primary_index]->array[index & cSECONDARY_INDEX_MASK];
      return (result->GetHandle() == handle) ? result : NULL;
    }
    return NULL;
  }

  /*!
   * Copies all framework elements that currently exist (including ports) to the specified buffer
   * (no support for output iterators, since typically only using arrays and vector makes sense)
   *
   * \param result_buffer Pointer to the first element of the result buffer
   * \param max_elements Maximum number of elements to copy (size of result buffer)
   * \param start_from_handle Handle to start from. Together with 'max_elements', can be used to get all elements with multiple calls to this function - using a small result buffer.
   * \return Number of elements that were copied
   */
  size_t GetAllElements(tFrameworkElement** result_buffer, size_t max_elements, tHandle start_from_handle);

  /*!
   * Remove element with specified handle from register
  *
   * \param handle Handle
   */
  void Remove(tHandle handle);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Secondary array chunk */
  struct tSecondaryArray
  {
    std::array<tFrameworkElement*, cSECONDARY_ARRAY_SIZE> array;

    tSecondaryArray();
  };

  /*! Information on unused slot */
  struct tUnusedSlot
  {
    /*! Time when element in this slot was deleted */
    rrlib::time::tTimestamp delete_time;

    /*! Handle of deleted element */
    tHandle ex_handle;

    explicit tUnusedSlot(tHandle ex_handle) :
      delete_time(rrlib::time::Now()),
      ex_handle(ex_handle)
    {}
  };

  /*! Pointers to any secondary arrays */
  std::array < tSecondaryArray*, cPRIMARY_ARRAY_SIZE + 1 > array_chunks;

  /*! Next index for non-ports */
  uint32_t next_index;

  /*! Next index for ports */
  uint32_t next_port_index;

  /*! Stores unused slots */
  std::queue<tUnusedSlot> unused_slot_queue;

  /*! Stores unused slots */
  std::queue<tUnusedSlot> unused_port_slot_queue;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
