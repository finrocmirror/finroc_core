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

#ifndef CORE__SETTINGS__TSETTING_H
#define CORE__SETTINGS__TSETTING_H

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base for a simple setting
 */
class tSetting : public util::tUncopyableObject
{
  friend class tSettings;
private:

  /*! Publish setting as port? */
  bool publish_as_aPort;

protected:

  /*! description of setting */
  util::tString description;

  /*! Port - in case setting is published as port */
  tAbstractPort* port;

  /*!
   * \param description description of setting
   * \param cPublish setting as port?
   */
  tSetting(const util::tString& description_, bool publish);

public:

  /*!
   * Creates port for setting as child of specified framework element
   *
   * \param parent Framework element
   */
  virtual tAbstractPort* CreatePort(tFrameworkElement* parent) = 0;

  /*!
   * \return description of setting
   */
  inline util::tString GetDescription() const
  {
    return description;
  }

  /*!
   * \return Port - in case setting is published as port
   */
  inline tAbstractPort* GetPort()
  {
    return port;
  }

  /*!
   * \return Publish setting as port?
   */
  inline bool PublishAsPort() const
  {
    return publish_as_aPort;
  }

};

} // namespace finroc
} // namespace core

#include "core/settings/tNumberSetting.h"

#endif // CORE__SETTINGS__TSETTING_H
