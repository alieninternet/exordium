/* Class for Service Modules */

#ifndef SERVICE_H
#define SERVICE_H
#include "kineircd/str.h"
#include <map>
#include <dlfcn.h>

namespace Exordium {

class Service
	{
	public:
		Service() {};
		virtual ~Service() {};
		virtual void parseLine(String const &, String const &) =0;
		virtual void parseLine(String const &, String const &, String const &) =0;
	};


class Core {
 private:
   struct ServiceModule {
      Service *service;
      void *handle;

      ServiceModule(Service *s, void *h)
        : service(s), handle(h)
          {};
   };
   typedef std::map <String, ServiceModule *> modules_type;
   modules_type serviceModules;

 public:
   Core(void) {
      // I'm anal, so shoot me.
      serviceModules.clear();
   };

   void addModule(String const &name, Service &s, void *h) {
      // Just add it - this will overwrite anything already there
      serviceModules[name.IRCtoLower()] = new ServiceModule(&s,h);
   };

   void delModule(String const &name) {
      ServiceModule *sm = serviceModules[name.IRCtoLower()];
      if (sm != 0) {
         dlclose(sm->handle);
         dlclose(sm->handle);
         delete sm;
	 serviceModules.erase(name.IRCtoLower());
	 return;
      }
      std::cout << "Umm... i couldn't find " << name << std::endl;
   };

   bool exists(String const &name)
	{
		ServiceModule *sm = serviceModules[name.IRCtoLower()];
		if ( sm == 0 )
		{
			serviceModules.erase(name.IRCtoLower());
			return false;
		}
		return true;
	}
   // Throw a line at the appropriate service
   void throwLine(String const &name, String const &line, String const &req) {
      ServiceModule *sm = serviceModules[name.IRCtoLower()];
      if (sm == 0) {
         // Give up.. delete what we just made and go bye byes
         serviceModules.erase(name.IRCtoLower());
         return;
      }
      Service *serv = sm->service;
      serv->parseLine(line,req);
      return;
   };

   void throwLine(String const &name, String const &line, String const &req, String const &ch) {
      // Find it...
      ServiceModule *sm = serviceModules[name.IRCtoLower()];

      // It will be null if we did not find it..
      if (sm == 0) {
         // Give up.. delete what we just made and go bye byes
         serviceModules.erase(name.IRCtoLower());
         return;
      }
      Service *serv = sm->service;
      // Otherwise we must have it.. send something to the service
      serv->parseLine(line,req,ch);
      return;
   };


   // Dump a list of modules
   String dumpModules(void) {
      std::cout << "Modules loaded: ";
      String tmp = "";
      for (modules_type::iterator it = serviceModules.begin();
           it != serviceModules.end(); it++) {
         // Output the key..
         std::cout << (*it).first << ' ';
	 tmp = String(tmp)+" "+(*it).first;
      }
      std::cout << std::endl;
      return tmp;
   }
};

};

#endif

