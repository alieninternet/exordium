# $Id$
#
# This is the layout for the MySQL backend of Exordium.
# it should be imported into a database called 'services'
# like so;
# mysqladmin create services
# mysql services < thisfile.sql
#



#
# Table structure for table 'access'
#

DROP TABLE IF EXISTS access;
CREATE TABLE access (
  id int(11) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  service varchar(32) NOT NULL default '',
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY accessIDXA (nickname(10),service(10))
) TYPE=MyISAM;

#
# Table structure for table 'chanaccess'
#

DROP TABLE IF EXISTS chanaccess;
CREATE TABLE chanaccess (
  chanid int(11) NOT NULL default '0',
  nickid int(11) NOT NULL default '0',
  access int(3) NOT NULL default '0'
) TYPE=MyISAM;

#
# Table structure for table 'chanbans'
#

DROP TABLE IF EXISTS chanbans;
CREATE TABLE chanbans (
  id int(11) NOT NULL auto_increment,
  chan int(11) NOT NULL default '0',
  mask varchar(200) NOT NULL default '',
  setby varchar(32) NOT NULL default '',
  seton datetime default NULL,
  expireon int(20) NOT NULL default '0',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'chanlogs'
#

DROP TABLE IF EXISTS chanlogs;
CREATE TABLE chanlogs (
  id int(11) NOT NULL auto_increment,
  name varchar(200) NOT NULL default '',
  llog int(20) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'chanopts'
#

DROP TABLE IF EXISTS chanopts;
CREATE TABLE chanopts (
  id int(11) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  clog tinyint(1) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'chans'
#

DROP TABLE IF EXISTS chans;
CREATE TABLE chans (
  id int(11) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  owner varchar(32) NOT NULL default '',
  topic varchar(250) NOT NULL default '',
  modes varchar(200) NOT NULL default '',
  cdesc varchar(250) default NULL,
  url varchar(200) default NULL,
  clog int(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'chanstatus'
#

DROP TABLE IF EXISTS chanstatus;
CREATE TABLE chanstatus (
  chanid int(11) NOT NULL default '0',
  nickid int(11) NOT NULL default '0',
  status int(11) NOT NULL default '0'
) TYPE=MyISAM;

#
# Table structure for table 'commands'
#

DROP TABLE IF EXISTS commands;
CREATE TABLE commands (
  id int(11) NOT NULL auto_increment,
  service varchar(32) NOT NULL default '',
  command varchar(32) NOT NULL default '',
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'emails'
#

DROP TABLE IF EXISTS emails;
CREATE TABLE emails (
  id int(11) NOT NULL auto_increment,
  gto varchar(128) default NULL,
  sub varchar(200) default NULL,
  body blob,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'foo'
#

DROP TABLE IF EXISTS foo;
CREATE TABLE foo (
  id int(1) NOT NULL default '0',
  bar set('a','e','b','c','d') default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'glines'
#

DROP TABLE IF EXISTS glines;
CREATE TABLE glines (
  id int(11) NOT NULL auto_increment,
  mask varchar(250) NOT NULL default '',
  expires int(14) NOT NULL default '0',
  setby varchar(32) NOT NULL default '',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'help'
#

DROP TABLE IF EXISTS help;
CREATE TABLE help (
  id int(11) NOT NULL auto_increment,
  service varchar(32) NOT NULL default '',
  word varchar(32) NOT NULL default '',
  parm varchar(32) NOT NULL default '',
  txt varchar(250) NOT NULL default '',
  lang varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'identified'
#

DROP TABLE IF EXISTS identified;
CREATE TABLE identified (
  id int(11) NOT NULL auto_increment,
  nick int(11) NOT NULL default '0',
  idas int(11) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'kills'
#

DROP TABLE IF EXISTS kills;
CREATE TABLE kills (
  id int(11) NOT NULL auto_increment,
  nick varchar(32) NOT NULL default '',
  tokill int(12) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'log'
#

DROP TABLE IF EXISTS log;
CREATE TABLE log (
  id int(20) NOT NULL auto_increment,
  nicknames varchar(128) NOT NULL default '',
  ident varchar(64) NOT NULL default '',
  hostname varchar(128) NOT NULL default '',
  service varchar(32) NOT NULL default '',
  did timestamp(14) NOT NULL,
  ntext mediumtext,
  cname varchar(250) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'news'
#

DROP TABLE IF EXISTS news;
CREATE TABLE news (
  id int(12) NOT NULL auto_increment,
  level int(1) NOT NULL default '0',
  expires int(12) NOT NULL default '0',
  txt varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'nicks'
#

DROP TABLE IF EXISTS nicks;
CREATE TABLE nicks (
  id int(11) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  password varchar(20) binary default NULL,
  email varchar(250) NOT NULL default '',
  registered datetime default NULL,
  lastid datetime default NULL,
  lasthost varchar(250) NOT NULL default '',
  privmsg int(1) NOT NULL default '0',
  lang varchar(20) default NULL,
  icq int(20) default NULL,
  msn varchar(200) default NULL,
  url varchar(200) NOT NULL default 'www.ircdome.org',
  deopaway int(1) NOT NULL default '0',
  yahoo varchar(32) NOT NULL default 'None Set',
  aim varchar(32) NOT NULL default 'None Set',
  quitmsg varchar(250) NOT NULL default 'No record',
  modnick tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'notes'
#

DROP TABLE IF EXISTS notes;
CREATE TABLE notes (
  id int(11) NOT NULL auto_increment,
  nfrom varchar(32) NOT NULL default '',
  nto varchar(32) NOT NULL default '',
  nsent datetime default NULL,
  note varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'onlinechan'
#

DROP TABLE IF EXISTS onlinechan;
CREATE TABLE onlinechan (
  id int(20) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'onlineclients'
#

DROP TABLE IF EXISTS onlineclients;
CREATE TABLE onlineclients (
  id int(11) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  hops int(3) NOT NULL default '1',
  timestamp int(12) NOT NULL default '0',
  username varchar(64) NOT NULL default '',
  hostname varchar(250) NOT NULL default '',
  vwhost varchar(250) NOT NULL default '',
  server varchar(128) NOT NULL default '',
  modes varchar(30) NOT NULL default '',
  realname varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'onlineservers'
#

DROP TABLE IF EXISTS onlineservers;
CREATE TABLE onlineservers (
  id int(11) NOT NULL auto_increment,
  servername varchar(250) NOT NULL default '',
  hops int(3) NOT NULL default '1',
  description varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;


#
# Table structure for table 'onlineopers'
#

DROP TABLE IF EXISTS onlineopers;
CREATE TABLE onlineopers (
  id int(11) NOT NULL auto_increment,
  nickid int(11) NOT NULL,
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id, nickid)
) TYPE=MyISAM;



#
# Table structure for table 'pendingnicks'
#

DROP TABLE IF EXISTS pendingnicks;
CREATE TABLE pendingnicks (
  id int(20) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  auth varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'serverlist'
#

DROP TABLE IF EXISTS serverlist;
CREATE TABLE serverlist (
  id int(11) NOT NULL auto_increment,
  name varchar(128) NOT NULL default '',
  hub int(1) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'stats'
#

DROP TABLE IF EXISTS stats;
CREATE TABLE stats (
  id int(12) NOT NULL auto_increment,
  maxclients int(6) NOT NULL default '0',
  maxchans int(6) NOT NULL default '0',
  maxservers int(6) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'topics'
#

DROP TABLE IF EXISTS topics;
CREATE TABLE topics (
  id int(11) NOT NULL auto_increment,
  type char(1) NOT NULL default 'A',
  contents varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'triggers'
#

DROP TABLE IF EXISTS triggers;
CREATE TABLE triggers (
  id int(11) NOT NULL auto_increment,
  host varchar(250) NOT NULL default '',
  number int(11) NOT NULL default '0',
  setby varchar(32) NOT NULL default '',
  email varchar(250) NOT NULL default '',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'words'
#

DROP TABLE IF EXISTS words;
CREATE TABLE words (
  id int(11) NOT NULL auto_increment,
  word varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

#
# Table structure for table 'credits'
#

DROP TABLE IF EXISTS credits;
CREATE TABLE credits (
  nickid int(11) NOT NULL,
  balance bigint(21),
  PRIMARY KEY  (nickid)
) TYPE=MyISAM;

