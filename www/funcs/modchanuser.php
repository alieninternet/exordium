<?
$CHAN = new Channel();
page_start();
if ($_POST[submit])
{
  $c=0;
  if (!$_POST[level])
    $err[$c++] = "You need to select a level to add your user at.";

  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Modifying user's access in the room.</td></tr>";
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
    if ($MYSQL->db_query("UPDATE chanaccess SET access='$_POST[level]' WHERE nickid='$_POST[nickid]' AND chanid='$_POST[chanid]'"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick modified $_POST[nickname] access to the chanid $_POST[chanid] to $_POST[level].");
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Added user successfully</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully modified ".$NICK->getNickName($_POST[nickid])." in the room's access list.<br>";
      echo "</td></tr></table>";
    }
  }
}
else if ($_GET[nickid] && $_GET[chanid])
{
  if ($r = $MYSQL->db_query("SELECT chanid, nickid, access FROM chanaccess WHERE nickid='$_GET[nickid]' AND chanid='$_GET[chanid]'"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      $row = $MYSQL->db_fetch_object($r);
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\">";
      echo "<input type=\"hidden\" name=\"action\" value=\"modchanuser\">";
      echo "<input type=\"hidden\" name=\"nickid\" value=\"$_GET[nickid]\">";
      echo "<tr id=\"header\"><td colspan=\"2\">Modify a users access in your room.</td></tr>";
      echo "<tr><td id=\"label\">nickname</td><td id=\"field\">".$NICK->getNickName($_GET[nickid])."</td></tr>";
      echo "<tr><td id=\"label\">room</td><td id=\"field\">";
      $CHAN->select_users_chans("chanid", $_GET[chanid]);
      echo "</td></tr>";
      echo "<tr><td id=\"label\">level</td><td id=\"field\">";
      $CHAN->options_chanaccess_levels("level", $row->access);
      echo "</td></tr>";
      echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"modify this user\"></form>";
      echo "</table>";
    }
  }
}
else
{
  $CHAN->listChannelUsers($_POST[chan_id]);
}
page_end();
?>
