<?
$CHAN = new Channel();
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
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"regchan\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Register a room</td></tr>";
  echo "<tr><td id=\"label\">room name</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"name\" size=\"15\" maxlength=\"25\" /></td></tr>";
  echo "<tr><td id=\"label\">topic</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"topic\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">room description</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"cdesc\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">room url</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"url\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">set your modes</td><td id=\"field\">";
  $CHAN->chan_mode_checkboxes();
  echo "</td></tr>";

  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Register this room\"></form>";
  echo "</table>";
}
page_end();
?>
