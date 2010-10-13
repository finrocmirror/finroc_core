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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__SETTINGS__TSETTINGS_H
#define CORE__SETTINGS__TSETTINGS_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/datatype/tBounds.h"

#include "core/settings/tNumberSetting.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tSetting;
class tUnit;

/*!
 * \author Max Reichardt
 *
 * This class contains a set of parameters/settings that can be
 * conveniently managed/accessed/created.
 *
 * The parameters may be exposed to the outside as input ports.
 * The parameters can be loaded from a config file.
 *
 * The parameters can have (safe) static declarations.
 * Initialization of ports etc. will start as soon as init() is
 * called (this should not be done from static initializations)
 */
class tSettings : public util::tUncopyableObject
{
private:

  /*! Prefix of keys in config file */
  util::tString config_prefix;

  /*! Settings description (port will be called like this) */
  util::tString description;

  /*! All defined settings */
  util::tSimpleList<tSetting*> settings;

  /*! Have settings been initialized? */
  bool initialized;

  /*! Is this a static settings instance? */
  bool is_static;

protected:

  /*! Framework element representing settings - will be initialized deferred in C++ */
  tFrameworkElement* port_root;

public:

  /*!
   * \param description Settings description (port will be called like this)
   * \param config_prefix Prefix of keys in config file
   * \param is_static Is this a static settings instance?
   */
  tSettings(const util::tString& description_, const util::tString& config_prefix_, bool is_static_);

  /*!
   * \param description Description/Key of setting
   * \param default_val Default value
   * \param publish_as_port Publish setting as port?
   * \return Created bool setting
   */
  tBoolSetting* Add(const util::tString& description_, bool default_val, bool publish_as_port);

  /*!
   * \param description Description/Key of setting
   * \param default_val Default value
   * \param publish_as_port Publish setting as port?
   * \param cUnit unit
   * \param bounds Bounds for values
   * \return Created int setting
   */
  tIntSetting* Add(const util::tString& description_, int default_val, bool publish_as_port, tUnit* unit = &tUnit::cNO_UNIT, tBounds bounds = tBounds());

  /*!
   * \param description Description/Key of setting
   * \param default_val Default value
   * \param publish_as_port Publish setting as port?
   * \param cUnit unit
   * \param bounds Bounds for values
   * \return Created int setting
   */
  tLongSetting* Add(const util::tString& description_, int64 default_val, bool publish_as_port, tUnit* unit = &tUnit::cNO_UNIT, tBounds bounds = tBounds());

  /*!
   * \param description Description/Key of setting
   * \param default_val Default value
   * \param publish_as_port Publish setting as port?
   * \param cUnit unit
   * \param bound Bounds for values
   * \return Created int setting
   */
  tDoubleSetting* Add(const util::tString& description_, double default_val, bool publish_as_port, tUnit* unit = &tUnit::cNO_UNIT, tBounds bounds = tBounds());

  virtual ~tSettings();

  /*!
   * Add settings node to specified FrameworkElement
   *
   * \param parent Settings instance to add these settings to
   */
  inline void Init(tSettings* parent)
  {
    Init(parent->port_root);
  }

  /*!
   * Add settings node to specified FrameworkElement
   *
   * \param parent FrameworkElement to add settings node to
   */
  void Init(tFrameworkElement* parent);

};

} // namespace finroc
} // namespace core

#endif // CORE__SETTINGS__TSETTINGS_H
