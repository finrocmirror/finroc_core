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

#ifndef core__plugin__tPlugins_h__
#define core__plugin__tPlugins_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tPlugin.h"
#include "core/plugin/tCreateFrameworkElementAction.h"

namespace finroc
{
namespace core
{
class tCreateExternalConnectionAction;

/*!
 * \author Max Reichardt
 *
 * This class is used for managing the Runtime's plugins
 */
class tPlugins : public util::tUncopyableObject
{
private:

  /*! All Plugins that are currently available */
  util::tSimpleList< ::std::shared_ptr<tPlugin> > plugins;

  /*! List with actions to create external connections */
  util::tSimpleList<tCreateExternalConnectionAction*> external_connections;

public:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "plugins");

private:

  void FindAndLoadPlugins();

  inline static const char* GetLogDescription()
  {
    return "Plugins";
  }

public:

  tPlugins() :
      plugins(),
      external_connections()
  {}

  /*!
   * Add Module Type
   * (objects won't be deleted by this class)
   *
   * \param cma CreateFrameworkElementAction to add
   */
  inline void AddModuleType(tCreateFrameworkElementAction* cma)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain, "Adding module type: ", cma->GetName(), " (", cma->GetModuleGroup(), ")");
    GetModuleTypes().Add(cma);
  }

  /*!
   * (possibly manually) add plugin
   *
   * \param p Plugin to add
   */
  inline void AddPlugin(tPlugin* p)
  {
    plugins.Add(::std::shared_ptr<tPlugin>(p));
    p->Init();
  }

  /*!
   * \return List with modules for external connections
   */
  inline util::tSimpleList<tCreateExternalConnectionAction*>* GetExternalConnections()
  {
    return &(external_connections);
  }

  /*!
   * \return Plugins singleton instance
   */
  inline static tPlugins* GetInstance()
  {
    static tPlugins instance;
    return &instance;
  }

  /*!
   * \return List with modules that can be instantiated in this runtime using the standard mechanism
   */
  inline util::tSimpleList<tCreateFrameworkElementAction*>& GetModuleTypes()
  {
    static util::tSimpleList<tCreateFrameworkElementAction*> module_types;
    return module_types;
  }

  /*!
   * \return List with plugins (do not modify!)
   */
  inline util::tSimpleList< ::std::shared_ptr<tPlugin> >* GetPlugins()
  {
    return &(plugins);
  }

  // closes dlopen-ed libraries
  class tDLCloser
  {
  public:
    util::tSimpleList<void*> loaded;

    tDLCloser() : loaded() {}
    ~tDLCloser();
  };

  /*!
   * Returns/loads CreateFrameworkElementAction with specified name and specified .so file.
   * (doesn't do any dynamic loading, if .so is already present)
   *
   * \param group Group (.jar or .so)
   * \param name Module type name
   * \return CreateFrameworkElementAction - null if it could not be found
   */
  tCreateFrameworkElementAction* LoadModuleType(const util::tString& group, const util::tString& name);

  /*!
   * Register module that can be used as external connection (e.g. in GUI)
   *
   * \param action Action to be registered
   * \return Action to be registered (same as above)
   */
  tCreateExternalConnectionAction* RegisterExternalConnection(tCreateExternalConnectionAction* action);

  /*!
   * Loads plugins
   */
  inline static void StaticInit()
  {
    tPlugins* p = GetInstance();
    p->FindAndLoadPlugins();
  }

};

} // namespace finroc
} // namespace core

#endif // core__plugin__tPlugins_h__
