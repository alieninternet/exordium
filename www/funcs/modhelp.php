<?
page_start();
if ($_GET[id])
{
  $help = $MYSQL->db_get("SELECT service, word, parm, txt, lang FROM help WHERE id='$_GET[id]'");
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"modhelp\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Modify User Help Files</td></tr>";
  echo "<tr><td id=\"label\">service</td><td id=\"field\">";
  select_servicesbots("service", $help->service);
  echo "</td></tr>";
  echo "<tr><td id=\"label\">command (eg: <u>help</u>)</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"word\" size=\"15\" maxlength=\"255\" value=\"$help->word\"/></td></tr>";
  echo "<tr><td id=\"label\">optional parameter (eg: <u>set</u>)</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"parm\" size=\"15\" maxlength=\"255\" value=\"$help->parm\"/></td></tr>";
  echo "<tr><td id=\"label\">language</td><td id=\"field\">";
  select_languages("lang", $help->lang);
  echo "</td></tr>";
  $txt = stripslashes($help->txt);
  echo "<tr><td id=\"label\">help text<p>legend:<br>&nbsp;%b% = <b>bold</b><br>&nbsp;%u% = <u>underline</u><br>&nbsp;%r% = reverse</p></td><td id=\"field\"><textarea name=\"txt\" cols=\"40\" rows=\"10\">$txt</textarea></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Modify this Help\"></form>";
  echo "</table>";
}
elseif ($_POST[submit])
{
  $c=0;
  if (!$_POST[word])
    $err[$c++] = "You need to put in a command.";
  if (!$_POST[txt])
    $err[$c++] = "You need to enter some sort of help information.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Add User Help Files</td></tr>";
    echo "<tr><td>";
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
    $txt = addslashes($_POST[txt]);
    if ($MYSQL->db_query("UPDATE help SET service='$_POST[service]', word='$_POST[word]', parm='$_POST[parm]', txt='$txt', lang='$_POST[lang]' WHERE id='$_POST[id]'"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick modified $_POST[service] help $_POST[word] $_POST[parm] in the help table.");
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Updated Help File</td></tr>";
      echo "<tr id=\"field\"><td>The modifications you have submitted have been updated in the database.<br><br>You will now be redirected.</td></tr>";
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
    }
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
        echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=modhelp&id=$row->id\">mod</a>]</td><td>$row->service</td><td>$row->word</td><td>$row->parm</td><td>$row->lang</td></tr>\n";
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
