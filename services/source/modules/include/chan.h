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
 */

#ifndef __CHAN_H_
#define __CHAN_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include "exordium/user.h"

#include <kineircd/str.h>

#define CHAN_FUNC(x)           x(Exordium::User& origin, LibAIS::StringTokens &tokens, LibAIS::String &chan)


class Chan : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void CHAN_FUNC ((Chan::* const function));
  };
  static struct functionTableStruct const functionTable[];
   
  void sendMessage(const LibAIS::String &message, const LibAIS::String &to)
	{
		services.serviceNotice(message,myName,to);
	}
public:
   Chan(Exordium::Services& s, const LibAIS::String &mn)
     : Exordium::Service(s, mn)
	{
	};
  ~Chan(void)
	{
	};
   void parseLine (LibAIS::StringTokens& line, Exordium::User& origin);
   void parseLine (LibAIS::StringTokens& line, Exordium::User& origin,LibAIS::String const &);
private:
   void CHAN_FUNC (parseHELP);
   void CHAN_FUNC (parseACCESS);
   void CHAN_FUNC (parseKICK);
   void CHAN_FUNC (parseOP);
   void CHAN_FUNC (parseVOICE);
   void CHAN_FUNC (parseREGISTER);
   void CHAN_FUNC (parseBAN);
   void CHAN_FUNC (parseDEOP);
   void CHAN_FUNC (parseTOPIC);
   void CHAN_FUNC (parseDEVOICE);
   void CHAN_FUNC (parseADDUSER);
   void CHAN_FUNC (parseINFO);
   void CHAN_FUNC (parseLISTBAN);
void CHAN_FUNC (parseSET);
void CHAN_FUNC (parseSEEN);
};




#endif
