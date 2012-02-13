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

#ifndef core__tRuntimeSettings_h__
#define core__tRuntimeSettings_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Contains global settings of runtime environment.
 *
 * Plugins should add global parameters to this tRuntimeSettings framework element.
 *
 * staticInit() should be called after runtime and data types have been initialized.
 */
class tRuntimeSettings : public tFrameworkElement
{
private:

  /*! Singleton Instance */
  static tRuntimeSettings* inst;

public:

  static const int cEDGE_LIST_DEFAULT_SIZE = 0;
  static const int cEDGE_LIST_SIZE_INCREASE_FACTOR = 2;

  /*! Collect edge statistics ? */
  static const bool cCOLLECT_EDGE_STATISTICS = false;

protected:

  tRuntimeSettings();

public:

  /*!
   * \return Singleton instance
   */
  static tRuntimeSettings* GetInstance();

  /*! Completes initialization */
  static void StaticInit();

};

} // namespace finroc
} // namespace core

#endif // core__tRuntimeSettings_h__
