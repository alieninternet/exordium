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
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <iostream>
#include "logger.h"

using namespace Exordium;


/* logLine - Log a string of text
 * Original 28/03/2002 pickle
 */
void Logger::logLine(const std::string& str,
		     const Kine::Logger::Mask::type mask)
{
   std::cout << "Database logger: The mask is " << mask <<
     " and the string is:\n'" << str << "'" << std::endl;
}
