<?
$CHAN = new Channel();
page_start();
if ($_POST[submit])
{
  $c=0;
  if (!$_POST[nickname])
    $err[$c++] = "You need to enter a nickname you want to add to your room.";

  if (!$_POST[level])
    $err[$c++] = "You need to select a level to add your user at.";

  if (!$nickid = $NICK->isNickRegistered($_POST[nickname]))
    $err[$c++] = "The nickname you specified is not registered with services.";

  if ($access = $CHAN->getChanAccess($_POST[chanid], $_POST[nickname]))
    $err[$c++] = "$_POST[nickname] already has access of $access in the room you specified.";
  
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Adding a new user to the room.</td></tr>";
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
    if ($MYSQL->db_query("INSERT INTO chanaccess VALUES('$_POST[chanid]', '$nickid', '$_POST[level]')"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick added $_POST[nickname] to the chanid $_POST[chanid].");
      //echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Added user successfully</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully added $_POST[nickname] to the room's access list.<br>";
      echo "</td></tr></table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"addchanuser\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Add user to your room</td></tr>";
  echo "<tr><td id=\"label\">nickname</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"nickname\" size=\"15\" maxlength=\"15\" /></td></tr>";
  echo "<tr><td id=\"label\">room</td><td id=\"field\">";
  $CHAN->select_users_chans("chanid");
  echo "</td></tr>";
  echo "<tr><td id=\"label\">level</td><td id=\"field\">";
  $CHAN->options_chanaccess_levels("level");
  echo "</td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"add this user\"></form>";
  echo "</table>";
}
page_end();
?>
