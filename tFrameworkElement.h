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
/*!\file    core/tFrameworkElement.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief   Contains tFrameworkElement
 *
 * \b tFrameworkElement
 *
 * This is the central class in the finroc core.
 * It is the base class of Ports, Modules, Groups and the Runtime environment.
 * Framework elements are arranged in a tree.
 * They may be linked/referenced from other parts of the tree.
 */
//----------------------------------------------------------------------
#ifndef __core__tFrameworkElement_h__
#define __core__tFrameworkElement_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/concurrent_containers/tSet.h"
#include "rrlib/thread/tLock.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/definitions.h"
#include "core/log_messages.h"
#include "core/tAnnotatable.h"
#include "core/tFrameworkElementFlags.h"
#include "core/tRuntimeListener.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace runtime_construction
{
class tFinstructable;
}

namespace core
{
class tAbstractPort;
class tRuntimeEnvironment;
namespace internal
{
class tGarbageDeleter;
}

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Framework element
/*!
 * This is the central class in the finroc core.
 * It is the base class of Ports, Modules, Groups and the Runtime environment.
 * Framework elements are arranged in a tree.
 * They may be linked/referenced from other parts of the tree.
 *
 * Everything is thread-safe as long as methods are used.
 * Framework elements have their own memory management due to concurrency.
 * Thus, ManagedDelete() should be called instead of deleting them directly.
 *
 * When dealing with unknown framework elements - check isReady() to make sure
 * they are fully initialized and not already deleted.
 * Init needs to be called before framework elements can be used, as well as being visible
 * to remote runtime environments/clients.
 *
 * To prevent deleting of framework element while using it over a longer period of time,
 * lock it - or the complete runtime environment.
 *
 * A framework element can be locked with
 *   rrlib::thread::tLock lock(<framework_element>)
 *
 * That's why it inherits from tMutexLockOrder.
 * This also defines the lock order in which framework elements can be locked.
 * Generally, the framework element tree is locked from root to leaves.
 * So children's lock level needs to be larger than their parent's.
 *
 * tMutexLockOrder's secondary component is the element's unique handle in local runtime environment.
 * ("normal" elements have negative handle, while ports have positive ones)
 *
 */
class tFrameworkElement : public tAnnotatable
{
public:
  class tLink;

private:

  /*! Type of child list */
  typedef rrlib::concurrent_containers::tSet < tLink*, rrlib::concurrent_containers::tAllowDuplicates::NO, rrlib::thread::tNoMutex,
          rrlib::concurrent_containers::set::storage::ArrayChunkBased<8, 19, definitions::cSINGLE_THREADED >> tChildSet;

  /*! Iterator filter that accepts all elements */
  struct tIteratorFilterNone
  {
    static bool Accept(const tFrameworkElement& fe)
    {
      return true;
    }
  };

  /*! Iterator filter that accepts only ports */
  struct tIteratorFilterPorts
  {
    static bool Accept(const tFrameworkElement& fe)
    {
      return fe.IsPort();
    }
  };
  typedef typename tChildSet::tConstIterator tPlainChildIterator;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef tFrameworkElementFlag tFlag;
  typedef tFrameworkElementFlags tFlags;
  typedef uint32_t tHandle;

  // Iterator types
  class tChildIterator;
  template <typename ELEMENT, typename FILTER, bool INCLUDE_LINKS = true>
  class tFilteredChildIterator;
  class tSubElementIterator;
  typedef tFilteredChildIterator<tAbstractPort, tIteratorFilterPorts> tChildPortIterator;


  /*! All status flags */
  static constexpr tFrameworkElementFlags cSTATUS_FLAGS = tFrameworkElementFlag::READY | tFrameworkElementFlag::PUBLISHED | tFrameworkElementFlag::DELETED;

  /*!
   * \param parent Parent of framework element (can be NULL here and added to its parent via AddChild() before initialization)
   * \param name Name of framework element (will be shown in browser etc.). May be empty here and changed via SetName() before initialization.
   * \param flags Any flags for framework element. Constant flags can only be set in constructor.
   */
  explicit tFrameworkElement(tFrameworkElement* parent = nullptr, const tString& name = "", tFlags flags = tFlags());

  /*!
   * Add Child to framework element
   * (It will be initialized as soon as this framework element is)
   *
   * \param fe Framework element to add (must not have been initialized already - structure is fixed in this case)
   */
  inline void AddChild(tFrameworkElement& fe)
  {
    AddChild(fe.primary);
  }

  /*!
   * (implementation note: uses tChildIterator to determine the number of children)
   *
   * \return Number of child elements of this framework element.
   */
  size_t ChildCount() const;

  /*!
   * \return An iterator to iterate over this node's child ports. Initially points to the first port.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.ChildPortsBegin(); it != fe.ChildPortsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tChildPortIterator ChildPortsBegin() const
  {
    return tChildPortIterator(children->Begin(), children->End());
  }

  /*!
   * \return An iterator to iterate over this node's child ports pointing to the past-the-end port.
   */
  inline tChildPortIterator ChildPortsEnd() const
  {
    return tChildPortIterator();
  }

  /*!
   * \return An iterator to iterate over this node's children. Initially points to the first element.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.ChildrenBegin(); it != fe.ChildrenEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tChildIterator ChildrenBegin() const
  {
    return tChildIterator(children->Begin());
  }

  /*!
   * \return An iterator to iterate over this node's children pointing to the past-the-end element.
   */
  inline tChildIterator ChildrenEnd() const
  {
    return tChildIterator();
  }

  /*!
   * \return Returns constant and non-constant flags
   */
  inline tFlags GetAllFlags() const
  {
    return flags;
  }

  /*!
   * \param name Name
   * \return Returns first child with specified name. Returns nullptr if no child with name exists.
   */
  tFrameworkElement* GetChild(const tString& name) const;

  /*!
   * \param path Relative path
   * \return Returns framework element with specified relative path. Returns nullptr if no element can be found with specified path.
   */
  tFrameworkElement* GetChild(const tPath& path);

  /*!
   * Is specified flag set?
   *
   * \param flag Flag to check
   * \return Answer
   */
  inline bool GetFlag(tFlag flag) const
  {
    return flags.Get(flag);
  }

  /*!
   * \return Element's handle in local runtime environment
   */
  inline tHandle GetHandle() const
  {
    return handle;
  }

  /*!
   * \param link_index Index of link (0 = primary)
   * \return Link with specified index. Returns invalid link if no index with specified index exists.
   */
  const tLink& GetLink(size_t link_index) const;

  /*!
   * \return Number of links to this element (only ports have more than one)
   */
  size_t GetLinkCount() const;

  inline const tFrameworkElement& GetLogDescription() const
  {
    return *this;
  }

  /*!
   * \return Name of this framework element
   *
   * (Calling this function is non-blocking and thread-safe.
   *  Working with the returned string reference is thread-safe as well - until framework element is completely deleted.
   *  The referenced string will not change when the name of the framework element is changed.)
   */
  const tString& GetName() const
  {
    return *(primary.name);
  }

  /*!
   * \return Primary parent framework element
   */
  inline tFrameworkElement* GetParent() const
  {
    return primary.parent;
  }

  /*!
   * \param link_index Link that is referred to (0 = primary link)
   * \return Parent of framework element using specified link
   */
  tFrameworkElement* GetParent(int link_index) const;

  /*!
   * Returns first parent that has the specified flags
   *
   * \param parent_flags Flags to look for
   * \return Parent or null
   */
  tFrameworkElement* GetParentWithFlags(tFlags parent_flags) const;

  /*!
   * \return Path to this framework element (including this framework element)
   */
  inline tPath GetPath() const
  {
    tPath result;
    GetPath(result, primary);
    return result;
  }

  /*!
   * Obtains path to this framework element (including this framework element)
   *
   * \param result Object to store result in. If reused and sufficiently large, does not require memory allocation.
   * \param link_index Index of link to this object to use
   * \return Whether the result is a globally unique path
   */
  inline bool GetPath(tPath& result, size_t link_index = 0) const
  {
    return GetPath(result, link_index == 0 ? primary : GetLink(link_index));
  }

  /*!
   * Obtains path to this framework element (including this framework element)
   *
   * \param result Object to store result in. If reused and sufficiently large, does not require memory allocation.
   * \param link Link to this object to use
   * \return Whether the result is a globally unique path
   */
  bool GetPath(tPath& result, const tLink& link) const;

  /*!
   * (for convenience)
   * \return The one and only RuntimeEnvironment
   */
  static tRuntimeEnvironment& GetRuntime();

  /*!
   * (for convenience)
   * \return Framework element hierarchy changing operations need to acquire a lock on this mutex
   * (Only lock runtime for minimal periods of time!)
   */
  rrlib::thread::tRecursiveMutex& GetStructureMutex() const;

  /*!
   * Initialize this framework element and all framework elements in sub tree that were created by this thread
   * and weren't initialized already.
   *
   * This must be called prior to using framework elements - and in order to them being published.
   */
  void Init();

  /*!
   * Initializes all unitialized framework elements created by this thread
   */
  static void InitAll();

  /*!
   * Calling this method will cause this framework element to be initially shown/opened in tools such as
   * finstruct and fingui. Their parents will be expanded in the tree view.
   * In finstruct, the main view will be set to the element with the highest priority.
   * If multiple elements have the same priority, the first one will be shown.
   *
   * The method may only be called prior to this framework element's initialization.
   *
   * @param priority Priority. Higher values mean higher priority. The main thread container of an application typically has "1"
   */
  void InitiallyShowInTools(int32_t priority = 1);

  /*!
   * Is Runtime element a child of the specified Runtime element?
   * (also considers links)
   *
   * \param re Possible parent of this Runtime element
   * \param ignore_delete_flag Perform check even if delete flag is already set on object (deprecated in C++ - except of directly calling on runtime change)
   * \return Answer
   */
  bool IsChildOf(const tFrameworkElement& re, bool ignore_delete_flag = false) const;

  /*!
   * \return true before element is officially declared as being initialized
   */
  inline bool IsConstructing() const
  {
    return !IsReady();
  }

  /*!
   * \return Has framework element been deleted? - dangerous if you actually encounter this in C++...
   */
  inline bool IsDeleted() const
  {
    return flags.Get(tFlag::DELETED);
  }

  /*!
   * \return Is element a port?
   */
  inline bool IsPort() const
  {
    return flags.Get(tFlag::PORT);
  }

  /*!
   * \return Is framework element ready/fully initialized and not yet deleted?
   */
  inline bool IsReady() const
  {
    return flags.Get(tFlag::READY);
  }

  /*!
   * \return Is this the one and only tRuntimeEnvironment object in this process?
   */
  inline bool IsRuntime() const
  {
    return handle == 0;
  }

  /*!
   * Deletes element and all child elements
   */
  inline void ManagedDelete()
  {
    ManagedDelete(nullptr);
  }

  /*!
   * Called whenever static parameters of this framework element need to be (re)evaluated.
   * (can be overridden to handle this event).
   * This may also be called directly to trigger static parameter evaluation.
   *
   * This typically happens at initialization and when user changes them via finstruct.
   * (This is never called when thread in surrounding thread container is running.)
   * (This must only be called with lock on runtime registry.)
   */
  virtual void OnStaticParameterChange() {}

  /*!
   * \param name New Port name
   *
   * (Name may only be changed once.
   *  May only be called by creator thread before element is initialized.
   *  Prints an error message otherwise and does not modify name.)
   */
  void SetName(const tString& name);

  /*!
   * \param include_this Include this element (root of the subtree) in iteration?
   * \return An iterator to iterate over each element of the complete subtree below this framework element. Initially points to the first direct child.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.SubElementsBegin(); it != fe.SubElementsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tSubElementIterator SubElementsBegin(bool include_this = false)
  {
    return tSubElementIterator(*this, include_this);
  }

  /*!
   * \return An iterator to iterate over each element of the complete subtree below this framework element pointing to the past-the-end element.
   */
  inline tSubElementIterator SubElementsEnd() const
  {
    return tSubElementIterator();
  }

  /*!
   * This allows to store framework elements in smart pointers - and auto-delete them using safe tPortWrapperBase::ManagedDelete()
   */
  struct tDeleter
  {
    void operator()(tFrameworkElement* ptr) const
    {
      if (ptr)
      {
        ptr->ManagedDelete();
      }
    }
  };

  /*!
   * Framework elements are inserted as children of other framework element
   * using this connector class.
   * Using this extra class allows links in the tree.
   *
   * Framework elements "own" these links - they are deleted with framework element
   */
  class tLink
  {
    friend class tFrameworkElement;
  private:

    /* Framework element that link points to */
    tFrameworkElement& points_to;

    /*! Name of Framework Element - in link context */
    const tString* name;

    /*! Buffer used for storing (initial) name of framework element */
    tString name_buffer;

    /*! Parent - Element in which this link was inserted */
    tFrameworkElement* parent;

    /*! Next link for this framework element (=> singly-linked list) */
    tFrameworkElement::tLink* next;

  public:

    tLink(tFrameworkElement& pointed_to);

    /*!
     * \return Element that this link points to
     */
    inline tFrameworkElement& GetChild() const
    {
      return points_to;
    }

    /*!
     * \return Name of Framework Element - in link context
     */
    inline const tString& GetName() const
    {
      return *name;
    }

    /*!
     * \return Parent - Element in which this link was inserted
     */
    inline tFrameworkElement* GetParent() const
    {
      return parent;
    }

    /*!
     * \return Whether this link is invalid (does not exist)
     */
    inline bool IsInvalid() const
    {
      return this == cINVALID_LINK.get();
    }

    /*!
     * \return Is this a primary link?
     */
    inline bool IsPrimaryLink() const
    {
      return this == &(GetChild().primary);
    }
  };

  /*!
   * Used to iterate over a framework element's child elements (input iterator)
   */
  class tChildIterator : public std::iterator<std::input_iterator_tag, tFrameworkElement, size_t>
  {
    typedef std::iterator<std::input_iterator_tag, tFrameworkElement, size_t> tBase;
    friend class tFrameworkElement;

  public:

    inline typename tBase::reference operator*() const
    {
      return (*plain_iterator)->GetChild();
    }
    inline typename tBase::pointer operator->() const
    {
      return &(operator*());
    }
    inline tChildIterator& operator++()
    {
      ++plain_iterator;
      return *this;
    }
    inline tChildIterator operator ++ (int)
    {
      tChildIterator temp(*this);
      operator++();
      return temp;
    }
    inline const bool operator == (const tChildIterator &other) const
    {
      return plain_iterator == other.plain_iterator;
    }
    inline const bool operator != (const tChildIterator &other) const
    {
      return !(*this == other);
    }

  private:

    tChildIterator() : plain_iterator() {}
    tChildIterator(const tPlainChildIterator& plain_iterator) : plain_iterator(plain_iterator) {}

    /*! Wrapped plain child iterator */
    tPlainChildIterator plain_iterator;
  };

  /*!
   * Used to iterate over a framework element's child elements - only returning framework
   * elements accepted by the specifed filter
   */
  template <typename TElement, typename TFilter, bool INCLUDE_LINKS>
  class tFilteredChildIterator : public std::iterator<std::input_iterator_tag, TElement, size_t>
  {
    typedef std::iterator<std::input_iterator_tag, TElement, size_t> tBase;
    friend class tFrameworkElement;

  public:

    inline typename tBase::reference operator*() const
    {
      return static_cast<TElement&>((*plain_iterator)->GetChild());
    }
    inline typename tBase::pointer operator->() const
    {
      return &(operator*());
    }
    inline tFilteredChildIterator& operator++()
    {
      ++plain_iterator;
      AdvanceToAccepted();
      return *this;
    }
    inline tFilteredChildIterator operator ++ (int)
    {
      tChildIterator temp(*this);
      operator++();
      return temp;
    }
    inline const bool operator == (const tFilteredChildIterator &other) const
    {
      return plain_iterator == other.plain_iterator;
    }
    inline const bool operator != (const tFilteredChildIterator &other) const
    {
      return !(*this == other);
    }

    tFilteredChildIterator(const tPlainChildIterator& plain_iterator_begin, const tPlainChildIterator& plain_iterator_end) :
      plain_iterator(plain_iterator_begin),
      plain_iterator_end(plain_iterator_end)
    {
      AdvanceToAccepted();
    }

    tFilteredChildIterator() : plain_iterator(), plain_iterator_end() {}

  private:

    void AdvanceToAccepted()
    {
      while ((plain_iterator != plain_iterator_end) && (!(TFilter::Accept((*plain_iterator)->GetChild()) && (INCLUDE_LINKS || (*plain_iterator)->IsPrimaryLink()))))
      {
        ++plain_iterator;
      }
    }

    /*! Wrapped plain child iterator */
    tPlainChildIterator plain_iterator, plain_iterator_end;
  };

  /*!
   * Used to iterate over each element of the complete subtree below a framework element (input iterator)
   * Ignores links.
   */
  class tSubElementIterator : public std::iterator<std::input_iterator_tag, tFrameworkElement, size_t>
  {
    typedef std::iterator<std::input_iterator_tag, tFrameworkElement, size_t> tBase;
    friend class tFrameworkElement;

  public:

    // Operators needed for C++ Input Iterator

    inline typename tBase::reference operator*() const
    {
      return at_root ? *at_root : *iterator_stack[current_depth - 1];
    }
    inline typename tBase::pointer operator->() const
    {
      return &(operator*());
    }

    tSubElementIterator& operator++();

    tSubElementIterator operator ++ (int);

    inline const bool operator == (const tSubElementIterator &other) const
    {
      return current_depth == other.current_depth && at_root == other.at_root && std::equal(other.iterator_stack.begin(), other.iterator_stack.end(), iterator_stack.begin());
    }
    inline const bool operator != (const tSubElementIterator &other) const
    {
      return !(*this == other);
    }

  private:

    typedef tFilteredChildIterator<tFrameworkElement, tIteratorFilterNone, false> tNoLinkChildIterator;

    /*! Maximum tree depth the iterator implementation can handle */
    const static size_t cMAX_TREE_DEPTH = 100;

    /*! Array with current iterator in each tree level */
    std::array<tNoLinkChildIterator, cMAX_TREE_DEPTH> iterator_stack;

    /*! Current depth in tree */
    size_t current_depth;

    /*! Set, if currently at root element */
    tFrameworkElement* at_root;

    /*!
     * \param framework_element Framework element over whose subtree to iterate
     * \param include_root Include root element in iteration
     */
    tSubElementIterator(tFrameworkElement& framework_element, bool include_root);

    /*!
     * Constructor for end iterator
     */
    inline tSubElementIterator() : current_depth(0), at_root(NULL) {}
  };

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual ~tFrameworkElement();

  /*!
   * Create link to this framework element
   *
   * \param parent Parent framework element
   * \param link_name name of link
   */
  void Link(tFrameworkElement& parent, const tString& link_name);

  /*!
   * Publish updated framework element information
   *
   * \param change_type Type of change (see Constants in RuntimeListener class)
   */
  void PublishUpdatedInfo(tRuntimeListener::tEvent change_type);

  /*!
   * (may only be called before element is initialized or with structure lock)
   *
   * \param flag Flag to set
   * \param value Value to set value to
   */
  void SetFlag(tFlag flag, bool value = true);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tRuntimeEnvironment;
  friend class internal::tGarbageDeleter;
  friend class runtime_construction::tFinstructable;

  /*! Element's handle in local runtime environment */
  const tHandle handle;

  /*! Primary link to framework element - the place at which it actually is in FrameworkElement tree - contains name etc. */
  tLink primary;

#ifndef RRLIB_SINGLE_THREADED
  /*! Uid of thread that created this framework element */
  const int64_t creater_thread_uid;
#endif

  /*!
   * Flags of framework element
   * (may only be modified before element is initialized or with structure lock)
   */
  tFlags flags;  // TODO: Check whether splitting flags up in const and non-const might allow compiler optimizations?

  /*! Children (concurrent set for efficient, thread-safe iteration) - points to empty_child_set for ports - never NULL */
  tChildSet* children;

  /*! Empty child set for ports */
  static tChildSet empty_child_set;

  /*! Pointer to invalid link object */
  static std::unique_ptr<const tLink> cINVALID_LINK;

  /*!
   * Adds child to parent (automatically called by constructor - may be called again though)
   * using specified link
   *
   * - Removes child from any former parent
   * - Init() method of child is not called
   *
   * \param child Link to child to add
   */
  void AddChild(tLink& child);

  /*!
   * \return Have all parents (including link parents) been initialized?
   * (may only be called in runtime-registry-synchronized context)
   */
  bool AllParentsReady() const;

  /*!
   * Checks if specified link can be added to parent.
   * Aborts program if this is not the case.
   *
   * (Check: Framework elements may only have multiple children with the same name
   *  if explicitly allowed in runtime settings.)
   */
  void CheckForNameClash(const tLink& link) const;

  /*!
   * Initializes element and all child elements that were created by this thread
   * (helper method for init())
   * (may only be called in runtime-registry-synchronized context)
   */
  void CheckPublish();

  /*!
   * Deletes all children of this framework element.
   *
   * (may only be called in runtime-registry-synchronized context)
   */
  void DeleteChildren();

  /*!
   * Helper for GetChild(const tPath&)
   *
   * \param path Relative_path (may contain '.' and '..' elements)
   * \param path_index Current index in path
   * \param root Root Element
   * \return Returns framework element with specified relative path. Returns nullptr if no element can be found with specified path.
   */
  tFrameworkElement* GetChild(const tPath& path, uint path_index, tFrameworkElement& root);

  /*!
   * \return Number of links to this port
   * (should be called in synchronized context)
   */
  size_t GetLinkCountHelper() const;

  /*!
   * same as above, but non-const
   * (should be called in synchronized context)
   */
  tLink* GetLinkInternal(size_t link_index);

  /*!
   * Obtains path to this framework element (including this framework element)
   * (may only be called with structure-lock)
   *
   * \param result Object to store result in. If reused and sufficiently large, does not require memory allocation.
   * \param link Link to this object to use
   * \return Whether the result is a globally unique path
   */
  bool GetPathImplementation(tPath& result, const tLink& link) const;

  /*!
   * Initializes element and all child elements that were created by this thread
   * (helper method for Init())
   * (may only be called with structure-lock)
   */
  void InitImplementation();

  /*!
   * \return Is current thread the thread that created this object?
   */
  bool IsCreator() const;

  /*!
   * Deletes element and all child elements
   *
   * \param dont_detach Don't detach this link from parent (typically, because parent will clear child list)
   */
  void ManagedDelete(tLink* dont_detach);

  /*!
   * Callback on initialization of element.
   * Called after OnInitialization() of all children has been called.
   * The tree structure should be established by now (handle is valid)
   *
   * (called with lock on runtime structure mutex)
   * (method was called PostChildInit() in former Finroc versions).
   */
  virtual void OnInitialization()
  {
  }

  /*!
   * Callback on managed deletion of element (when ManagedDelete() is called)
   *
   * Prepares element for deletion.
   * Ports, for instance, are removed from edge lists etc.
   * The final deletion will be done by the GarbageCollector thread after a few seconds
   * (to ensure no other thread is working on this object anymore).
   *
   * Is called before OnManagedDelete() of children.
   *
   * (is called with lock on this framework element and possibly all of its parents,
   *  but not runtime structure mutex. Keep this in mind when cleaning up & joining threads.
   *  This is btw the place to do that.)
   * (method was called PrepareDelete() in former Finroc versions)
   */
  virtual void OnManagedDelete()
  {
  }
};

std::ostream& operator << (std::ostream& output, const tFrameworkElement& fe);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#endif
