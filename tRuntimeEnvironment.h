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
/*!\file    core/tRuntimeEnvironment.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tRuntimeEnvironment
 *
 * \b tRuntimeEnvironment
 *
 * This is an application's main central class (root object). It contains groups and modules.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__tRuntimeEnvironment_h__
#define __core__tRuntimeEnvironment_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <map>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"
#include "core/tRuntimeListener.h"
#include "core/internal/tFrameworkElementRegister.h"

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
namespace internal
{
class tLinkEdge;
}

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Finroc runtime environment
/*!
 * This is Finroc's so-called runtime environment.
 * It is the root of the framework element hierarchy and performs various
 * management tasks.
 *
 * In order to modify the framework element hierarchy, a framework element
 * hierarchy lock must be acquired.
 */
class tRuntimeEnvironment : public tFrameworkElement
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tRuntimeEnvironment();

  virtual ~tRuntimeEnvironment();

  /*!
   * (usually called from main())
   * Register any relevant command line arguments at runtime.
   * Can be retrieved by GetCommandLineArgument later.
   *
   * \param option Name of command line option.
   * \param value Value of option.
   */
  void AddCommandLineArgument(const tString& option, const tString& value)
  {
    command_line_args.insert(std::pair<std::string, std::string>(option, value));
  }

  /*!
   * Add runtime listener
   *
   * \param listener Listener to add
   */
  void AddListener(tRuntimeListener& listener);

  /*!
   * Copies all framework elements that currently exist (including ports) to the specified buffer
   * (no support for output iterators, since typically only using arrays and vector makes sense)
   *
   * \param result_buffer Pointer to the first element of the result buffer
   * \param max_elements Maximum number of elements to copy (size of result buffer)
   * \param start_from_handle Handle to start from. Together with 'max_elements', can be used to get all elements with multiple calls to this function - using a small result buffer.
   * \return Number of elements that were copied
   */
  size_t GetAllElements(tFrameworkElement** result_buffer, size_t max_elements, tHandle start_from_handle = 0);

  /*!
   * Copies all ports that currently exist to the specified buffer
   * (no support for output iterators, since typically only using arrays and vector makes sense)
   *
   * \param result_buffer Pointer to the first element of the result buffer
   * \param max_ports Maximum number of ports to copy (size of result buffer)
   * \param start_from_handle Handle to start from. Together with 'max_elements', can be used to get all ports with multiple calls to this function - using a small result buffer.
   * \return Number of ports that were copied
   */
  size_t GetAllPorts(tAbstractPort** result_buffer, size_t max_ports, tHandle start_from_handle = internal::tFrameworkElementRegister::cFIRST_PORT_HANDLE);

  /*!
   * \return Timestamp when runtime environment was created
   */
  inline rrlib::time::tTimestamp GetCreationTime()
  {
    return creation_time;
  }

  /*/
   * \param name Name of command line argument
   * \return Value of argument - or "" if not set.
   */
  tString GetCommandLineArgument(const tString& name);

  /*!
   * Get Framework element from handle
   *
   * \param handle Handle of framework element
   * \return Pointer to framework element - or null if it has been deleted
   */
  tFrameworkElement* GetElement(tHandle handle);

  /*!
   * (IMPORTANT: This should not be called during static initialization)
   *
   * \return Singleton instance of Runtime environment
   */
  static tRuntimeEnvironment& GetInstance();

  /*!
   * get Port by handle
   *
   * \param port_handle port handle
   * \return Port - if initialized port with such handle exists - otherwise null.
   */
  tAbstractPort* GetPort(tHandle port_handle);

  /*!
   * \param link_name (relative) Fully qualified name of port
   * \return Port with this name - or null if it does not exist. Port may not be initialized yet.
   */
  tAbstractPort* GetPort(const tString& link_name);

  /*!
   * \return Framework element hierarchy changing operations need to acquire a lock on this mutex
   * (Only lock runtime for minimal periods of time!)
   */
  rrlib::thread::tRecursiveMutex& GetStructureMutex()
  {
    return structure_mutex;
  }

  /*!
   * Remove runtime listener
   *
   * \param listener Listener to remove
   */
  void RemoveListener(tRuntimeListener& listener);

  /*!
   * Using only the basic constructs from Finroc - things should shutdown cleanly automatically.
   *
   * However, if things like the TCP server are used, this method should be called
   * at the end of the program in order to shut everything down cleanly.
   */
  static void Shutdown();

  /*!
   * \return Is runtime environment currently shutting down?
   */
  static bool ShuttingDown();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tFrameworkElement;
  friend class tAbstractPort;
  friend class internal::tLinkEdge;

  /*! Global register of all framework elements */
  internal::tFrameworkElementRegister elements;

  /*! Edges dealing with linked ports */
  std::map<std::string, internal::tLinkEdge*> link_edges;

  /*! List of global link edges (link edges with two links are added to this list by tAbstractPort) */
  std::vector<std::unique_ptr<internal::tLinkEdge>> global_link_edges;

  /*! List with runtime listeners */
  rrlib::concurrent_containers::tSet < tRuntimeListener*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
        rrlib::concurrent_containers::set::storage::ArrayChunkBased<8, 31, definitions::cSINGLE_THREADED >> runtime_listeners;

  /*! Temporary buffer - may be used in synchronized context */
  std::string temp_buffer;

  /*! Alternative roots for links (usually remote runtime environments mapped into this one) */
  std::vector<tFrameworkElement*> alternative_link_roots;

  /*! Mutex for framework element hierarchy */
  rrlib::thread::tRecursiveMutex structure_mutex;

  /*! Raw pointer to runtime - that also exists during destruction */
  static tRuntimeEnvironment* instance_raw_ptr;

  /*! Timestamp when runtime environment was created */
  rrlib::time::tTimestamp creation_time;

  /*! Is RuntimeEnvironment currently active (and needs to be deleted?) */
  static bool active;

  /*! Command line arguments (used by parameters, for instance). Needs to be manually filled (usually in main()). */
  std::map<std::string, std::string> command_line_args;

  /*! Framework element that contains all framework elements that have no parent specified */
  tFrameworkElement* unrelated;


  /*!
   * Initializes the runtime environment. Needs to be called before any
   * other operation (especially getInstance()) is called.
   */
  static void InitialInit();

  /*!
   * (usually only called by LinkEdge)
   * Add link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void AddLinkEdge(const tString& link, internal::tLinkEdge& edge);

  /*!
   * Called before a framework element is initialized - can be used to create links etc. to this element etc.
   *
   * \param element Framework element that will be initialized soon
   */
  void PreElementInit(tFrameworkElement& element);

  /*!
   * Register framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to register
   * \param port Is framework element a port?
   * \return Handle of Framework element
   */
  tHandle RegisterElement(tFrameworkElement& fe, bool port);

  /*!
   * (usually only called by LinkEdge)
   * Remove link edge that is interested in specific link
   *
   * \param link link that edge is interested in
   * \param edge Edge to add
   */
  void RemoveLinkEdge(const tString& link, internal::tLinkEdge& edge);

  /*!
   * Called whenever a framework element was added/removed or changed
   *
   * \param change_type Type of change
   * \param element FrameworkElement that changed
   * \param edge_target Target of edge, in case of EDGE_CHANGE
   *
   * (Is called with structure mutex obtained... so method should not block)
   * (should only be called by FrameworkElement class)
   */
  void RuntimeChange(tRuntimeListener::tEvent change_type, tFrameworkElement& element, tAbstractPort* edge_target = NULL);

  /*!
   * Unregister framework element at RuntimeEnvironment.
   * This is done automatically and should not be called by a user.
   *
   * \param framework_element Element to remove
   */
  void UnregisterElement(tFrameworkElement& fe);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
