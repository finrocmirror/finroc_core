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
#ifndef core__port__tCombinedPointer_h__
#define core__port__tCombinedPointer_h__

namespace finroc
{
namespace core
{

namespace tCombinedPointerOps
{

/** Mask for last three bits */
static const size_t cINFO_MASK = 0x7;

/** Mask for pointer bits */
static const size_t cPOINTER_MASK = (0 - 1) & ~0x7;

/** create combined pointer (PTR must be subclass of CombinedPointer<T> */
template <typename cPTR, typename T>
cPTR* Create(T* ptr, size_t info = 0)
{
  assert((info & cPOINTER_MASK) == 0);
  return (cPTR*)(((size_t)ptr) | info);
}

template <typename T>
T* GetPointer(size_t raw)
{
  return (T*)(raw & cPOINTER_MASK);
}

}

/*
 * This class wraps a pointer to 8-bit-aligned data and stores additional custom information
 * in the last three pointer bits. This way, the custom data can be set and read together with the
 * pointer in an atomic operation.
 *
 * It works somewhat similar to smart pointer classes.
 * Obviously, it is a hack: but using is very efficient (both memory and CPU) and elegant (probably
 * depends on the personal taste :-) - but try to have volatile and non-volatile versions when
 * wrapping pointer as member of class (as it was implemented before) -
 * and then there are all these constructors and destructors :-/ ).
 *
 * The class is never constructed or destructed - use CombinedPointerOps for creation.
 * Destruction is not necessary, because all information is stored in the pointer to this class :-)
 */
template < typename T = void >
class tCombinedPointer
{
public:

  /*inline size_t getRaw() {
    return ptr;
  }*/

  /** Get Pointer */
  inline T* GetPointer() const
  {
    return (T*)(((size_t)this) & tCombinedPointerOps::cPOINTER_MASK);
  }

  /** Get additional info */
  inline size_t GetInfo() const
  {
    return ((size_t)this) & tCombinedPointerOps::cINFO_MASK;
  }

  /** Set pointer and additional info - info must lie in range 0 to 7 */
  /*inline void set(T* pointer, size_t info = 0) {
    assert(info & CombinedPointerOps::POINTER_MASK == 0);
    ptr = ((size_t)pointer) | info;
  }
  inline void set(void* combined) {
    set((size_t)combined);
  }
  inline void set(size_t combined) {
    ptr = combined;
  }
  inline void set(const CombinedPointer& cp) {
    ptr = cp.ptr;
  }*/

  /** atomic featch and store - only works with internal type of tbb::atomic */
  /*inline void* getAndSet(void* combined) {
    return (void*)getAndSet((size_t)combined);
  }
  inline size_t getAndSet(size_t combined) {
    return ptr.fetch_and_store(combined);
  }
  inline size_t getAndSet(const CombinedPointer& cp) {
    return ptr.fetch_and_store(cp.ptr);
  }*/

  /** operator definitions */
  /*inline bool operator==(const CombinedPointer& other) const {
    return ptr == other.ptr;
  }
  inline bool operator==(size_t combined) const {
    return ptr == combined;
  }
  inline bool operator==(void* combined) const {
    return ptr == (size_t)combined;
  }

  inline void operator=(const CombinedPointer& other) {
    ptr = other.ptr;
  }
  inline void operator=(size_t combined) {
    ptr = combined;
  }
  inline void operator=(void* combined) {
    ptr = (size_t)combined;
  }

  inline operator size_t() const {
    return ptr;
  }*/
};


} // namespace finroc
} // namespace core


#endif // core__port__tCombinedPointer_h__
