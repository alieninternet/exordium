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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <aisutil/string/string.h>
#include <aisutil/string/tokens.h>

#include "exordium/service.h"


using namespace Exordium;
using AISutil::String;
using AISutil::StringTokens;


/* parseLine - Handle a message/query sent directly to us from a user
 * Original 08/06/2003 pickle
 */
void Service::parseLine(Kine::Client& origin,
			const StringTokens& line,
			const bool isSafe)
{
   // temporary until the User:: thing is resolved
   (void)origin.sendMessage(*this, line);
}


/* parseLine - Handle a message sent to a channel we're on
 * Original 08/06/2003 pickle
 */
void Service::parseLine(Kine::Client& origin,
			Kine::Channel& destination,
			const StringTokens& line)
{
   // temporary until the User:: thing is resolved
   (void)origin.sendMessage(*this, line);
}


/* parseLine - Handle the reception of a service query to this service
 * Original 08/06/2003 pickle
 */
const Kine::Error::error_type
  Service::sendQuery(Kine::Client& from,
		     const std::string& message,
		     const Kine::Receiver::Directivity directivity)
{
   /* Check if this client is a naughty flooder here?
    * 
    * Since the original code from parser.cpp was back when User:: presumed
    * that everyone was a registered user, I'm not sure how this should be
    * done, so I'm guessing it'll need to be rewritten.. James??
    */
   
   // Convert the message into a tokenised form
   StringTokens tokens(message);
   
   // Pass the tokenised message onto the parser
   parseLine(from, tokens, directivity.server);
   
   // No problem :)
   return Kine::Error::NO_ERROR;
}


/* parseLine - Handle the reception of a private message send to this service
 * Original 08/06/2003 pickle
 */
const Kine::Error::error_type
  Service::sendMessage(Kine::Sender& from,
		       const std::string& message,
		       const Kine::Receiver::Directivity directivity)
{
   // Try to convert this over to a client..
   Kine::Client* const client = dynamic_cast<Kine::Client* const>(&from);

   // Make sure this *is* a client
   if (client != 0) {
      // Check if this is a CTCP (this should be moved elsewhere, IMHO -simon)
      if ((message[0] == '\001') &&
	  (message[message.length() - 1] == '\001')) {
	 // Break the message down
	 StringTokens CTCPtokens(message.substr(1, message.length() - 2));
	 String CTCPcommand = CTCPtokens.nextToken().toUpper();
	 
	 // Check what kind of CTCP this is
	 if (CTCPcommand == "PING") {
	    client->sendNotice(*this,
			       "\001PING " + CTCPtokens.rest() + "\001");
	 } else if (CTCPcommand == "VERSION") {
	    client->sendNotice(*this,
			       "\001VERSION Exordium Network Services "
			       "http://exordium.sourceforge.net/\001");
	 }
	 
	 return Kine::Error::NO_ERROR;
      }
      
      // It's not a CTCP message, throw it over to the SQUERY handler
      return sendQuery(*client, message, directivity);
   }
   
   // It's not a client, we don't want to deal with it..
   return Kine::Error::UNSUPPORTED_BY_ENTITY;
}
