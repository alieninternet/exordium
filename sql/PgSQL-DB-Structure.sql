------------------------------------------------------------------
-- My2Pg 1.27 translated dump
--
------------------------------------------------------------------

BEGIN;




--
-- Sequences for table ACCESS
--

CREATE SEQUENCE access_id_seq;

-- MySQL dump 8.22
--
-- Host: 127.0.0.1    Database: services
---------------------------------------------------------
-- Server version	3.23.52

--
-- Table structure for table 'access'
--

CREATE TABLE access (
  id INT4 DEFAULT nextval('access_id_seq'),
  nickname varchar(32) NOT NULL DEFAULT '',
  service varchar(32) NOT NULL DEFAULT '',
  access INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'access'
--



--
-- Table structure for table 'chanaccess'
--

CREATE TABLE chanaccess (
  chanid INT4 NOT NULL DEFAULT '0',
  nickid INT4 NOT NULL DEFAULT '0',
  access INT4 NOT NULL DEFAULT '0'
);

--
-- Dumping data for table 'chanaccess'
--



--
-- Table structure for table 'chanbans'
--



--
-- Sequences for table CHANBANS
--

CREATE SEQUENCE chanbans_id_seq;

CREATE TABLE chanbans (
  id INT4 DEFAULT nextval('chanbans_id_seq'),
  chan INT4 NOT NULL DEFAULT '0',
  mask varchar(200) NOT NULL DEFAULT '',
  setby varchar(32) NOT NULL DEFAULT '',
  seton TIMESTAMP DEFAULT NULL,
  expireon INT4 NOT NULL DEFAULT '0',
  reason varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'chanbans'
--



--
-- Table structure for table 'chanlogs'
--



--
-- Sequences for table CHANLOGS
--

CREATE SEQUENCE chanlogs_id_seq;

CREATE TABLE chanlogs (
  id INT4 DEFAULT nextval('chanlogs_id_seq'),
  name varchar(200) NOT NULL DEFAULT '',
  llog INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'chanlogs'
--



--
-- Table structure for table 'chanopts'
--



--
-- Sequences for table CHANOPTS
--

CREATE SEQUENCE chanopts_id_seq;

CREATE TABLE chanopts (
  id INT4 DEFAULT nextval('chanopts_id_seq'),
  name varchar(250) NOT NULL DEFAULT '',
  clog INT2 DEFAULT NULL,
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'chanopts'
--



--
-- Table structure for table 'chans'
--



--
-- Sequences for table CHANS
--

CREATE SEQUENCE chans_id_seq;

CREATE TABLE chans (
  id INT4 DEFAULT nextval('chans_id_seq'),
  name varchar(250) NOT NULL DEFAULT '',
  owner varchar(32) NOT NULL DEFAULT '',
  topic varchar(250) NOT NULL DEFAULT '',
  modes varchar(200) NOT NULL DEFAULT '',
  cdesc varchar(250) DEFAULT NULL,
  url varchar(200) DEFAULT NULL,
  clog INT4 DEFAULT NULL,
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'chans'
--



--
-- Table structure for table 'chanstatus'
--

CREATE TABLE chanstatus (
  chanid INT4 NOT NULL DEFAULT '0',
  nickid INT4 NOT NULL DEFAULT '0',
  status INT4 NOT NULL DEFAULT '0'
);

--
-- Dumping data for table 'chanstatus'
--


INSERT INTO chanstatus VALUES (1,1,2);

--
-- Table structure for table 'commands'
--



--
-- Sequences for table COMMANDS
--

CREATE SEQUENCE commands_id_seq;

CREATE TABLE commands (
  id INT4 DEFAULT nextval('commands_id_seq'),
  service varchar(32) NOT NULL DEFAULT '',
  command varchar(32) NOT NULL DEFAULT '',
  access INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'commands'
--



--
-- Table structure for table 'emails'
--



--
-- Sequences for table EMAILS
--

CREATE SEQUENCE emails_id_seq;

CREATE TABLE emails (
  id INT4 DEFAULT nextval('emails_id_seq'),
  gto varchar(128) DEFAULT NULL,
  sub varchar(200) DEFAULT NULL,
  body text,
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'emails'
--




--
-- Table structure for table 'glines'
--



--
-- Sequences for table GLINES
--

CREATE SEQUENCE glines_id_seq;

CREATE TABLE glines (
  id INT4 DEFAULT nextval('glines_id_seq'),
  mask varchar(250) NOT NULL DEFAULT '',
  expires INT4 NOT NULL DEFAULT '0',
  setby varchar(32) NOT NULL DEFAULT '',
  reason varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'glines'
--



--
-- Table structure for table 'help'
--



--
-- Sequences for table HELP
--

CREATE SEQUENCE help_id_seq;

CREATE TABLE help (
  id INT4 DEFAULT nextval('help_id_seq'),
  service varchar(32) NOT NULL DEFAULT '',
  word varchar(32) NOT NULL DEFAULT '',
  parm varchar(32) NOT NULL DEFAULT '',
  txt varchar(250) NOT NULL DEFAULT '',
  lang varchar(20) DEFAULT NULL,
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'help'
--



--
-- Table structure for table 'identified'
--



--
-- Sequences for table IDENTIFIED
--

CREATE SEQUENCE identified_id_seq;

CREATE TABLE identified (
  id INT4 DEFAULT nextval('identified_id_seq'),
  nick INT4 NOT NULL DEFAULT '0',
  idas INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'identified'
--



--
-- Table structure for table 'kills'
--



--
-- Sequences for table KILLS
--

CREATE SEQUENCE kills_id_seq;

CREATE TABLE kills (
  id INT4 DEFAULT nextval('kills_id_seq'),
  nick varchar(32) NOT NULL DEFAULT '',
  tokill INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'kills'
--



--
-- Table structure for table 'log'
--



--
-- Sequences for table LOG
--

CREATE SEQUENCE log_id_seq;

CREATE TABLE log (
  id INT4 DEFAULT nextval('log_id_seq'),
  nicknames varchar(128) NOT NULL DEFAULT '',
  ident varchar(64) NOT NULL DEFAULT '',
  hostname varchar(128) NOT NULL DEFAULT '',
  service varchar(32) NOT NULL DEFAULT '',
  did TIMESTAMP NOT NULL,
  ntext text,
  cname varchar(250) DEFAULT NULL,
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'log'
--



--
-- Table structure for table 'news'
--



--
-- Sequences for table NEWS
--

CREATE SEQUENCE news_id_seq;

CREATE TABLE news (
  id INT4 DEFAULT nextval('news_id_seq'),
  level INT4 NOT NULL DEFAULT '0',
  expires INT4 NOT NULL DEFAULT '0',
  txt varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'news'
--



--
-- Table structure for table 'nicks'
--



--
-- Sequences for table NICKS
--

CREATE SEQUENCE nicks_id_seq;

CREATE TABLE nicks (
  id INT4 DEFAULT nextval('nicks_id_seq'),
  nickname varchar(32) NOT NULL DEFAULT '',
  password varchar(20) binary DEFAULT NULL,
  email varchar(250) NOT NULL DEFAULT '',
  registered TIMESTAMP DEFAULT NULL,
  lastid TIMESTAMP DEFAULT NULL,
  lasthost varchar(250) NOT NULL DEFAULT '',
  privmsg INT4 NOT NULL DEFAULT '0',
  lang varchar(20) DEFAULT NULL,
  icq INT4 DEFAULT NULL,
  msn varchar(200) DEFAULT NULL,
  url varchar(200) NOT NULL DEFAULT 'www.ircdome.org',
  deopaway INT4 NOT NULL DEFAULT '0',
  yahoo varchar(32) NOT NULL DEFAULT 'None Set',
  aim varchar(32) NOT NULL DEFAULT 'None Set',
  quitmsg varchar(250) NOT NULL DEFAULT 'No record',
  modnick INT2 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'nicks'
--


INSERT INTO nicks VALUES (1,'blah','.','.','0001-01-01 00:00:00','0001-01-01 00:00:00','.',0,'.',0,'.','.',0,'.','.','.',0);

--
-- Table structure for table 'notes'
--



--
-- Sequences for table NOTES
--

CREATE SEQUENCE notes_id_seq;

CREATE TABLE notes (
  id INT4 DEFAULT nextval('notes_id_seq'),
  nfrom varchar(32) NOT NULL DEFAULT '',
  nto varchar(32) NOT NULL DEFAULT '',
  nsent TIMESTAMP DEFAULT NULL,
  note varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'notes'
--



--
-- Table structure for table 'onlinechan'
--



--
-- Sequences for table ONLINECHAN
--

CREATE SEQUENCE onlinechan_id_seq;

CREATE TABLE onlinechan (
  id INT4 DEFAULT nextval('onlinechan_id_seq'),
  name varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'onlinechan'
--


INSERT INTO onlinechan VALUES (1,'#chaty');

--
-- Table structure for table 'onlineclients'
--



--
-- Sequences for table ONLINECLIENTS
--

CREATE SEQUENCE onlineclients_id_seq;

CREATE TABLE onlineclients (
  id INT4 DEFAULT nextval('onlineclients_id_seq'),
  nickname varchar(32) NOT NULL DEFAULT '',
  hops INT4 NOT NULL DEFAULT '1',
  timestamp INT4 NOT NULL DEFAULT '0',
  username varchar(64) NOT NULL DEFAULT '',
  hostname varchar(250) NOT NULL DEFAULT '',
  vwhost varchar(250) NOT NULL DEFAULT '',
  server varchar(128) NOT NULL DEFAULT '',
  modes varchar(30) NOT NULL DEFAULT '',
  realname varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'onlineclients'
--



--
-- Table structure for table 'onlineopers'
--



--
-- Sequences for table ONLINEOPERS
--

CREATE SEQUENCE onlineopers_id_seq;

CREATE TABLE onlineopers (
  id INT4 DEFAULT nextval('onlineopers_id_seq'),
  nickid INT4 NOT NULL DEFAULT '0',
  access INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id,nickid)

);

--
-- Dumping data for table 'onlineopers'
--



--
-- Table structure for table 'onlineservers'
--



--
-- Sequences for table ONLINESERVERS
--

CREATE SEQUENCE onlineservers_id_seq;

CREATE TABLE onlineservers (
  id INT4 DEFAULT nextval('onlineservers_id_seq'),
  servername varchar(250) NOT NULL DEFAULT '',
  hops INT4 NOT NULL DEFAULT '1',
  description varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'onlineservers'
--


INSERT INTO onlineservers VALUES (1,'dfwin.neo.net',1,':WinIRCD test server\r');

--
-- Table structure for table 'pendingnicks'
--



--
-- Sequences for table PENDINGNICKS
--

CREATE SEQUENCE pendingnicks_id_seq;

CREATE TABLE pendingnicks (
  id INT4 DEFAULT nextval('pendingnicks_id_seq'),
  nickname varchar(32) NOT NULL DEFAULT '',
  auth varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'pendingnicks'
--



--
-- Table structure for table 'serverlist'
--



--
-- Sequences for table SERVERLIST
--

CREATE SEQUENCE serverlist_id_seq;

CREATE TABLE serverlist (
  id INT4 DEFAULT nextval('serverlist_id_seq'),
  name varchar(128) NOT NULL DEFAULT '',
  hub INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'serverlist'
--



--
-- Table structure for table 'stats'
--



--
-- Sequences for table STATS
--

CREATE SEQUENCE stats_id_seq;

CREATE TABLE stats (
  id INT4 DEFAULT nextval('stats_id_seq'),
  maxclients INT4 NOT NULL DEFAULT '0',
  maxchans INT4 NOT NULL DEFAULT '0',
  maxservers INT4 NOT NULL DEFAULT '0',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'stats'
--



--
-- Table structure for table 'topics'
--



--
-- Sequences for table TOPICS
--

CREATE SEQUENCE topics_id_seq;

CREATE TABLE topics (
  id INT4 DEFAULT nextval('topics_id_seq'),
  type char(1) NOT NULL DEFAULT 'A',
  contents varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'topics'
--



--
-- Table structure for table 'triggers'
--



--
-- Sequences for table TRIGGERS
--

CREATE SEQUENCE triggers_id_seq;

CREATE TABLE triggers (
  id INT4 DEFAULT nextval('triggers_id_seq'),
  host varchar(250) NOT NULL DEFAULT '',
  number INT4 NOT NULL DEFAULT '0',
  setby varchar(32) NOT NULL DEFAULT '',
  email varchar(250) NOT NULL DEFAULT '',
  reason varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'triggers'
--



--
-- Table structure for table 'words'
--



--
-- Sequences for table WORDS
--

CREATE SEQUENCE words_id_seq;

CREATE TABLE words (
  id INT4 DEFAULT nextval('words_id_seq'),
  word varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (id)

);

--
-- Dumping data for table 'words'
--





--
-- Indexes for table ACCESS
--

CREATE INDEX accessIDXA_access_index ON access (nickname(10),service(10));

--
-- Sequences for table HELP
--

SELECT SETVAL('help_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from help));

--
-- Sequences for table GLINES
--

SELECT SETVAL('glines_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from glines));

--
-- Sequences for table TOPICS
--

SELECT SETVAL('topics_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from topics));

--
-- Sequences for table NOTES
--

SELECT SETVAL('notes_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from notes));

--
-- Sequences for table NICKS
--

SELECT SETVAL('nicks_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from nicks));

--
-- Sequences for table IDENTIFIED
--

SELECT SETVAL('identified_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from identified));

--
-- Sequences for table CHANBANS
--

SELECT SETVAL('chanbans_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from chanbans));

--
-- Sequences for table WORDS
--

SELECT SETVAL('words_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from words));

--
-- Sequences for table CHANS
--

SELECT SETVAL('chans_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from chans));

--
-- Sequences for table COMMANDS
--

SELECT SETVAL('commands_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from commands));

--
-- Sequences for table LOG
--

SELECT SETVAL('log_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from log));

--
-- Sequences for table TRIGGERS
--

SELECT SETVAL('triggers_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from triggers));

--
-- Sequences for table SERVERLIST
--

SELECT SETVAL('serverlist_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from serverlist));

--
-- Sequences for table CHANLOGS
--

SELECT SETVAL('chanlogs_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from chanlogs));

--
-- Sequences for table ONLINECHAN
--

SELECT SETVAL('onlinechan_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from onlinechan));

--
-- Sequences for table NEWS
--

SELECT SETVAL('news_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from news));

--
-- Sequences for table ONLINESERVERS
--

SELECT SETVAL('onlineservers_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from onlineservers));

--
-- Sequences for table KILLS
--

SELECT SETVAL('kills_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from kills));

--
-- Sequences for table ONLINEOPERS
--

SELECT SETVAL('onlineopers_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from onlineopers));

--
-- Sequences for table EMAILS
--

SELECT SETVAL('emails_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from emails));

--
-- Sequences for table ACCESS
--

SELECT SETVAL('access_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from access));

--
-- Sequences for table ONLINECLIENTS
--

SELECT SETVAL('onlineclients_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from onlineclients));

--
-- Sequences for table STATS
--

SELECT SETVAL('stats_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from stats));

--
-- Sequences for table PENDINGNICKS
--

SELECT SETVAL('pendingnicks_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from pendingnicks));

--
-- Sequences for table CHANOPTS
--

SELECT SETVAL('chanopts_id_seq',(select case when max(id)>0 then max(id)+1 else 1 end from chanopts));

COMMIT;
