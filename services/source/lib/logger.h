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

#ifndef _INCLUDE_EXORDIUM_LOGGER_H_
# define _INCLUDE_EXORDIUM_LOGGER_H_ 1

# include <kineircd/logger.h>

namespace Exordium {
   class Logger : public Kine::Logger {
    private:
      // Log a string of text
      void logLine(const std::string& str,
		   const Kine::Logger::Mask::type mask);
      
    public:
      // Constructor
      Logger(Kine::Logger::Mask::lazy_type mask)
	: Kine::Logger(mask)
	{};
      
      // Destructor
      ~Logger(void)
	{};
      
      // Is the log ok? For now, always presume yes (i.e. the db engine is ok)
      bool ok(void) const
	{ return true; };
   }; // class Logger
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_LOGGER_H_

