<?
page_start();
if ($_POST[submit])
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
    if ($MYSQL->db_query("INSERT INTO commands VALUES('', '$_POST[service]', '$_POST[command]', '$_POST[access]')"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick added $_POST[service] $_POST[command] $_POST[access] to the command table.");
      echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Added Command</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully entered the command ($_POST[command]) for $_POST[service].<br>";
      echo "</td></tr></table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"addcommand\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Add a Command</td></tr>";
  echo "<tr><td id=\"label\">service</td><td id=\"field\">";
  select_admin_servicesbots("service");
  echo "</td></tr>";
  echo "<tr><td id=\"label\">command</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"command\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">access</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"access\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Add this Command\"></form>";
  echo "</table>";
}
page_end();
?>
