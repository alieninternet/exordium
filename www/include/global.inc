<?
// Turn off all error reporting
//error_reporting(0);

// Report simple running errors
error_reporting  (E_ERROR | E_WARNING | E_PARSE);

// Reporting E_NOTICE can be good too (to report uninitialized
// variables or catch variable name misspellings ...)
//error_reporting (E_ERROR | E_WARNING | E_PARSE | E_NOTICE);

// Report all errors except E_NOTICE
// This is the default value set in php.ini
//error_reporting (E_ALL ^ E_NOTICE);

// Report all PHP errors (bitwise 63 may be used in PHP 3)
//error_reporting (E_ALL);

$path_mail = "/usr/sbin/sendmail -t";
$salt = "69";

require "config.inc";
include "Mysql.php";
include "Channel.php";
include "Nick.php";
$debug = 0;

$MYSQL = new MySQL();
$MYSQL->db_connect();

$site["title"] = "..:: PeopleChat.org ::..";

$services_bots = array("nick" => "Nick",
                       "chan" => "Chan",
                       "serv" => "Serv",
                       "bot" => "Bot",
                       "game" => "Game",
                       "love" => "Love",
                       "stats" => "Stats",
                       "vote" => "Vote");
$admin_services_bots = array("serv" => "Serv", "oper" => "Oper", "vote" => "Vote");

$channel_modes = array("a" => "irc operator only",
               "b" => "channel bans",
               "c" => "no colors",
               "e" => "exemption ban",
               "i" => "invite only",
               "k" => "keyed entry",
               "l" => "limited users",
               "m" => "moderated channel",
               "n" => "no external topic setting",
               "o" => "channel ops",
               "p" => "private channel",
               "r" => "registered channel",
               "s" => "secret channel",
               "t" => "topic only set by ops",
               "v" => "channel voice",
               "x" => "regulated channel",
               "R" => "restricted to identified users",
               "W" => "webchat only");

$languages = array("english");

/*
** HTML Functions
*/
function
select_servicesbots($name, $selected = "FALSE")
{
  global $services_bots;
  echo "<select name=\"$name\">\n";
  while (list($bval, $bname) = each($services_bots))
  {
    echo "<option value=\"$bval\"";
    if ($bval == $selected)
      echo " selected";
    echo ">$bname</option>\n";
  }
  echo "</select>\n";
}

function
select_admin_servicesbots($name, $selected = "FALSE")
{
  global $admin_services_bots;
  echo "<select name=\"$name\">\n";
  while (list($bval, $bname) = each($admin_services_bots))
  {
    echo "<option value=\"$bval\"";
    if ($bval == $selected)
      echo " selected";
    echo ">$bname</option>\n";
  }
  echo "</select>\n";
}

function
select_languages($name, $selected = "FALSE")
{
  global $languages;
  echo "<select name=\"$name\">\n";
  while (list($li, $lname) = each($languages))
  {
    echo "<option value=\"$lname\"";
    if ($lname == $selected)
      echo " selected";
    echo ">$lname</option>\n";
  }
  echo "</select>\n";
}



/*
**  Page starting and ending.
*/
function
page_start()
{
  global $site, $debug;
  include $_SERVER[DOCUMENT_ROOT]."/include/header.php";
}

function
page_end()
{
  include $_SERVER[DOCUMENT_ROOT]."/include/footer.php";
}


/*
** User Authentication.
*/

function
user_authenticate($nickname, $password)
{
  global $MYSQL;
  $r = $MYSQL->db_query("SELECT nickname, password FROM nicks WHERE nickname='$nickname' AND password='$password'");
  if ($MYSQL->db_numrows($r) == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

function
check_auth()
{
  session_start();
  if (!session_is_registered("SESSION"))
  {
    header("Location: /error.php?e=2");
    exit();
  }
}

function
is_auth()
{
  session_start();
  if (!session_is_registered("SESSION"))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

function
has_serv_access()
{
  global $MYSQL;
  $nick = $_SESSION[SESSION][nickname];
  if ($r = $MYSQL->db_query("SELECT access FROM access WHERE service='serv' AND nickname='$nick'"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

/*
**  Logging
*/
function
event_log($message)
{
  global $MYSQL;
  $nick = $_SESSION[SESSION][nickname];
  $addr = $_SERVER[REMOTE_ADDR];
  $MYSQL->db_query("INSERT INTO log VALUES('', '$nick', 'web-user', '$addr', 'web-user', NOW(), '$message', '')");
}

function
email_notify($recipients, $subject, $email)
{
  global $path_mail;
  if ($email)
  {
    $sp = popen($path_mail, "w");
    if($sp)
    {
      fwrite($sp, "From: Peoplechat Services <services@peoplechat.org>\r\n");
      fwrite($sp, "Return-Path: Peoplechat Services <services@peoplechat.org>\r\n");
      fwrite($sp, "To: $recipients\r\n");
      fwrite($sp, "Subject: $subject\r\n");
      fwrite($sp, stripslashes($email)."\r\n");
      fwrite($sp,"\n\n-------- Env Report --------\r\n");
      fwrite($sp,"REMOTE_HOST: ".getenv("REMOTE_HOST")."\r\n");
      fwrite($sp,"REMOTE_ADDR: ".getenv("REMOTE_ADDR")."\r\n");
      fwrite($sp,"REMOTE_USER: ".getenv("REMOTE_USER")."\r\n");
      fwrite($sp,"HTTP_USER_AGENT: ".getenv("HTTP_USER_AGENT")."\r\n");

      $ret = pclose($sp);
    }
  }
}

function
is_email($email)
{
  return(preg_match("/^[-_.[:alnum:]]+@((([[:alnum:]]|
[[:alnum:]][[:alnum:]-]*[[:alnum:]])\.)+(ad|ae|aero|
af|ag|ai|al|am|an|ao|aq|ar|arpa|as|at|au|aw|az|ba|bb|
bd|be|bf|bg|bh|bi|biz|bj|bm|bn|bo|br|bs|bt|bv|bw|by|
bz|ca|cc|cd|cf|cg|ch|ci|ck|cl|cm|cn|co|com|coop|cr|
cs|cu|cv|cx|cy|cz|de|dj|dk|dm|do|dz|ec|edu|ee|eg|eh|
er|es|et|eu|fi|fj|fk|fm|fo|fr|ga|gb|gd|ge|gf|gh|gi|
gl|gm|gn|gov|gp|gq|gr|gs|gt|gu|gw|gy|hk|hm|hn|hr|ht|
hu|id|ie|il|in|info|int|io|iq|ir|is|it|jm|jo|jp|ke|
kg|kh|ki|km|kn|kp|kr|kw|ky|kz|la|lb|lc|li|lk|lr|ls|
lt|lu|lv|ly|ma|mc|md|mg|mh|mil|mk|ml|mm|mn|mo|mp|mq|
mr|ms|mt|mu|museum|mv|mw|mx|my|mz|na|name|nc|ne|net|
nf|ng|ni|nl|no|np|nr|nt|nu|nz|om|org|pa|pe|pf|pg|ph|
pk|pl|pm|pn|pr|pro|ps|pt|pw|py|qa|re|ro|ru|rw|sa|sb|
sc|sd|se|sg|sh|si|sj|sk|sl|sm|sn|so|sr|st|su|sv|sy|
sz|tc|td|tf|tg|th|tj|tk|tm|tn|to|tp|tr|tt|tv|tw|tz|
ua|ug|uk|um|us|uy|uz|va|vc|ve|vg|vi|vn|vu|wf|ws|ye|
yt|yu|za|zm|zw)$|(([0-9][0-9]?|[0-1][0-9][0-9]|
[2][0-4][0-9]|[2][5][0-5])\.){3}([0-9][0-9]?|
[0-1][0-9][0-9]|[2][0-4][0-9]|[2][5][0-5]))$/i",
$email));
}
?>
