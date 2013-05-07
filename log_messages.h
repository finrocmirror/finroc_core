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
/*!\file    core/log_messages.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief
 *
 * rrlib_logging log messages customized for Finroc.
 * In any Finroc-dependent code, these messages should be used instead of the RRLIB_LOG_PRINT* messages.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__log_messages_h__
#define __core__log_messages_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/logging/messages.h"

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

/*!
 * short macro for code location (for convenience)
 */
#define CODE_LOCATION_MACRO \
  __FUNCTION__, __FILE__, __LINE__

/*! Macro to print messages using stream semantics
 *
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_LOG_PRINT(level, args...) \
  RRLIB_LOG_PRINT(level, args) \
   
/*! Macro to print messages to explicitly specified domain using stream semantics
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_LOG_PRINT_TO(domain, level, args...) \
  RRLIB_LOG_PRINT_TO(domain, level, args) \
   
/*! Macro to print messages using stream semantics from static context
 *
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_LOG_PRINT_STATIC(level, args...) \
  RRLIB_LOG_PRINT_STATIC(level, args) \
   
/*! Macro to print messages to explicitly specified domain using stream semantics from static context
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_LOG_PRINT_STATIC_TO(domain, level, args...) \
  RRLIB_LOG_PRINT_STATIC_TO(domain, level, args) \
   
/*! Macro to print messages using stream semantics
 *  Debug messages will only be output if ShowDebugMessages() returns true.
 *  (all 'tFrameworkElement's have such a method)
 *
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_MODULE_LOG_PRINT(level, args...) \
  if (__EXPAND_LEVEL__(level) <= __EXPAND_LEVEL__(WARNING) || ShowObjectDebugMessages()) \
  { \
    FINROC_LOG_PRINT(level, args) \
  } \
   
/*! Macro to print messages to explicitly specified domain using stream semantics
 *  Debug messages will only be output if ShowDebugMessages() returns true.
 *  (all 'tFrameworkElement's have such a method)
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The data to be put into the underlying stream
 */
#define FINROC_MODULE_LOG_PRINT_TO(domain, level, args...) \
  if (__EXPAND_LEVEL__(level) <= __EXPAND_LEVEL__(WARNING) || ShowObjectDebugMessages()) \
  { \
    FINROC_LOG_PRINT_TO(domain, level, args) \
  } \
   
/*! Macro to print messages using printf semantics
 *
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_LOG_PRINTF(level, args...) \
  RRLIB_LOG_PRINTF(level, args) \
   
/*! Macro to print messages to explicitly specified domain using printf semantics
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_LOG_PRINTF_TO(domain, level, args...) \
  RRLIB_LOG_PRINTF_TO(domain, level, args) \
   
/*! Macro to print messages using printf semantics from static context
 *
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_LOG_PRINTF_STATIC(level, args...) \
  RRLIB_LOG_PRINTF_STATIC(level, args) \
   
/*! Macro to print messages to explicitly specified domain using printf semantics from static context
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_LOG_PRINTF_STATIC_TO(domain, level, args...) \
  RRLIB_LOG_PRINTF_STATIC_TO(domain, level, args) \
   
/*! Macro to print messages using printf semantics
 *  Debug messages will only be output if ShowDebugMessages() returns true.
 *  (all 'tFrameworkElement's have such a method)
 *
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_MODULE_LOG_PRINTF(level, args...) \
  if (__EXPAND_LEVEL__(level) <= __EXPAND_LEVEL__(WARNING) || ShowObjectDebugMessages()) \
  { \
    FINROC_LOG_PRINTF(level, args) \
  } \
   
/*! Macro to print messages to explicitly specified domain using printf semantics
 *  Debug messages will only be output if ShowDebugMessages() returns true.
 *  (all 'tFrameworkElement's have such a method)
 *
 * \param domain   The domain the message should be printed to
 * \param level    The level of the message
 * \param args     The format string for printf and the optional arguments to be printed
 */
#define FINROC_MODULE_LOG_PRINTF_TO(domain, level, args...) \
  if (__EXPAND_LEVEL__(level) <= __EXPAND_LEVEL__(WARNING) || ShowObjectDebugMessages()) \
  { \
    FINROC_LOG_PRINTF_TO(domain, level, args) \
  } \
   
//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
