-- MySQL dump 9.07
--
-- Host: localhost    Database: services
---------------------------------------------------------
-- Server version	4.0.12-log

--
-- Table structure for table 'access'
--

CREATE TABLE access (
  id int(11) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  service varchar(32) NOT NULL default '',
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY service_idx (nickname(10),service(5)),
  KEY service_srv_idx (service(5))
) TYPE=MyISAM;

--
-- Table structure for table 'bank'
--

CREATE TABLE bank (
  nickid int(11) NOT NULL default '0',
  balance bigint(21) default NULL,
  PRIMARY KEY  (nickid),
  KEY bank_idx (nickid)
) TYPE=MyISAM;

--
-- Table structure for table 'builds'
--

CREATE TABLE builds (
  id int(12) NOT NULL auto_increment,
  build int(12) NOT NULL default '0',
  notes mediumtext,
  PRIMARY KEY  (id),
  KEY bulid_idx (build)
) TYPE=MyISAM;

--
-- Table structure for table 'chanaccess'
--

CREATE TABLE chanaccess (
  chanid int(11) NOT NULL default '0',
  nickid int(11) NOT NULL default '0',
  access int(3) NOT NULL default '0',
  KEY chanaccess_idx (nickid,access)
) TYPE=MyISAM;

--
-- Table structure for table 'chanbans'
--

CREATE TABLE chanbans (
  id int(11) NOT NULL auto_increment,
  chan int(11) NOT NULL default '0',
  mask varchar(200) NOT NULL default '',
  setby varchar(32) NOT NULL default '',
  seton datetime default NULL,
  expireon int(20) NOT NULL default '0',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id),
  KEY chanbans_idx (chan)
) TYPE=MyISAM;

--
-- Table structure for table 'chanfreeze'
--

CREATE TABLE chanfreeze (
  id int(12) NOT NULL auto_increment,
  name int(12) NOT NULL default '0',
  setby varchar(32) NOT NULL default '',
  seton datetime default NULL,
  expires int(12) NOT NULL default '0',
  reason varchar(250) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY chanfreeze_idx (name),
  KEY chanfreeze_setby_idx (setby)
) TYPE=MyISAM;

--
-- Table structure for table 'chanlogs'
--

CREATE TABLE chanlogs (
  id int(11) NOT NULL auto_increment,
  name varchar(200) NOT NULL default '',
  llog int(20) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY chanlogs_idx (name)
) TYPE=MyISAM;

--
-- Table structure for table 'chanmodes'
--

CREATE TABLE chanmodes (
  id int(11) NOT NULL auto_increment,
  mode char(2) NOT NULL default '',
  tdesc varchar(50) NOT NULL default '',
  html varchar(255) NOT NULL default '',
  PRIMARY KEY  (id),
  KEY chanmodes_idx (mode)
) TYPE=MyISAM;

--
-- Table structure for table 'chanopts'
--

CREATE TABLE chanopts (
  id int(11) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  clog tinyint(1) NOT NULL default '0',
  secure tinyint(1) NOT NULL default '0',
  private tinyint(1) NOT NULL default '0',
  enforcebans tinyint(1) NOT NULL default '0',
  tracktopics tinyint(1) NOT NULL default '0',
  modelock tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY chanopts_idx (name)
) TYPE=MyISAM;

--
-- Table structure for table 'chans'
--

CREATE TABLE chans (
  id int(11) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  owner varchar(32) NOT NULL default '',
  regdate datetime NOT NULL,
  topic mediumtext NOT NULL,
  modes varchar(200) NOT NULL default '',
  cdesc varchar(250) default NULL,
  url varchar(200) default NULL,
  entrymsg varchar(200) default NULL,
  partmsg varchar(200) default NULL,
  clog int(20) default '0',
  PRIMARY KEY  (id),
  KEY chans_idx (name),
  KEY chans_owner_idx (owner)
) TYPE=MyISAM;

--
-- Table structure for table 'chanstatus'
--

CREATE TABLE chanstatus (
  chanid int(11) NOT NULL default '0',
  nickid int(11) NOT NULL default '0',
  status int(11) NOT NULL default '0',
  KEY chanstatus_idx (chanid,nickid)
) TYPE=MyISAM;

--
-- Table structure for table 'commands'
--

CREATE TABLE commands (
  id int(11) NOT NULL auto_increment,
  service varchar(32) NOT NULL default '',
  command varchar(32) NOT NULL default '',
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY commands_idx (service,command)
) TYPE=MyISAM;

--
-- Table structure for table 'emails'
--

CREATE TABLE emails (
  id int(11) NOT NULL auto_increment,
  gto varchar(128) default NULL,
  sub varchar(200) default NULL,
  body blob,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'glines'
--

CREATE TABLE glines (
  id int(11) NOT NULL auto_increment,
  mask varchar(250) NOT NULL default '',
  expires int(14) NOT NULL default '0',
  setby varchar(32) NOT NULL default '',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id),
  KEY glines_idx (setby)
) TYPE=MyISAM;

--
-- Table structure for table 'help'
--

CREATE TABLE help (
  id int(11) NOT NULL auto_increment,
  service varchar(32) NOT NULL default '',
  word varchar(32) NOT NULL default '',
  parm varchar(32) NOT NULL default '',
  txt varchar(250) NOT NULL default '',
  lang varchar(20) default NULL,
  PRIMARY KEY  (id),
  KEY help_idx (service,word,parm,txt)
) TYPE=MyISAM;

--
-- Table structure for table 'kills'
--

CREATE TABLE kills (
  id int(11) NOT NULL auto_increment,
  nick varchar(32) NOT NULL default '',
  tokill int(12) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'log'
--

CREATE TABLE log (
  id int(20) NOT NULL auto_increment,
  nicknames varchar(128) NOT NULL default '',
  ident varchar(64) NOT NULL default '',
  hostname varchar(128) NOT NULL default '',
  service varchar(32) NOT NULL default '',
  did timestamp(14) NOT NULL,
  ntext mediumtext,
  cname varchar(250) default NULL,
  PRIMARY KEY  (id),
  KEY log_idx_1 (nicknames,ident,hostname,service)
) TYPE=MyISAM;

--
-- Table structure for table 'news'
--

CREATE TABLE news (
  id int(12) NOT NULL auto_increment,
  level int(1) NOT NULL default '0',
  expires int(12) NOT NULL default '0',
  txt varchar(250) NOT NULL default '',
  PRIMARY KEY  (id),
  KEY news_idx (level,expires)
) TYPE=MyISAM;

--
-- Table structure for table 'nicks'
--

CREATE TABLE nicks (
  id int(11) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  password varchar(60) NOT NULL default 'NONESET',
  email varchar(25) NOT NULL default 'NONE@RECORDED',
  registered datetime NOT NULL default '0000-00-00 00:00:00',
  lastid datetime NOT NULL default '0000-00-00 00:00:00',
  lasthost varchar(250) NOT NULL default 'None@Recorded',
  privmsg int(1) NOT NULL default '0',
  lang varchar(20) NOT NULL default 'english',
  icq int(20) NOT NULL default '0',
  msn varchar(200) NOT NULL default 'None Set',
  url varchar(200) NOT NULL default 'www.peoplechat.org',
  deopaway int(1) NOT NULL default '0',
  yahoo varchar(32) NOT NULL default 'None Set',
  aim varchar(32) NOT NULL default 'None Set',
  quitmsg varchar(250) NOT NULL default 'No record',
  modnick tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (id),
  KEY nicks_idx (nickname,password)
) TYPE=MyISAM;

--
-- Table structure for table 'nicksidentified'
--

CREATE TABLE nicksidentified (
  id int(11) NOT NULL auto_increment,
  nick int(11) NOT NULL default '0',
  idas int(11) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'nickspending'
--

CREATE TABLE nickspending (
  id int(20) NOT NULL auto_increment,
  nickname varchar(32) NOT NULL default '',
  auth varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'notes'
--

CREATE TABLE notes (
  id int(11) NOT NULL auto_increment,
  nfrom varchar(32) NOT NULL default '',
  nto varchar(32) NOT NULL default '',
  nsent datetime default NULL,
  note varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'onlinechan'
--

CREATE TABLE onlinechan (
  id int(20) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'onlineclients'
--

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

--
-- Table structure for table 'onlineopers'
--

CREATE TABLE onlineopers (
  id int(11) NOT NULL auto_increment,
  nickid int(11) NOT NULL default '0',
  access int(3) NOT NULL default '0',
  PRIMARY KEY  (id,nickid)
) TYPE=MyISAM;

--
-- Table structure for table 'onlineservers'
--

CREATE TABLE onlineservers (
  id int(11) NOT NULL auto_increment,
  servername varchar(250) NOT NULL default '',
  hops int(3) NOT NULL default '1',
  description varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'serverlist'
--

CREATE TABLE serverlist (
  id int(11) NOT NULL auto_increment,
  name varchar(128) NOT NULL default '',
  hub int(1) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'stats'
--

CREATE TABLE stats (
  id int(12) NOT NULL auto_increment,
  maxclients int(6) NOT NULL default '0',
  maxchans int(6) NOT NULL default '0',
  maxservers int(6) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'stats_chans'
--

CREATE TABLE stats_chans (
  id int(12) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  users int(12) NOT NULL default '0',
  topic varchar(250) NOT NULL default '',
  modes varchar(200) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'stats_global'
--

CREATE TABLE stats_global (
  id int(12) NOT NULL auto_increment,
  users int(12) NOT NULL default '0',
  servers int(12) NOT NULL default '0',
  channels int(12) NOT NULL default '0',
  maxusers int(12) NOT NULL default '0',
  maxopers int(12) NOT NULL default '0',
  maxservers int(12) NOT NULL default '0',
  maxusers_time datetime default NULL,
  maxopers_time datetime default NULL,
  maxservers_time datetime default NULL,
  lastrun datetime default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'stats_servers'
--

CREATE TABLE stats_servers (
  id int(12) NOT NULL auto_increment,
  name varchar(250) NOT NULL default '',
  users int(12) NOT NULL default '0',
  opers int(12) NOT NULL default '0',
  maxusers int(12) NOT NULL default '0',
  maxopers int(12) NOT NULL default '0',
  maxusers_time datetime default NULL,
  maxopers_time datetime default NULL,
  lag float NOT NULL default '0',
  version varchar(250) NOT NULL default '0',
  uptime int(12) NOT NULL default '0',
  maxuptime int(12) NOT NULL default '0',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Table structure for table 'triggers'
--

CREATE TABLE triggers (
  id int(11) NOT NULL auto_increment,
  host varchar(250) NOT NULL default '',
  number int(11) NOT NULL default '0',
  setby varchar(32) NOT NULL default '',
  email varchar(250) NOT NULL default '',
  reason varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
) TYPE=MyISAM;

