/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef core__port__tPortDataPtr_h__
#define core__port__tPortDataPtr_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/portdatabase/typeutil.h"
#include <boost/utility.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/utility/enable_if.hpp>

namespace finroc
{
namespace core
{

namespace detail
{
template <bool b>
struct tManagerLookup
{
  typedef tPortDataManager tManager;
  typedef tPortDataManager tManagerTL;
};

template <>
struct tManagerLookup<true>
{
  typedef tCCPortDataManager tManager;
  typedef tCCPortDataManagerTL tManagerTL;
};

}

template <typename T>
class tPort;

template <typename T>
class tPortUtil;

class tPortDataPtrBase : public boost::noncopyable
{
public:

  /** Mode that pointer was created in */
  enum tMode { eUNUSED, eNORMAL };
};

/*!
 * Special port data pointer class. Meant for users of tPort class.
 * It is more or less a customized std::unique_ptr - and used very similarly.
 * Class can be used directly. However, it preferred to use the type tPort<T>::tDataPtr
 */
template <typename T>
class tPortDataPtr : public tPortDataPtrBase
{
public:
  typedef typename detail::tManagerLookup<typeutil::tUseCCType<T>::value>::tManager tManager;
  typedef typename detail::tManagerLookup<typeutil::tUseCCType<T>::value>::tManagerTL tManagerTL;

private:
  friend class tPort<T>;
  friend class tPortUtilHelper;
  friend class tPortDataPtr<rrlib::serialization::tGenericObject>;
  friend class tPortDataPtr<const T>;

  /** Pointer to data */
  T* data;

  /** Pointer to manager */
  union
  {
    tManager* manager;
    tManagerTL* manager_tl;
  };

  /** Mode of pointer */
  tMode mode;

  static inline T* GetData(tManager* mgr)
  {
    rrlib::serialization::tGenericObject* go = mgr->GetObject();
    return go->GetData<T>();
  }

  template < typename Q = tManagerTL >
  static inline T* GetData(typename boost::enable_if_c<typeutil::tUseCCType<T>::value, Q>::type* mgr)
  {
    if (mgr == NULL)
    {
      return NULL;
    }
    rrlib::serialization::tGenericObject* go = mgr->GetObject();
    return go->GetData<T>();
  }

public:

  tPortDataPtr() : data(NULL), manager(NULL), mode(eNORMAL) {}

  // Standard constructor
  tPortDataPtr(T* data_, tManager* manager_, tMode mode_ = eNORMAL) : data(data_), manager(manager_), mode(mode_)
  {}

  // Standard constructor
  template < typename Q = tManagerTL >
  tPortDataPtr(T* data_, typename boost::enable_if_c<typeutil::tUseCCType<T>::value, Q*>::type manager_) : data(data_), manager_tl(manager_), mode(eUNUSED)
  {}

  // Standard constructor
  explicit tPortDataPtr(tManager* manager_, tMode mode_ = eNORMAL) : data(GetData(manager_)), manager(manager_), mode(mode_)
  {}

  // Standard constructor
  template < typename Q = tManagerTL >
  explicit tPortDataPtr(typename boost::enable_if_c<typeutil::tUseCCType<T>::value, Q*>::type manager_) : data(GetData(manager)), manager_tl(manager_), mode(eUNUSED)
  {}

  // Move constructor
  tPortDataPtr(tPortDataPtr<T> && o) : data(NULL), manager(NULL), mode(eNORMAL)
  {
    std::swap(data, o.data);
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
  }

  // Move constructor (from non-const)
  template < typename Q = T >
  tPortDataPtr(typename boost::enable_if_c<boost::is_const<T>::value, tPortDataPtr<typename boost::remove_const<Q>::type> >::type && o) : data(NULL), manager(NULL), mode(eNORMAL)
  {
    data = o.data;
    o.data = NULL;
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
  }

  // Move assignment
  tPortDataPtr& operator=(tPortDataPtr<T> && o)
  {
    std::swap(data, o.data);
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
    return *this;
  }

  // Move assignment (GenericObject to T)
  template < typename G = rrlib::serialization::tGenericObject >
  tPortDataPtr& operator=(tPortDataPtr < typename boost::enable_if_c < !boost::is_same<T, G>::value, G >::type > && o)
  {
    rrlib::serialization::tGenericObject* go = o.data;
    data = (go == NULL) ? NULL : go->GetData<T>();
    o.data = NULL;
    manager = (o.manager == NULL) ? NULL : static_cast<tManager*>(o.manager);
    o.manager = NULL;
    std::swap(mode, o.mode);
    return *this;
  }

  ~tPortDataPtr()
  {
    if (mode == eNORMAL)
    {
      if (manager != NULL)
      {
        manager->HandlePointerRelease();
        manager = NULL;
        data = NULL;
      }
    }
    else
    {
      assert((manager_tl == NULL) && "Buffers acquired via getUnuserBuffer() have to be published via port!");
    }
  }

  void reset()
  {
    Reset();
  }

  /** Reset pointer (releases lock) */
  void Reset()
  {
    if (mode == eNORMAL)
    {
      if (manager != NULL)
      {
        manager->HandlePointerRelease();
        manager = NULL;
        data = NULL;
      }
    }
    else
    {
      assert((manager_tl == NULL) && "Buffers acquired via getUnuserBuffer() have to be published via port!");
    }
  }

  T& operator*() const
  {
    assert(data != NULL);
    return *data;
  }

  T* operator->() const
  {
    return Get();
  }

  T* Get() const
  {
    assert(data != NULL);
    return data;
  }

  T* get() const
  {
    return Get();
  }

  operator const void*() const
  {
    return data;
  }

  operator const tPortDataPtr<const T>&() const
  {
    return *reinterpret_cast<const tPortDataPtr<const T>*>(this);
  }

  operator tPortDataPtr<const T>&()
  {
    return *reinterpret_cast<tPortDataPtr<const T>*>(this);
  }

  /*!
   * \return Pointer to manager of data
   */
  tManager* GetManager() const
  {
    assert(mode == eNORMAL);
    return manager;
  }
};

template <>
class tPortDataPtr<rrlib::serialization::tGenericObject> : public tPortDataPtrBase
{
public:
  typedef tReusableGenericObjectManager tManager;

private:
  typedef rrlib::serialization::tGenericObject T;

  friend class tPort<T>;
  friend class tPortUtil<T>;

public: // TODO ... public because we cannot declare tPortDataPtr<U> as friend

  /** Pointer to data */
  T* data;

  /** Pointer to manager */
  tManager* manager;

  /** Mode of pointer */
  tMode mode;

  tPortDataPtr() : data(NULL), manager(NULL), mode(eNORMAL) {}

  // Standard constructor
  tPortDataPtr(T* data_, tManager* manager_) : data(data_), manager(manager_), mode(eNORMAL)
  {}

  // Standard constructor
  explicit tPortDataPtr(tManager* manager_) : data(manager_ == NULL ? NULL : manager_->GetObject()), manager(manager_), mode(eNORMAL)
  {}

  // Move constructor
  tPortDataPtr(tPortDataPtr<T> && o) : data(NULL), manager(NULL), mode(eNORMAL)
  {
    std::swap(data, o.data);
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
  }

  // Move constructor (from non-const)
  template < typename Q = T >
  tPortDataPtr(typename boost::enable_if_c<boost::is_const<T>::value, tPortDataPtr<typename boost::remove_const<Q>::type> >::type && o) : data(NULL), manager(NULL), mode(eNORMAL)
  {
    std::swap(data, o.data);
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
  }

  // Move assignment
  tPortDataPtr& operator=(tPortDataPtr<T> && o)
  {
    std::swap(data, o.data);
    std::swap(manager, o.manager);
    std::swap(mode, o.mode);
    return *this;
  }

  // Move assignment (T to GenericObject)
  template <typename U>
  tPortDataPtr& operator=(tPortDataPtr<U> && o)
  {
    o.data = NULL;
    if (o.manager != NULL)
    {
      data = o.manager->GetObject();
      manager = o.manager;
    }
    else
    {
      data = NULL;
      manager = NULL;
    }
    o.manager = NULL;
    std::swap(mode, o.mode);
    return *this;
  }

  ~tPortDataPtr()
  {
    if (manager != NULL)
    {
      manager->GenericLockRelease();
      manager = NULL;
      data = NULL;
    }
  }

  void reset()
  {
    Reset();
  }

  /** Reset pointer (releases lock) */
  void Reset()
  {
    if (manager != NULL)
    {
      manager->GenericLockRelease();
      manager = NULL;
      data = NULL;
    }
  }

  T& operator*() const
  {
    assert(data != NULL);
    return *data;
  }

  T* operator->() const
  {
    return Get();
  }

  T* Get() const
  {
    assert(data != NULL);
    return data;
  }

  T* get() const
  {
    return Get();
  }

  operator const void*() const
  {
    return data;
  }

  /*!
   * \return Pointer to manager of data
   */
  tManager* GetManager() const
  {
    assert(mode == eNORMAL);
    return manager;
  }

  /*!
   * \return Pointer to manager of data - if manager has type U - otherwise NULL
   */
  template <typename U>
  U* GetManagerT() const
  {
    assert(mode == eNORMAL);
    if (typeid(*manager).name() == typeid(U).name())
    {
      return static_cast<U*>(manager);
    }
    return NULL;
  }
};


template <typename T, typename U>
inline bool operator!=(const tPortDataPtr<T>& x, const tPortDataPtr<U>& y)
{
  return x.get() != y.get();
}

template <typename T, typename U>
inline bool operator<(const tPortDataPtr<T>& x, const tPortDataPtr<U>& y)
{
  return x.get() < y.get();
}

template <typename T, typename U>
inline bool operator<=(const tPortDataPtr<T>& x, const tPortDataPtr<U>& y)
{
  return x.get() <= y.get();
}

template <typename T, typename U>
inline bool operator>(const tPortDataPtr<T>& x, const tPortDataPtr<U>& y)
{
  return x.get() > y.get();
}

template <typename T, typename U>
inline bool operator>=(const tPortDataPtr<T>& x, const tPortDataPtr<U>& y)
{
  return x.get() >= y.get();
}

} // namespace finroc
} // namespace core

#endif // core__port__tPortDataPtr_h__
