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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PLUGIN__TPLUGINS_H
#define CORE__PLUGIN__TPLUGINS_H

#include "finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tPlugin.h"
#include "core/plugin/tCreateExternalConnectionAction.h"
#include "core/plugin/tPluginManager.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This class is used for managing the Runtime's plugins
 */
class tPlugins : public util::tUncopyableObject
{
private:
  /*implements HTTPResource*/

  //  /** relative path of packages that contain Widgets and Interfaces */
  //  private static final Class<?> GUI_ROOT_CLASS = JMCAGUI.class;
  //  private static final String WIDGETPACKAGENAME = "widgets";
  //
  /*! Plugins singleton instance */
  static ::std::tr1::shared_ptr<tPlugins> instance;

  /*! All Plugins that are currently available */
  util::tSimpleList< ::std::tr1::shared_ptr<tPlugin> > plugins;

  /*! List with actions to create external connections */
  util::tSimpleList< ::std::tr1::shared_ptr<tCreateExternalConnectionAction> > external_connections;

  /*! Plugin manager instance */
  tPluginManager plugin_manager;

public:

  /*! Log domain for this class */
  CREATE_NAMED_LOGGING_DOMAIN(log_domain, "plugins");

private:

  void FindAndLoadPlugins();

  inline static const char* GetLogDescription()
  {
    return "Plugins";
  }

public:

  tPlugins() :
      plugins(),
      external_connections(),
      plugin_manager()
  {}

  /*!
   * (possibly manually) add plugin
   *
   * \param p Plugin to add
   */
  inline void AddPlugin(tPlugin* p)
  {
    plugins.Add(::std::tr1::shared_ptr<tPlugin>(p));
    p->Init(plugin_manager);
  }

  /*!
   * \return List with modules for external connections
   */
  inline util::tSimpleList< ::std::tr1::shared_ptr<tCreateExternalConnectionAction> >* GetExternalConnections()
  {
    return &(external_connections);
  }

  /*!
   * \return Plugins singleton instance
   */
  inline static tPlugins* GetInstance()
  {
    assert((instance != NULL));
    return instance.get();
  }

  /*!
   * \return List with plugins (do not modify!)
   */
  inline util::tSimpleList< ::std::tr1::shared_ptr<tPlugin> >* GetPlugins()
  {
    return &(plugins);
  }

  /*!
   * Register module that can be used as external connection (e.g. in GUI)
   *
   * \param action Action to be registered
   * \return Action to be registered (same as above)
   */
  inline ::std::tr1::shared_ptr<tCreateExternalConnectionAction> RegisterExternalConnection(::std::tr1::shared_ptr<tCreateExternalConnectionAction> action)
  {
    external_connections.Add(action);
    return action;
  }

  //
  //  /** PluginsListener */
  //  private final List<PluginsListener> pluginsListener = new ArrayList<PluginsListener>();
  //
  //  /**
  //   * Flag for indicating whether listeners should be notified after add operation
  //   * (good idea to deactivate and afterwards reactivate when adding a bunch of plugins)
  //   */
  //  private boolean notify = true;
  //
  //  /** List with a set of actions that can be used to create Modules/Groups that are contained in all loaded Plugins. */
  //  private final List<CreateModuleAction> pluginModules = new ArrayList<CreateModuleAction>();
  //  private final List<CreateModuleAction> pluginModulesU = Collections.unmodifiableList(pluginModules);
  //
  //  /** List with a set of actions that can be used to create Connection modules that are contained in all loaded Plugins. */
  //  private final List<CreateExternalConnectionAction> connectionModules = new ArrayList<CreateExternalConnectionAction>();
  //  private final List<CreateExternalConnectionAction> connectionModulesU = Collections.unmodifiableList(connectionModules);
  //
  //  /** Class loader for plugins */
  //  private PluginClassLoader classLoader;
  //
  //  /** List with Widget classes */
  //  private final List<Class<? extends Widget>> widgetClasses = new ArrayList<Class<? extends Widget>>();
  //  private final List<Class<? extends Widget>> widgetClassesU = Collections.unmodifiableList(widgetClasses);
  //
  //  /** GUI file codecs list */
  //  private final List<GUICodec> guiCodecs = new ArrayList<GUICodec>();
  //  private final List<GUICodec> guiCodecsU = Collections.unmodifiableList(guiCodecs);
  //
  //
  //  @SuppressWarnings("unchecked")
  //  private Plugins() {
  //
  //      // add widgets from widget package
  //      try {
  //          for (Class<?> c : Files.getPackageClasses(GUI_ROOT_CLASS, WIDGETPACKAGENAME)) {
  //              if (Widget.class.isAssignableFrom(c)) {
  //                  widgetClasses.add((Class<? extends Widget>)c);
  //                  JmcaguiXmlSerializer.getInstance().alias(c.getSimpleName(), c);
  //              }
  //          }
  //      } catch (Exception e) {
  //          e.printStackTrace();
  //      }
  //
  //      guiCodecs.add(new JGUI1Importer());
  //  }
  //

  /*!
   * Loads plugins
   */
  inline static void StaticInit()
  {
    instance = ::std::tr1::shared_ptr<tPlugins>(new tPlugins());
    instance->FindAndLoadPlugins();
  }

//
//  /**
//   * Add Plugin to set of Plugins
//   *
//   * \param pluginClass Fully qualified Plugin class the implements Plugin interface
//   * \param jar Jar File that contains ALL files that are necessary for the plugin (including libraries)
//   */
//  @SuppressWarnings("unchecked")
//  public synchronized void loadPlugin(Manifest mf) throws Exception {
//      String className = mf.getMainAttributes().getValue("Plugin-Class");
//      String dependenciesS = mf.getMainAttributes().getValue("Dependencies");
//      List<String> dependencies = new ArrayList<String>();
//      if (dependenciesS != null) {
//          dependencies.addAll(Arrays.asList(dependenciesS.split(",")));
//      }
//      Class<? extends Plugin> c;
//
//      if (!(RuntimeSettings.isDebugging() || RuntimeSettings.isRunningInApplet() > 0)) {
//
//          // Are dependencies met?
//          for (String dependency : dependencies) {
//              classLoader.loadClass(dependency);
//          }
//
//          // Classloader needed for plugins in separate .jars
//          c = (Class<? extends Plugin>)classLoader.loadClass(className);
//      } else {
//
//          // Are dependencies met?
//          for (String dependency : dependencies) {
//              Class.forName(dependency);
//          }
//
//          c = (Class<? extends Plugin>)Class.forName(className);
//      }
//
//      // Is Plugin?
//      if (!Plugin.class.isAssignableFrom(c)) {
//          throw new Exception(className + " is not a plugin class.");
//      }
//      System.out.println("Found plugin: " + className);
//
//      // ensure that plugin is loaded only once
//      for (int i = 0; i < plugins.size(); i++) {
//          if (plugins.get(i).getClass().getName().equals(c.getName())) {
//              plugins.remove(i);
//              break;
//          }
//      }
//
//      // add and initialize plugin
//      Plugin plugin = c.newInstance();
//      plugins.add(plugin);
//
//      // add modules
//      CreateModuleAction[] cmas = plugin.getPluginModules();
//      if (cmas != null) {
//          pluginModules.addAll(Arrays.asList(cmas));
//          for (CreateModuleAction cma : cmas) {
//              if (cma instanceof CreateExternalConnectionAction) {
//                  connectionModules.add((CreateExternalConnectionAction)cma);
//              }
//          }
//      }
//
//      // add data types
//      Class<PortData>[] dataTypes = plugin.getDataTypes();
//      if (dataTypes != null) {
//          DataTypeRegister.getInstance().add(dataTypes);
//      }
//
//      if (plugin instanceof GUIPlugin) {
//
//          // add widgets
//          Class<? extends Widget>[] w = ((GUIPlugin)plugin).getWidgets();
//          if (w != null) {
//              for (Class<? extends Widget> cl : w) {
//                  widgetClasses.add(cl);
//                  JmcaguiXmlSerializer.getInstance().alias(cl.getSimpleName(), cl);
//              }
//          }
//
//          // add codecs
//          GUICodec[] codecs = ((GUIPlugin)plugin).getGUICodecs();
//          if (c != null) {
//              guiCodecs.addAll(Arrays.asList(codecs));
//          }
//      }
//
//      if (notify) {
//          notifyPluginsListener();
//      }
//  }
//
//  /**
//   * Add Plugin to set of Plugins
//   *
//   * \param pluginClass Fully qualified Plugin class the implements Plugin interface
//   * \param jar Jar File that contains ALL files that are necessary for the plugin (including libraries)
//   */
//  //@SuppressWarnings("unchecked")
//  /*public static synchronized void addPlugin(String pluginClass, File jar) throws Exception {
//
//      // search for libraries in jar Path
//      System.out.println("add plugin: " + jar.toString());
//      List<URL> urls = new ArrayList<URL>();
//      for (File f : jar.getParentFile().listFiles()) {
//          if (f.getName().endsWith(".jar")) {
//              urls.add(f.toURI().toURL());
//              //System.out.println("add url: " + urls.get(urls.size() - 1).toString());
//          }
//      }
//
//      addPlugin(pluginClass, urls.toArray(new URL[0]));
//  }*/
//
//  /**
//   * Loads all plugins that are found in the application's root and sub directories.
//   * More precise: Root directory of the classpath the calling class is in.
//   *
//   * The file names need to have the following form:
//   *
//   * Jar:  <fully-qualified-classname>.jar
//   * Conf: <fully-qualified-classname>.conf
//   */
//  public synchronized void addPluginsFromApplicationDir() {
//
//      notify = false;
//
//      if (!(RuntimeSettings.isDebugging() || RuntimeSettings.isRunningInApplet() > 0)) {
//
//          List<URL> allJars = new ArrayList<URL>();
//          List<File> pluginMainJars = new ArrayList<File>();
//
//          // collect jars
//          File rootDir = RuntimeSettings.getRootDir();
//          for (File file : rootDir.listFiles()) {
//              if (file.isDirectory()) {
//                  // search for jars in dist directory
//                  String filename = file.getName();
//                  File jar = new File(file.getAbsolutePath() + File.separator + filename + ".jar");
//                  if (jar.exists()) {
//                      pluginMainJars.add(jar);
//                  }
//                  for (File f : Files.getAllFiles(file, new String[]{"jar"}, false, false)) {
//                      try {
//                          allJars.add(f.toURI().toURL());
//                      } catch (MalformedURLException e) {
//                          e.printStackTrace();
//                      }
//                  }
//              }
//          }
//
//          addToClassLoader(allJars);
//
//          // load plugins
//          for (File pluginJar : pluginMainJars) {
//              try {
//                  //String className = new JarFile(pluginJar).getManifest().getMainAttributes().getValue("Plugin-Class");
//                  //System.out.println("Found plugin " + filename + "; Plugin class: " + className);
//                  JarFile jf = new JarFile(pluginJar);
//                  loadPlugin(jf.getManifest());
//                  jf.close();
//              } catch (Exception e) {
//                  System.err.println("Error loading plugin: " + pluginJar.getName());
//                  e.printStackTrace();
//              }
//          }
//
//      } else {
//
//          // search for plugins in subdirectories
//          for (File folder : Files.getDir(Plugins.class).listFiles()) {
//              if (folder.isDirectory()) {
//                  // Plugin (?)
//                  try {
//                      for (File mf : folder.listFiles()) {
//                          if (mf.getName().endsWith(".mf")) {
//                              //String filename = folder.getName();
//                              //String className = new Manifest(new FileInputStream(mf)).getMainAttributes().getValue("Plugin-Class");
//                              //System.out.println("Found plugin " + filename + "; Plugin class: " + className);
//                              InputStream is = new FileInputStream(mf);
//                              loadPlugin(new Manifest(is));
//                              is.close();
//                              break;
//                          }
//                      }
//                  } catch (Exception e) {
//                      System.err.println("Error loading plugin: " + folder.getName());
//                      e.printStackTrace();
//                  }
//              }
//          }
//      }
//
//      notify = true;
//      notifyPluginsListener();
//
//  }
//
//  /**
//   * Add jar files to class loader. Init class loader if not yet done.
//   *
//   * \param jars Jar files to add
//   */
//  private synchronized void addToClassLoader(List<URL> jars) {
//      if (classLoader == null) {
//          try {
//              classLoader = new PluginClassLoader(jars.toArray(new URL[0]));
//          } catch (Exception e) {
//              e.printStackTrace();
//          }
//      } else {
//          for (URL url : jars) {
//              classLoader.addURL(url);
//          }
//      }
//  }
//
//
//  /**
//   * Returns all data types that a plugin provides
//   *
//   * \return Classes
//   */
//  /*public List<Class<PortData>> getPluginClasses(Class<PortData> t) {
//      List<Class<PortData>> result = new ArrayList<Class<PortData>>();
//      for (Plugin p : plugins) {
//          Class<PortData>[] pluginResult = p.getDataTypes();
//          if (pluginResult != null) {
//              result.addAll(Arrays.asList(pluginResult));
//          }
//      }
//      return result;
//  }*/
//
//  /**
//   * \return Returns a set of actions that can be used to create Modules/Groups that
//   * are contained in all loaded Plugins.
//   */
//  public List<CreateModuleAction> getPluginModules() {
//      return pluginModulesU;
//  }
//
//  /**
//   * \return Returns a set of actions that can be used to create Modules/Groups that
//   * are contained in all loaded Plugins.
//   */
//  public List<CreateExternalConnectionAction> getExternalConnections() {
//      return connectionModulesU;
//  }
//
//  /**
//   * \return Returns all known widget classes
//   */
//  public List<Class<? extends Widget>> getWidgets() {
//      return widgetClassesU;
//  }
//
//  /**
//   * \return Returns all known widget classes
//   */
//  public List<GUICodec> getGUICodecs() {
//      return guiCodecsU;
//  }
//
//
//  /**
//   * Create Module/Group provided by plugin.
//   *
//   * \param name Name of CreateModuleAction
//   * \param addTo Runtime Element to add module/group to
//   * \param Object Parameters
//   */
//  public FrameworkElement createModule(String name, Group addTo, Object... params) throws Exception {
//      for (CreateModuleAction cma : pluginModules) {
//          if (cma.toString().equals(name)) {
//              FrameworkElement re = cma.createModule(params);
//              if (addTo == null) {
//                  addTo = RuntimeEnvironment.getInstance();
//              }
//              if (re instanceof Group) {
//                  addTo.addGroup((Group)re);
//              } else if (re instanceof Module) {
//                  addTo.addModule((Module)re);
//              } else {
//                  throw new Exception("Did not create module nor group");
//              }
//              return re;
//          }
//      }
//      throw new RuntimeException("Module " + name + " not found");
//  }
//
//  /**
//   * Create External connection provided by plugin.
//   *
//   * \param name Name of External Connection class
//   * \param addTo Runtime Element to add module/group to
//   * \param address Address to connection
//   */
//  public ExternalConnection createExternalConnection(String name, Group addTo, String address) throws Exception {
//      for (CreateExternalConnectionAction ceca : connectionModules) {
//          if (ceca.toString().equalsIgnoreCase(name)) {
//              ExternalConnection re = ceca.createModule(address);
//              if (addTo != null) {
//                  addTo.addModule(re);
//              } else {
//                  RuntimeEnvironment.getInstance().addModule(re);
//              }
//              return re;
//          }
//      }
//      throw new RuntimeException("Module " + name + " not found");
//  }
//
//  /**
//   * Load class that is provided by a plugin.
//   * This method is only meant for testing
//   *
//   * \param class1 Class name
//   */
//  @Deprecated
//  public Class<?> loadClass(String class1) {
//      for (Plugin p : plugins) {
//          try {
//              return p.getClass().getClassLoader().loadClass(class1);
//          } catch (ClassNotFoundException e) {
//              // try next
//          }
//      }
//      return null;
//  }
//
//  /**
//   * \param pl PluginsListener
//   */
//  public void addPluginsListener(PluginsListener pl) {
//      if (!pluginsListener.contains(pl)) {
//          pluginsListener.add(pl);
//      }
//  }
//
//  /**
//   * Notifiy PluginsListener of change
//   */
//  public void notifyPluginsListener() {
//      for (PluginsListener pl : pluginsListener) {
//          pl.pluginsChanged();
//      }
//  }
//  /**
//   * \return Returns HTTP resources provided by all loaded Plugins
//   */
//  public List<HTTPResource> getHTTPResources() {
//      List<HTTPResource> result = new ArrayList<HTTPResource>();
//      for (Plugin p : plugins) {
//          HTTPResource[] pluginResult = p.getHTTPResources();
//          if (pluginResult != null) {
//              result.addAll(Arrays.asList(pluginResult));
//          }
//      }
//      return result;
//  }
//
//  @Override
//  public String getHTTPDirectory() {
//      return "/Plugins";
//  }
//
//  @Override
//  public TreeNode getHTTPDirectoryStructure() {
//      /*DefaultMutableTreeNode result = new DefaultMutableTreeNode("Plugins");
//      for (int i = 0; i < plugins.size(); i++) {
//          Plugin p = plugins.get(i);
//          String temp = p.getHTTPResources()[0].getHTTPDirectory();
//          result.add(new DefaultMutableTreeNode(temp.substring(9, temp.length() - 1)));
//      }
//      return result;*/
//      return null;
//  }
//
//  @Override
//  public HTTPResponse serveHTTP(HttpServletRequest request) {
//      HTMLResponse response = new HTMLResponse("Plugins");
//      HTMLTable table = new HTMLTable("Loaded Plugins", "Name", "Jar");
//      for (Plugin p : plugins) {
//          String jar = ((URLClassLoader)p.getClass().getClassLoader()).getURLs()[0].toString();
//          jar = jar.substring(jar.indexOf(File.separator) + 1);
//          table.addRow(p.getClass().getName(), RuntimeSettings.isDebugging() ? "N/A" : jar);
//      }
//      response.getBody().add(table);
//      return response;
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TPLUGINS_H
