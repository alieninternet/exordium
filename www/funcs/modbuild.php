<?
page_start();
if ($_GET[id])
{
  $r = $MYSQL->db_query("SELECT build, notes FROM builds WHERE id='$_GET[id]'");
  $build = $MYSQL->db_fetch_object($r);
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"modbuild\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Modify Build Information</td></tr>";
  echo "<tr><td id=\"label\">build #</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"build\" size=\"15\" maxlength=\"15\" value=\"$build->build\"/></td></tr>";
  $notes = stripslashes($build->notes);
  echo "<tr><td id=\"label\">notes</td><td id=\"field\"><textarea name=\"notes\" cols=\"40\" rows=\"10\">$notes</textarea></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Modify this Build Info\"></form>";
  echo "</table>";
}
elseif ($_POST[submit])
{
  $c=0;
  if (!$_POST[build])
    $err[$c++] = "You need to put in build number.";
  if (!$_POST[notes])
    $err[$c++] = "You need to enter some sort of build information.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Modifying Build Information</td></tr>";
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
    $notes = addslashes($_POST[notes]);
    if ($MYSQL->db_query("UPDATE builds SET build='$_POST[build]', notes='$notes' WHERE id='$_POST[id]'"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick modified build #: $_POST[build] in the builds table.");
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Updated Build Information</td></tr>";
      echo "<tr id=\"field\"><td>The modifications you have submitted have been updated in the database.<br><br>You will now be redirected.</td></tr>";
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    }
  }
}
else
{
  if ($r = $MYSQL->db_query("SELECT id, build, notes FROM builds ORDER BY id"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td colspan=\"5\">Current Builds and Information</td></tr>";
      echo "<tr id=\"header\"><td>&nbsp;</td><td>build #</td><td>notes</td></tr>";
      while ($row = $MYSQL->db_fetch_object($r))
      {
        echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=modbuild&id=$row->id\">mod</a>]</td><td>$row->build</td><td>$row->notes</td></tr>\n";
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
