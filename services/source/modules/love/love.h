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
	void x(const Kine::String& origin, const Kine::StringTokens& tokens)


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
      const char* command;			// Command name
      const unsigned short minParams;		// The min. number of params
      const unsigned short maxParams;		// Maximum parameters
      const static unsigned short MAX_PARAMS_UNLIMITED = 65535; // (any for ^^)
      const handler_type Love::* const handler;	// The function (handler)
      const int padding;			// For future use..
   } static const commandTable[];

   // Where is services?
   Exordium::Services& services;
   
   // Who are we?!
   const String myName;
   
   // Our wonderful command handlers
   handler_type handleCOMMANDS;
   handler_type handleTEST;

   // How to send private-messages (stepping-stone)
   void sendMessage(const String& recipient, const String& message)
     {
	// This name should not be hard-coded - icky
	// 
	// It isn't anymore! (ner :( ) - init_func can now take another
	// parm, being a String containing your name. (as defined by
	// loadModule)
	// 
	// Oh wonderful James :) I suppose this comment is now redundant? :(
	services.serviceNotice(message, myName, recipient);
     };
   
 public:
   // Our constructor
   Love(Exordium::Services& s, const String& mn);

   // Our destructor
   ~Love(void) 
     {};
   
   // Parser for incoming stuff sent on a channel
   void parseLine(const Kine::String& line, const Kine::String& origin,
		  const Kine::String& channel)
     { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaehHAEhaE!!! */ };
   
   // Parser for incoming stuff sent directly to us
   void parseLine(const Kine::String& line, const Kine::String& origin);
};

#endif // __LOVE_H_
