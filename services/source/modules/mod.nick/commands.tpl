[+ AutoGen5 template cpp h +]
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
 *
 *
[+(dne " * ")+]
 */
[+CASE (suffix)+][+== cpp+]

#include "commands.h"

using namespace Exordium::NickModule;
[+DEFINE output-block+]     {
	"[+
   (string-downcase 
      (get "command"))
 +]",
	  &Module::parse[+
   (string-upcase 
      (get "commandName"))
 +],
          [+IF
   (exist? "subcommand")
 +]&[+
   (string-downcase
      (get "commandName"))
 +]CommandTable[+ELSE+]0[+ENDIF+],
	  &Language::tagMap[Language::nick_HELP_[+
   (string-upcase
      (get "commandName"))
 +]].tagID
     },[+ENDDEF+]
const Commands::commandTable_type Commands::commandTable = {[+FOR command+]
[+output-block
   commandName=(get "name")
   command=(get "name")
 +][+FOR alias+]
[+output-block
   commandName=(get "name")
   command=(get "alias")
 +][+ENDFOR+][+ENDFOR+]
     { 0, 0, 0, 0 }
};
[+DEFINE output-sub-block+]     {
	"[+
   (string-downcase 
      (get "command"))
 +]",
	  &Language::tagMap[Language::nick_HELP_[+
   (string-upcase
      (get "prefix"))
 +]_[+
   (string-upcase
      (get "commandName"))
 +]].tagID
     },[+ENDDEF+][+DEFINE output-subs+][+FOR subcommand+]
[+output-sub-block
   command=(get "name")
   commandName=(get "name")
 +][+FOR alias+]
[+output-sub-block
   command=(get "alias")
   commandName=(get "name")
 +][+ENDFOR+][+ENDFOR+][+ENDDEF+]
[+FOR command+][+IF
   (exist? ".subcommand")
 +]const Commands::subcommandTable_type Commands::[+
   (string-downcase
      (get "name"))
 +]CommandTable = {[+output-subs 
   prefix=(get "name")
 +]
     { 0, 0 }
};
[+ENDIF+][+ENDFOR+]
[+== h+]
#ifndef _SOURCE_MODULES_MOD_NICK_COMMANDS_H_
# define _SOURCE_MODULES_MOD_NICK_COMMANDS_H_

# include "language.h"
# include "nick.h"

namespace Exordium {
   namespace NickModule {
      namespace Commands {
      	 // Subcommand info
	 struct subcommand_type {
	    const char* const command;
	    const Kine::Languages::tagID_type* const help;
	 };
      	 typedef subcommand_type subcommandTable_type[];
      
         // The command table
         struct command_type {
	    const char* const command;
	    NICK_FUNC((Module::* const function));
	    const subcommandTable_type* subcommands;
	    const Kine::Languages::tagID_type* const help;
	 };
	 typedef command_type commandTable_type[];

	 // Command tables
	 extern const commandTable_type commandTable;[+FOR command+][+IF
   (exist? ".subcommand")
 +]
         extern const subcommandTable_type [+
   (string-downcase
      (get "name"))
 +]CommandTable;[+ENDIF+][+ENDFOR+]
      }; // namespace Commands
   }; // namespace NickModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_MOD_NICK_COMMANDS_H_
[+ESAC+]
