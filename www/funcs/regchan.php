<?
$CHAN = new Channel();
page_start();
if ($_POST[submit])
{
  $c=0;
  if (!$_POST[name])
    $err[$c++] = "You must specify a name for your room.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error registering your room</td></tr>";
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
// add check to see if channel is registered (add it in the channel class)

    $name = strtolower($_POST[name]);
    if (!strchr($name, "#"))
      $name = "#".$name;
    if ($CHAN->isChanRegistered($name))
    {
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Error registering room.</td></tr>";
      echo "<tr id=\"field\"><td>A channel named <b>$_POST[name]</b> is already registered with services.<br>";
      echo "</td></tr></table>";
      exit;
    }
    if ($CHAN->has_chans())
    {
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Error registering room.</td></tr>";
      echo "<tr id=\"field\"><td>You already own a channel, and cannot register more.<br>";
      echo "</td></tr></table>";
      exit;
    }
    if ($CHAN->registerChannel($name))
    {
  
    }
    else
    {

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
  echo "<tr><td id=\"label\">room entry message</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"entrymsg\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">room part message</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"partmsg\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\" colspan=\"2\">set your channel modes</td></tr>";
  echo "<td id=\"field\" colspan=\"2\">";
  $CHAN->chan_mode_checkboxes();
  echo "</td></tr>";
  echo "<tr><td id=\"label\" colspan=\"2\">other channel settings</td></tr>";
  echo "<td id=\"field\" colspan=\"2\">";
  echo "<input id=\"q\" type=\"checkbox\" name=\"chanmode[k]\"> set a channel key?<br>";
  echo "&nbsp;&nbsp;<input id=\"q\" type=\"text\" name=\"chankey\" size=\"10\" maxlength=\"25\"> <small>enter the key here</small><br><br>\n";
  echo "<input id=\"q\" type=\"checkbox\" name=\"chanmode[l]\"> set a channel user limit?<br>";
  echo "&nbsp;&nbsp;<input id=\"q\" type=\"text\" name=\"chanlimit\" size=\"3\" maxlength=\"4\"> <small>enter the limit here</small><br><br>\n";
  echo "<input id=\"q\" type=\"checkbox\" name=\"clog\" value=\"1\"> enable services logging?<br>";
  echo "</td></tr>\n";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Register this room\"></form>";
  echo "</table>";
}
page_end();
?>
