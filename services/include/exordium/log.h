/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
 *
 * This file is a part of Exordium.
 * 
 * Exordium is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Exordium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Exordium; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _INCLUDE_EXORDIUM_LOG_H_
# define _INCLUDE_EXORDIUM_LOG_H_ 1

# include <kineircd/logger.h>

namespace Exordium {
   namespace Log {
      /* This is sneaky. In order to fake our own interface to a logger, and
       * to simplify things, we minimise the mask types down to a handful of
       * potentials, but their values are actually mapped to fixed Kine masks.
       * When our logging function is called, these masks are type-casted to
       * the KineIRCd logger's mask type for seamless interaction. Exordium and
       * Exordium modules should not type-cast to KineIRCd masks, since this
       * fixed set is here for a reason.
       */
      enum mask_type {
	 Debug		= Kine::Logger::Mask::Debug,
	 Informative	= Kine::Logger::Mask::Housekeeping,
	 Warning	= Kine::Logger::Mask::Warning,
	 Error		= Kine::Logger::Mask::Error_Local,
	 Fatality	= Kine::Logger::Mask::Fatal_Error
      };
   }; // namespace Log
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_LOG_H_
