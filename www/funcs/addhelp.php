<?
page_start();
if ($_POST[submit])
{
  $c=0;
  if (!$_POST[word])
    $err[$c++] = "You need to put in a command.";
  if (!$_POST[txt])
    $err[$c++] = "You need to enter some sort of help information.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error!! Adding User Help Files</td></tr>";
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
    $txt = addslashes($_POST[txt]);
    if ($MYSQL->db_query("INSERT INTO help VALUES('', '$_POST[service]', '$_POST[word]', '$_POST[parm]', '$txt', '$_POST[lang]')"))
    {
      $nick = $_SESSION[SESSION][nickname];
      event_log("$nick added $_POST[service] help $_POST[word] $_POST[parm] to the help table.");
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Added Help File</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully entered the help data for $_POST[service].<br>";
      echo "The command would look as such: /msg $_POST[service] help $_POST[word] $_POST[parm]<br>";
      echo "Displaying the following help:<br>";
      echo nl2br($_POST[txt]);
      echo "</td></tr></table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"addhelp\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Add User Help Files</td></tr>";
  echo "<tr><td id=\"label\">service</td><td id=\"field\">";
  select_servicesbots("service");
  echo "</td></tr>";
  echo "<tr><td id=\"label\">command (eg: <u>help</u>)</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"word\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">optional parameter (eg: <u>set</u>)</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"parm\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">language</td><td id=\"field\">";
  select_languages("lang");
  echo "</td></tr>";
  echo "<tr><td id=\"label\">help text<p>legend:<br>&nbsp;%b% = <b>bold</b><br>&nbsp;%u% = <u>underline</u><br>&nbsp;%r% = reverse</p></td><td id=\"field\"><textarea name=\"txt\" cols=\"40\" rows=\"10\"></textarea></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Add Help\"></form>";
  echo "</table>";
}
page_end();
?>
