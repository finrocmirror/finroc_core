//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/port/tPortWrapperBase.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-21
 *
 * \brief   Contains tPortWrapperBase
 *
 * \b tPortWrapperBase
 *
 * Port implementation classes (derived from tAbstractPort) are typically not used directly by an application developer.
 * Rather a wrapper class based on this class is used.
 * This has the following advantages:
 * - Wrapper class wraps and manages pointer to actual port. No pointers are needed to work with these classes - they are passed by value.
 * - Only parts of the API meant to be used by the application developer are exposed.
 * - Connect() methods can be hidden/reimplemented (via name hiding). This can be used to enforce that only certain connections can be created at compile time.
 */
//----------------------------------------------------------------------
#ifndef __plugins__data_ports__tPortWrapperBase_h__
#define __plugins__data_ports__tPortWrapperBase_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tAbstractPort.h"
#include "core/port/tAbstractPortCreationInfo.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Base class for port wrapper classes
/*!
 * Port implementation classes (derived from tAbstractPort) are typically not used directly by an application developer.
 * Rather a wrapper class based on this class is used.
 * This has the following advantages:
 * - Wrapper class wraps and manages pointer to actual port. No pointers are needed to work with these classes - they are passed by value.
 * - Only parts of the API meant to be used by the application developer are exposed.
 * - Connect() methods can be hidden/reimplemented (via name hiding). This can be used to enforce that only certain connections can be created at compile time.
 */
class tPortWrapperBase
{
  template <typename T, typename A>
  class HasSet;

  template <typename T, typename A>
  struct SetBaseClass;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef tAbstractPort::tConnectDirection tConnectDirection;
  typedef tFrameworkElement::tFlag tFlag;
  typedef tFrameworkElement::tFlags tFlags;

  tPortWrapperBase() :
    wrapped(nullptr)
  {}

  tPortWrapperBase(core::tAbstractPort* wrap) :
    wrapped(wrap)
  {}


  //TODO: Smarter port deletion

  /*!
   * Add annotation to this port
   *
   * \param ann Annotation
   * \throw Throws std::runtime_error if port wrapper is empty
   */
  void AddAnnotation(tAnnotation& ann);

  /*!
   * Connect port to specified partner port
   *
   * \param partner_port Partner port
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(tAbstractPort& partner_port, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    if (wrapped)
    {
      wrapped->ConnectTo(partner_port, connect_direction);
    }
  }

  /*!
   * Connect port to specified partner port
   *
   * \param partner_port Partner port
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(const tPortWrapperBase& partner_port, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    if (wrapped && partner_port.GetWrapped())
    {
      wrapped->ConnectTo(*partner_port.wrapped, connect_direction);
    }
  }

  /*!
   * Connect port to specified partner port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of partner port (relative to parent framework element)
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(const tString& link_name, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    if (wrapped)
    {
      wrapped->ConnectTo(link_name, connect_direction);
    }
  }

  /*!
   * Connect port to specified partner port
   *
   * \param partner_port_parent Parent of port to connect to
   * \param partner_port_name Name of port to connect to
   * \param warn_if_not_available Print warning message if connection cannot be established
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(tFrameworkElement& partner_port_parent, const tString& partner_port_name, bool warn_if_not_available = true, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    if (wrapped)
    {
      wrapped->ConnectTo(partner_port_parent, partner_port_name, warn_if_not_available, connect_direction);
    }
  }

  /*!
   * Disconnects all edges
   *
   * \param incoming disconnect incoming edges?
   * \param outgoing disconnect outgoing edges?
   */
  void DisconnectAll(bool incoming = true, bool outgoing = true)
  {
    if (wrapped)
    {
      wrapped->DisconnectAll(incoming, outgoing);
    }
  }

  /*!
   * Disconnect from specified port
   *
   * \param partner_port Port to disconnect from
   */
  void DisconnectFrom(const tPortWrapperBase& partner_port)
  {
    if (wrapped)
    {
      wrapped->DisconnectFrom(*partner_port.GetWrapped());
    }
  }

  /*!
   * Disconnect from port with specified link (removes link edges
   *
   * \param link Qualified link of connection partner
   */
  void DisconnectFrom(const tString& link)
  {
    if (wrapped)
    {
      wrapped->DisconnectFrom(link);
    }
  }

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \tparam TAnnotation Annotation type (with which annotation was added)
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  template <typename TAnnotation>
  TAnnotation* GetAnnotation() const
  {
    return wrapped ? wrapped->GetAnnotation<TAnnotation>() : nullptr;
  }

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \param rtti_name Annotation type name as obtained from C++ RTTI (typeid(...).name())
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  tAnnotation* GetAnnotation(const char* rtti_name) const
  {
    return wrapped ? wrapped->GetAnnotation(rtti_name) : nullptr;
  }

  /*!
   * \return Type of port data
   */
  inline rrlib::rtti::tType GetDataType() const
  {
    return wrapped ? wrapped->GetDataType() : rrlib::rtti::tType();
  }

  /*!
   * \param flag Flag to check
   * \return Is specified flag set?
   */
  inline bool GetFlag(tFrameworkElement::tFlag flag) const
  {
    return wrapped ? wrapped->GetFlag(flag) : false;
  }

  /*!
   * \return Log description (tRuntimeEnvironment if port wrapper is empty)
   */
  const tFrameworkElement& GetLogDescription() const;

  /*!
   * \return Name of wrapped framework element (see tFrameworkElement::GetName())
   */
  inline const tString& GetName() const
  {
    return wrapped ? wrapped->GetName() : cEMPTY_PORT_WRAPPER;
  }

  /*!
   * \return Primary parent framework element
   */
  inline tFrameworkElement* GetParent()
  {
    return wrapped ? wrapped->GetParent() : nullptr;
  }

  /*!
   * \return Wrapped port. For rare case that someone really needs to access ports.
   */
  inline tAbstractPort* GetWrapped() const
  {
    return wrapped;
  }

  /*!
   * Initialize this port.
   * This must be called prior to using port
   * (usually done by initializing parent)
   * - and in order port being published.
   */
  inline void Init()
  {
    if (wrapped)
    {
      wrapped->Init();
    }
  }

  /*!
   * (slightly expensive)
   * \return Is port currently connected?
   */
  inline bool IsConnected() const
  {
    return wrapped && wrapped->IsConnected();
  }

  /*!
   * \return Has port been deleted? (also true, if port wrapper is empty)
   */
  inline bool IsDeleted() const
  {
    return (!wrapped) || wrapped->IsDeleted();
  }

  /*!
   * \return Is framework element ready/fully initialized and not yet deleted?
   */
  inline bool IsReady() const
  {
    return wrapped && wrapped->IsReady();
  }

  /*!
   * Deletes port wrapped by this port wrapper class.
   * Port wrapper is empty after calling this.
   */
  void ManagedDelete()
  {
    if (wrapped)
    {
      wrapped->ManagedDelete();
    }
    wrapped = nullptr;
  }

  /*!
   * Are name of this element and String 'other' identical?
   * (result is identical to getName().equals(other); but more efficient in C++)
   *
   * \param other Other String
   * \return Result
   */
  inline bool NameEquals(const tString& other) const
  {
    return wrapped && wrapped->NameEquals(other);
  }

  // using this operator, it can be checked conveniently in PortListener's PortChanged()
  // whether origin port is the same port as this object wraps
  bool operator ==(const tAbstractPort& p) const
  {
    return wrapped == &p;
  }
  bool operator !=(const tAbstractPort& p) const
  {
    return wrapped != &p;
  }
  bool operator ==(const tPortWrapperBase& p) const
  {
    return wrapped == p.wrapped;
  }
  bool operator !=(const tPortWrapperBase& p) const
  {
    return wrapped != p.wrapped;
  }


  /*!
   * Many port constructors have a variadic argument list.
   *
   * Instead of providing suitable constructors for all types of sensible
   * combinations of the numerous (often optional) construction parameters,
   * there is only one constructor with a variadic argument list.
   *
   * This utility class is used to process this list and store the parameters
   * in its base class T which bundles various parameters for the creation of
   * ports.
   * Parameters that are not specified have a default value (which might
   * explicitly indicate that they have not been set).
   *
   * \tparam TParameterSet Base class that handles constructor arguments.
   *                       For each constructor argument TParameterSet::Set(argument) is called.
   */
  template <typename TParameterSet>
  class tConstructorArguments : public TParameterSet
  {
    //----------------------------------------------------------------------
    // Public methods and typedefs
    //----------------------------------------------------------------------
  public:

    tConstructorArguments()
    {}

    /*!
     * Constructor takes variadic argument list... just any properties to be
     * assigned to port.
     *
     * It calls Set(argument) for every argument.
     * A static assertion is thrown if an argument of an invalid type is provided.
     */
    template <typename ARG1, typename ... TArgs>
    explicit tConstructorArguments(ARG1&& argument1, TArgs&&... rest) :
      TParameterSet()
    {
      static_assert(!std::is_base_of<tPortWrapperBase, ARG1>::value, "Invalid port type for copy construction");
      this->Set(std::forward<ARG1>(argument1));
      this->ProcessArguments(std::forward<TArgs>(rest)...);
    }

    /*!
     * This Set() method will set a suitable parameter in the base class
     * depending on the type of the argument passed to it.
     *
     * A static assertion is thrown if an argument of an invalid type is provided.
     */
    template <typename TArgument>
    void Set(TArgument&& argument)
    {
      typedef typename SetBaseClass<TParameterSet, TArgument>::type tBase;
      tBase::Set(std::forward<TArgument>(argument));
    }

    //----------------------------------------------------------------------
    // Private fields and methods
    //----------------------------------------------------------------------
  private:

    /*! Process constructor arguments */
    void ProcessArguments() {}

    template <typename A, typename ... ARest>
    void ProcessArguments(A&& argument1, ARest&&... rest)
    {
      this->Set(std::forward<A>(argument1));
      this->ProcessArguments(std::forward<ARest>(rest)...);
    }
  };

  /*! used in enable_if expressions of constructors in derived classes */
  class tNoArgument {};

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  /*!
   * \param wrapped Wrapped port
   */
  void SetWrapped(tAbstractPort* wrapped)
  {
    this->wrapped = wrapped;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Wrapped port */
  tAbstractPort* wrapped;

  /*! Name returned by empty port wrappers */
  static const std::string cEMPTY_PORT_WRAPPER;

  /*!
   * Type trait that determines whether type T has a Set() method suitable
   * for an argument of type A.
   * Used by tConstructorArguments.
   */
  template <typename T, typename A>
  class HasSet
  {
    template <typename U>
    static U &Make();

    template <typename U = T, typename V = A>
    static int16_t Test(decltype(Make<U>().Set(Make<A>()))*);
    static int32_t Test(...);

  public:

    enum { value = sizeof(Test(Make<void*>())) == sizeof(int16_t) };
  };

  /*!
   * Type trait to determine base class of T that has as suitable Set() method
   * for an argument of type A.
   * Result may be T itself.
   */
  template <typename T, typename A>
  struct SetBaseClass
  {
    static_assert(!std::is_same<T, core::tAbstractPortCreationInfo::tBase>::value, "An argument of type A is not supported by the port constructor");

    struct TypeReplicator
    {
      typedef T type;
    };

    typedef typename std::conditional<HasSet<T, A>::value, TypeReplicator, SetBaseClass<typename T::tBase, A>>::type::type type;
  };
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
