<?
page_start();
if ($_GET[id])
{
  $comm = $MYSQL->db_get("SELECT service, command, access FROM commands WHERE id='$_GET[id]'");
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"modcommand\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Modify User Commands</td></tr>";
  echo "<tr><td id=\"label\">service</td><td id=\"field\">";
  select_admin_servicesbots("service", $comm->service);
  echo "</td></tr>";
  echo "<tr><td id=\"label\">command (eg: <u>help</u>)</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"command\" size=\"15\" maxlength=\"255\" value=\"$comm->command\"/></td></tr>";
  echo "<tr><td id=\"label\">access</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"access\" size=\"15\" maxlength=\"255\" value=\"$comm->access\"/></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Modify this Command\"></form>";
  echo "</table>";
}
elseif ($_POST[submit])
{
  $c=0;
  if (!$_POST[command])
    $err[$c++] = "You need to put in a command.";
  if (!$_POST[access])
    $err[$c++] = "You need to put an access level.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Adding Command</td></tr>";
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
    if ($MYSQL->db_query("UPDATE commands SET service='$_POST[service]', command='$_POST[command]', access='$_POST[access]' WHERE id='$_POST[id]'"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick modified $_POST[service] $_POST[command] $_POST[access] in the commands table.");
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Updated Command</td></tr>";
      echo "<tr id=\"field\"><td>The modifications you have submitted have been updated in the database.<br><br>You will now be redirected.</td></tr>";
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    }
  }
}
else
{
  if ($r = $MYSQL->db_query("SELECT id, service, command, access FROM commands ORDER BY service"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td colspan=\"5\">Current Commands</td></tr>";
      echo "<tr id=\"header\"><td>&nbsp;</td><td>service</td><td>command</td><td>access</td></tr>";
      while ($row = $MYSQL->db_fetch_object($r))
      {
        echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=modcommand&id=$row->id\">mod</a>]</td><td>$row->service</td><td>$row->command</td><td>$row->access</td></tr>\n";
      }
      echo "</table>";
    }
    else
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
      echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addcommand\">Click Here</a> to add commands.<br>";
      echo "</table>";
    }
  }
}
page_end();
?>
