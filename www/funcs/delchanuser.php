<?
$CHAN = new Channel();
page_start();
if ($_GET[nickid] && $_GET[chanid])
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<tr id=\"header\"><td colspan=\"2\">Access removal confirmation</td></tr>";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\">";
  echo "<input type=\"hidden\" name=\"action\" value=\"delchanuser\">";
  echo "<input type=\"hidden\" name=\"nickid\" value=\"$_GET[nickid]\">";
  echo "<input type=\"hidden\" name=\"chanid\" value=\"$_GET[chanid]\">";
  echo "<tr><td id=\"field\"><br>";
  echo "Are you <b>sure</b> you want to delete ".$NICK->getNickName($_GET[nickid])." from ".$CHAN->getChanName($_GET[chanid])."?<br>";
  echo "</td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"yes, delete this user\"></form>";
  echo "</table>";
  echo "</form>";
}
else if ($_POST[submit])
{
  if ($MYSQL->db_query("DELETE FROM chanaccess WHERE chanid='$_POST[chanid]' AND nickid='$_POST[nickid]'"))
  {
    $nick = $_SESSION[SESSION][nickname];
    $chan = $CHAN->getChanName($_POST[chanid]);
    event_log("$nick deleted nickid $_POST[nickid] from $chan s access database.");
    echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>User's access removed</td></tr>";
    echo "<tr><td id=\"field\">Successfully <b>deleted</b> the selected user from $chan's access database.<br>";
    echo "</td></tr></table>";
  }
}
else
{
  $CHAN->listChannelUsers($_POST[chan_id]);
}
page_end();
?>
