<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";

if ($_POST[submit])
{
  global $MYSQL, $salt;
  $NICK = new Nick();
  page_start();
  $_POST[nickname] = strtolower($_POST[nickname]);
  if (!$_POST[nickname])
    $err[$c++] = "You must specify the nickname you are attempting to register.";
  if (!$_POST[email])
    $err[$c++] = "You must specify the e-mail address authorization was sent to.";
  if (!is_email($_POST[email]))
    $err[$c++] = "You must specify a valid e-mail address.";
  if (!$_POST[password])
    $err[$c++] = "You must specify the password you specified before in the password box.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error confirming your nickname</td></tr>";
    echo "<tr><td id=\"field\"><br>";
 	  echo "<ul>";
	  for ($i=0;$i < count($err); $i++)
    {
		  echo "<li>$err[$i]</li>\n";
	  }
	  echo "</ul>";
	  echo "<a href=\"javascript:history.back(1)\">Click Here</a> to go back.<br>";
	  echo "</td></tr></table>";
  }
  else
  {
    if ($NICK->is_confirm($_POST[nickname]))
    {
      $str = $_POST[nickname].$_POST[password].$_POST[email].$salt;
      $hash = md5($str);
      if ($hash != $_POST[hash])
      {
        echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
        echo "<tr id=\"header\"><td>Error confirming your nickname</td></tr>";
        echo "<tr><td id=\"field\"><br>Incorrect nickname/password combination.<br>";
	      echo "<a href=\"javascript:history.back(1)\">Click Here</a> to go back.<br>";
	      echo "</td></tr></table>";
	      exit();
      }
      else
      {
        if ($r = $MYSQL->db_query("INSERT INTO nicks (id, nickname, password, email, registered, lasthost, lastid) VALUES('', '$_POST[nickname]', '$_POST[password]', '$_POST[email]', NOW(), '$_SERVER[REMOTE_ADDR]', NOW())"))
        {
          event_log("registered the nickname $_POST[nickname] ($_SERVER[REMOTE_ADDR].");
          echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
          echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
          echo "<tr id=\"header\"><td>Successfully registered your nickname.</td></tr>";
          echo "<tr id=\"field\"><td>You have successfully registered your nickname <b>$_POST[nickname]</b><br>";
          echo "</td></tr></table>";
        }
      }
    }
    else
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Error confirming your nickname</td></tr>";
      echo "<tr><td id=\"field\"><br>There is no matching hash in the pending nicknames<br>";
	    echo "<a href=\"javascript:history.back(1)\">Click Here</a> to go back.<br>";
	    echo "</td></tr></table>";
	    exit();
    }
  }
}
else if ($_GET[hash])
{
  global $MYSQL;
  
  if ($r = $MYSQL->db_query("SELECT nickname, auth FROM nickspending WHERE auth='$_GET[hash]'"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      page_start();
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"300\" id=\"login\">\n";
      echo "<form method=\"POST\" action=\"$_SERVER[PHP_SELF]\">";
      echo "<tr id=\"header\"><td colspan=\"2\">Confirm your nickname with PeopleChat</td></tr>";
      echo "<input type=\"hidden\" name=\"action\" value=\"confirmnick\" \>";
      echo "<input type=\"hidden\" name=\"hash\" value=\"$_GET[hash]\" \>";
      echo "<tr><td id=\"label\">nickname</td><td><input id=\"q\" type=\"text\" name=\"nickname\" size=\"15\" maxlength=\"255\" /></td></tr>";
      echo "<tr><td id=\"label\">password</td><td><input id=\"q\" type=\"password\" name=\"password\" size=\"15\" maxlength=\"255\" /></td></tr>";
      echo "<tr><td id=\"label\">e-mail address</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"email\" size=\"25\" maxlength=\"255\" /></td></tr>";
      echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Confirm it!\"></td></tr>";
      echo "</form>";
      echo "</table>";
      page_end();
    }
  }
}
else
{
  header("Location: /");
}


?>
