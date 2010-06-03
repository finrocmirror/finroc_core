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

namespace finroc
{
namespace core
{
template<typename T>
const int tRemoteCoreRegister<T>::cLEVEL_ONE_BLOCK_SIZE;

template<typename T>
const int tRemoteCoreRegister<T>::cLEVEL_TWO_BLOCK_SIZE;

template<typename T>
const int tRemoteCoreRegister<T>::cLEVEL_TWO_MASK;

template<typename T>
const int tRemoteCoreRegister<T>::cLEVEL_ONE_MASK;

template<typename T>
const int tRemoteCoreRegister<T>::cLEVEL_ONE_SHIFT;

template<typename T>
tRemoteCoreRegister<T>::~tRemoteCoreRegister()
{
  for (int i = 0; i < cLEVEL_ONE_BLOCK_SIZE; i++)
  {
    delete elements[i];
    elements[i] = NULL;
  }
}

template<typename T>
T tRemoteCoreRegister<T>::Get(int handle)
{
  int lv1_block = (handle & cLEVEL_ONE_MASK) >> cLEVEL_ONE_SHIFT;
  int lv2_block = handle & cLEVEL_TWO_MASK;
  util::tArrayWrapper<T>* cur_lvl2_block = GetLvl2Element(lv1_block);
  if (cur_lvl2_block != NULL)
  {
    return cur_lvl2_block->Get(lv2_block);
  }
  return NULL;
}

template<typename T>
void tRemoteCoreRegister<T>::Put(int handle, const T& elem)
{
  util::tLock lock1(this);
  int lv1_block = (handle & cLEVEL_ONE_MASK) >> cLEVEL_ONE_SHIFT;
  int lv2_block = handle & cLEVEL_TWO_MASK;
  util::tArrayWrapper<T>* cur_lvl2_block = GetLvl2Element(lv1_block);
  if (cur_lvl2_block == NULL)
  {
    cur_lvl2_block = new util::tArrayWrapper<T>(cLEVEL_TWO_BLOCK_SIZE);
    SetLvl2Element(lv1_block, cur_lvl2_block);
  }
  assert((cur_lvl2_block->Get(lv2_block) == NULL));
  cur_lvl2_block->Set(lv2_block, elem);
  assert((Get(handle) == elem));
}

template<typename T>
void tRemoteCoreRegister<T>::Remove(int handle)
{
  util::tLock lock1(this);
  int lv1_block = (handle & cLEVEL_ONE_MASK) >> cLEVEL_ONE_SHIFT;
  int lv2_block = handle & cLEVEL_TWO_MASK;
  util::tArrayWrapper<T>* cur_lvl2_block = GetLvl2Element(lv1_block);
  assert(((cur_lvl2_block != NULL)) && "Trying to remove non-existing element");
  assert(((cur_lvl2_block->Get(lv2_block) != NULL)) && "Trying to remove non-existing element");
  cur_lvl2_block->Set(lv2_block, NULL);
}

template<typename T>
T tRemoteCoreRegister<T>::tIterator::Next()
{
  while (true)
  {
    if (lvl2_idx >= cLEVEL_TWO_BLOCK_SIZE - 1)
    {
      lvl2_idx = 0;
      do
      {
        lvl1_idx++;
        if (lvl1_idx == cLEVEL_ONE_BLOCK_SIZE)    // we're finished
        {
          return NULL;
        }
        cur_lvl2_block = outer_class_ptr->GetLvl2Element(lvl1_idx);
      }
      while (cur_lvl2_block == NULL);
    }
    else
    {
      lvl2_idx++;
    }
    T elem = cur_lvl2_block->Get(lvl2_idx);
    if (elem != NULL)
    {
      return elem;
    }
  }
}

template<typename T>
void tRemoteCoreRegister<T>::tIterator::Reset()
{
  cur_lvl2_block = NULL;
  lvl1_idx = -1;
  lvl2_idx = cLEVEL_TWO_BLOCK_SIZE;
}

} // namespace finroc
} // namespace core

