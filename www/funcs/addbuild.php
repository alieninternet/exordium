<?
page_start();
if ($_POST[submit])
{
  $c=0;
  if (!$_POST[build])
    $err[$c++] = "You need to put in build number.";
  if (!$_POST[notes])
    $err[$c++] = "You need to enter some sort of build information.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Adding Build Information</td></tr>";
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
    if ($MYSQL->db_query("INSERT INTO builds VALUES('', '$_POST[build]', '$notes')"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick added build # $_POST[build] to the builds table.");
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Added Build Information</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully entered the build information.<br>";
      echo "</td></tr></table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"addbuild\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Add Build Information</td></tr>";
  echo "<tr><td id=\"label\">build #</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"build\" size=\"15\" maxlength=\"15\" /></td></tr>";
  echo "<tr><td id=\"label\">notes</td><td id=\"field\"><textarea name=\"notes\" cols=\"40\" rows=\"10\"></textarea></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Add Build Info\"></form>";
  echo "</table>";
}
page_end();
?>
