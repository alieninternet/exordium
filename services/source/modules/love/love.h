/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __LOVE_H_
# define __LOVE_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <kineircd/str.h>


# define LOVE_FUNC(x) \
	void x(const Kine::String &origin, const Kine::StringTokens &tokens)


// The class of lurrve
class Love : public Exordium::Service {
 private:
   // Our convenient type-definition of our handler functions
   typedef LOVE_FUNC(handler_type);
   
   /* Each command gets a little descriptor with the following parameters.
    * Note that in the future the padding field will be used, but for now
    * it is there to make the structure even (4x4 bytes) as processors like
    * to chew on nice neat evenly spaces arrays using some magical multiple
    * of its natural word size (like 4).. Aren't we nice to succumb to its
    * wishes? Well okay, I thought we were at least.. Stop looking at me like
    * that.. Stop it! Stop it..... okay, OKAY - fine.. I'll go then. :(
    */
   struct commandTable_type {
      const char *command;		// Command name
      const unsigned short minParams;	// The minimum number of parameters
      const unsigned short maxParams;	// Maximum parameters (Any = below)
      const static unsigned short MAX_PARAMS_UNLIMITED = 65535;
      const handler_type Love::*handler;// The function (handler) to call
      const int blahblahblahblahblah;	// For future use..
   } static const commandTable[];
   
   // Our wonderful command handlers
   handler_type handleTEST;

   // How to send private-messages (stepping-stone)
   void sendMessage(const String &recipient, const String &message)
     {
	// This name should not be hard-coded - icky
	Exordium::Services::serviceNotice(message, "Love", recipient);
     };

   
 public:
   // Our constructor
   Love(void)
     {
	std::cout << 
	  "Love::Love() - Why hello there, I'm a loving instance :) :)" << 
	  std::endl;
     };

   // Our destructor
   ~Love(void) 
     { 
	std::cout << 
	  "Love::~Love() - Share the love, don't trample on it :(" << 
	  std::endl;
     };
   
   // Parser for incoming stuff sent on a channel
   void parseLine(const Kine::String &line, const Kine::String &origin,
		  const Kine::String &channel)
     { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaehHAEhaE!!! */ };
   
   // Parser for incoming stuff sent directly to us
   void parseLine(const Kine::String &line, const Kine::String &origin);
};

#endif // __LOVE_H_
