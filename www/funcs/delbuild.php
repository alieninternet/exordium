<?
page_start();
if ($_GET[id])
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<tr id=\"header\"><td>Confirmation of Deletion</td></tr>";
  echo "<tr><td id=\"centerfield\">Are you <u>sure</u> you want to delete this build entry?<br><br>";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"delbuild\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Confirm Deletion\"></form>";
  echo "</td></tr></table>";
}
elseif ($_POST[submit])
{
  if ($MYSQL->db_query("DELETE FROM builds WHERE id='$_POST[id]'"))
  {
    $nick = $_SESSION[SESSION][nickname];
    event_log("$nick deleted buildid $_POST[id] from the builds table.");
    echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>Build Information Deleted</td></tr>";
    echo "<tr><td id=\"field\">Successfully <b>deleted</b> the selected build information.<br>";
    echo "</td></tr></table>";
  }
}
else
{
  if ($r = $MYSQL->db_query("SELECT id, build, LEFT(notes, 15) as notes FROM builds ORDER BY id"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td colspan=\"5\">Current Builds and Information</td></tr>";
      echo "<tr id=\"header\"><td>&nbsp;</td><td>build</td><td>notes</td></tr>";
      while ($row = $MYSQL->db_fetch_object($r))
      {
        $notes = stripslashes($row->notes);
        echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=delbuild&id=$row->id\">del</a>]</td><td>$row->build</td><td>$notes</td></tr>\n";
      }
      echo "</table>";
    }
    else
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
      echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addbuild\">Click Here</a> to add build information.<br>";
      echo "</table>";
    }
  }
}
page_end();
?>
