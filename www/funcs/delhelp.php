<?
page_start();
if ($_GET[id])
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<tr id=\"header\"><td>Confirmation of Deletion</td></tr>";
  echo "<tr><td id=\"centerfield\">Are you <u>sure</u> you want to delete this help entry?<br><br>";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"delhelp\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Confirm Deletion\"></form>";
  echo "</td></tr></table>";
}
elseif ($_POST[submit])
{
  if ($MYSQL->db_query("DELETE FROM help WHERE id='$_POST[id]'"))
  {
    $nick = $_SESSION[SESSION][nickname];
    event_log("$nick deleted helpid $_POST[id] from the help table.");
    echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>Help File Deleted</td></tr>";
    echo "<tr><td id=\"field\">Successfully <b>deleted</b> the selected help file.<br>";
    echo "</td></tr></table>";
  }
}
else
{
  if ($r = $MYSQL->db_query("SELECT id, service, word, parm, lang FROM help ORDER BY service"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td colspan=\"5\">Current User Help Files</td></tr>";
      echo "<tr id=\"header\"><td>&nbsp;</td><td>service</td><td>command</td><td>optional parm</td><td>language</td></tr>";
      while ($row = $MYSQL->db_fetch_object($r))
      {
        echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=delhelp&id=$row->id\">del</a>]</td><td>$row->service</td><td>$row->word</td><td>$row->parm</td><td>$row->lang</td></tr>\n";
      }
      echo "</table>";
    }
    else
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
      echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addhelp\">Click Here</a> to add help entries.<br>";
      echo "</table>";
    }
  }
}
page_end();
?>
