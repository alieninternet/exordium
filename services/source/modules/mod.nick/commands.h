/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2003 Exordium Development Team
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
 */

#ifndef _SOURCE_MODULES_MOD_NICK_COMMANDS_H_
# define _SOURCE_MODULES_MOD_NICK_COMMANDS_H_

# include "language.h"
# include "nick.h"

namespace Exordium {
   namespace NickModule {
      namespace Commands {
         // The command table
         struct commandTable_type {
	    const char* const command;
	    NICK_FUNC((Module::* const function));
	    const Kine::Languages::tagID_type* const help;
	 } extern const commandTable[];
      }; // namespace Commands
   }; // namespace NickModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_MOD_NICK_COMMANDS_H_

