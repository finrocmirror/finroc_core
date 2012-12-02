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
 * Port classes are not directly used by an application developer.
 * Rather a wrapped class based on this class is used.
 * This has the following advantages:
 * - Wrapped wraps and manages pointer to actual port. No pointers are needed to work with these classes
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
 * Port classes are not directly used by an application developer.
 * Rather a wrapped class based on this class is used.
 * This has the following advantages:
 * - Wrapped wraps and manages pointer to actual port. No pointers are needed to work with these classes
 * - Only parts of the API meant to be used by the application developer are exposed.
 * - Connect() methods can be hidden/reimplemented (via name hiding). This can be used to enforce that only certain connections can be created at compile time.
 */
class tPortWrapperBase
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef tAbstractPort::tConnectDirection tConnectDirection;

  tPortWrapperBase() :
    wrapped(NULL)
  {}

  //TODO: Smarter port deletion

  /*!
   * Add annotation to this port
   *
   * \param ann Annotation
   */
  inline void AddAnnotation(tAnnotation& ann)
  {
    wrapped->AddAnnotation(ann);
  }

  /*!
   * Connect port to specified partner port
   *
   * \param source Partner port
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(tAbstractPort& source, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    wrapped->ConnectTo(source, connect_direction);
  }

  /*!
   * Connect port to specified partner port
   *
   * \param source Partner port
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(const tPortWrapperBase& source, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    wrapped->ConnectTo(*source.wrapped, connect_direction);
  }

  /*!
   * Connect port to specified partner port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of partner port (relative to parent framework element)
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  inline void ConnectTo(const tString& src_link, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    wrapped->ConnectTo(src_link, connect_direction);
  }

  /*!
   * Connect port to specified partner port
   *
   * \param partner_port_parent Parent of port to connect to
   * \param partner_port_name Name of port to connect to
   * \param warn_if_not_available Print warning message if connection cannot be established
   * \param connect_direction Direction for connection. "AUTO" should be appropriate for almost any situation. However, another direction may be enforced.
   */
  void ConnectTo(tFrameworkElement& src_port_parent, const tString& src_port_name, bool warn_if_not_available = true, tConnectDirection connect_direction = tConnectDirection::AUTO)
  {
    wrapped->ConnectTo(src_port_parent, src_port_name, warn_if_not_available, connect_direction);
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
    return wrapped->GetAnnotation<TAnnotation>();
  }

  /*!
   * Obtain annotation of specified type attached to this object
   *
   * \param rtti_name Annotation type name as obtained from C++ RTTI (typeid(...).name())
   * \return Annotation. Null if this object has no annotation of specified type attached.
   */
  tAnnotation* GetAnnotation(const char* rtti_name) const
  {
    return wrapped->GetAnnotation(rtti_name);
  }

  /*!
   * Same as getName()
   * (except that we return a const char*)
   */
  inline const char* GetCName() const
  {
    return wrapped->GetCName();
  }

  /*!
   * \return Type of port data
   */
  inline const rrlib::rtti::tType GetDataType() const
  {
    return wrapped->GetDataType();
  }

  /*!
   * \return Log description
   */
  inline const tFrameworkElement& GetLogDescription() const
  {
    return wrapped->GetLogDescription();
  }

  /*!
   * \return Name of this framework element
   */
  inline const tString GetName() const
  {
    return wrapped->GetName();
  }

  /*!
   * \return Primary parent framework element
   */
  inline tFrameworkElement* GetParent()
  {
    return wrapped->GetParent();
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
    wrapped->Init();
  }

  /*!
   * (slightly expensive)
   * \return Is port currently connected?
   */
  inline bool IsConnected() const
  {
    return wrapped->IsConnected();
  }

  /*!
   * \return Has port been deleted? (you should not encounter this)
   */
  inline bool IsDeleted() const
  {
    return wrapped->IsDeleted();
  }

  /*!
   * \return Is framework element ready/fully initialized and not yet deleted?
   */
  inline bool IsReady() const
  {
    return wrapped->IsReady();
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
    return wrapped->NameEquals(other);
  }

  // using this operator, it can be checked conveniently in PortListener's PortChanged()
  // whether origin port is the same port as this object wraps
  bool operator ==(const tAbstractPort* p) const
  {
    return wrapped == p;
  }

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
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
