#ifndef __SERV_H_
#define __SERV_H_

#include "kineircd/str.h"

# define SERV_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {

class Serv : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void SERV_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
	Serv(void)
		: Service()
	{
		std::cout << "Serv New()" << std::endl;
	};
	~Serv(void)
	{
		std::cout << "Serv Deadeded" << std::endl;
	};
   void parseLine (String const &, String const &);
   void parseLine (String const &, String const &, String const &);
private:
static void SERV_FUNC (parseCLIST);
static void SERV_FUNC (parseDELNICK);
static void SERV_FUNC (parseELIST);
static void SERV_FUNC (parseNLIST);
static void SERV_FUNC (parseHELPON);
static void SERV_FUNC (parseHELP);
static void SERV_FUNC (parseUSER);
static void SERV_FUNC (parseCHAN);
static void SERV_FUNC (parseDIE);
static void SERV_FUNC (parseRAW);
};
};

#endif
