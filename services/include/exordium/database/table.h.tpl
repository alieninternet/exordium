[+ AutoGen5 template h +]
/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
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
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
 *
[+(dne " * ")+]
 */

#ifndef _INCLUDE_EXORDIUM_DATABASE_TABLE_H_
# define _INCLUDE_EXORDIUM_DATABASE_TABLE_H_ 1

namespace Exordium {
   /* This describes a table in the database in a somewhat generic form. Any
    * modules requiring a table in the database should query the database
    * engine with a reference to a static copy of this table structure
    * describing the table they need to use prior to using it - such as when
    * Exordium triggers a module to start. If the table does not exist or is
    * incomplete, the database engine may be able to create it or update it as
    * necessary.
    */
   struct DatabaseTable {
      /* The name of the table, used to reference it. This should be considered
       * case sensitive
       */
      const char* const name;
      
      /* An array of the fields use within this table. Terminate this list
       * with a field with a null name.
       */
      struct Field {
	 // The name used to reference this field. Consider this case sensitive
	 const char* const name;
	 
	 /* The type of the field. Many field types are given here, but not all
	  * may be available using the user's specified database engine. Each
	  * database engine will compensate in that event so you are still
	  * guaranteed types as specified in the comments. For example, you
	  * may have a table using a variety of integer sizes but the selected
	  * engine may only have one form of integer. That single form will be
	  * used.
	  *
	  * Note that the engine's table affirmation routines may also use the
	  * width field to optimise the type.
	  */
	 struct Type { // <=- should be namespace :(
	    enum type {[+ FOR fieldtypes +]
	       [+ (sprintf "%-27s // %s"
	           (sprintf "%s," (get "name"))
		   (get "comment"))
		+][+ ENDFOR fieldtypes +]
	    };
	 };
	 const Type::type type;
	 
	 // The width of the field
	 const unsigned int width;
	 
	 // The default value of the field. Make this a null pointer for 'NULL'
	 const char* const defaultValue;
	 
	 // Flags to tighten the field's definition
	 struct Flags { // <=- should be namespace too :(
	    enum {[+ (define bitvalue 1) +][+ FOR fieldflags +]
	       [+ (define bitvalue (* bitvalue 2))
	          (sprintf "%-13s = 0x%08X, // %s"
		   (get "name")
		   (/ bitvalue 2)
		   (get "comment"))
                +][+ ENDFOR fieldtypes +]
	    };
	 };
	 const unsigned int flags;
      } const fields[];
   }; // struct DatabaseTable
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_DATABASE_TABLE_H_
