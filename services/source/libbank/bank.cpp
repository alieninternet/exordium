/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#include <sstream>

#include "bank.h"

using AISutil::String;

const int Exordium::Bank::getBalance(const Exordium::User& origin)
{
  int curBalance = 0;
  if(services.getDatabase().dbSelect("balance", "bank", "nickid='"+String::convert(origin.getOnlineID())+"'") < 1)
    return -1;

  curBalance = services.getDatabase().dbGetValue().toInt();

  return curBalance;
}

bool Exordium::Bank::alterBalance(const Exordium::User& origin, 
    const int amount)
{
  int curBalance, newBalance;

  // SELECT * FROM credits WHERE nick_id=origin.id
 
  newBalance = curBalance + amount;
  if(newBalance < 0)
  {
    // No overdrafts!
    return false;
  }

#ifdef DEBUG
  std::cerr << "Balance for " << origin.getNickname() << " has been altered to " 
    << newBalance << std::endl;
#endif

  // UPDATE INTO credits SET balance='newBalance' WHERE nick_id='origin.id'
  
  return true;
}
