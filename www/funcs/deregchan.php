<?
$CHAN = new Channel();
page_start();
if ($_POST[submit])
{
  if (!$CHAN->isChanRegisteredID($_POST[chanid]))
  {
    if ($CHAN->deregisterChannel($_POST[chanid]))
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>room has been dropped</td></tr>";
      echo "<tr id=\"field\"><td>your room has successfully been dropped from services registration.<br>";
      echo "</td></tr></table>";
    }
  }
  else
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>Error dropping room.</td></tr>";
    echo "<tr id=\"field\"><td>unable to confirm this room as being registered with services.<br>\n";
    echo "</td></tr></table>";
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "  <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"deregchan\">";
  echo "  <input type=\"hidden\" name=\"chanid\" value=\"$_POST[chan_id]\">\n";
  echo "  <tr id=\"header\"><td>drop your room</td></tr>\n";
  echo "  <tr><td id=\"label\">are you completely sure you want to drop your room from registration with services?<br></td></tr>\n";
  echo "  <tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Yes, drop it\"></form>\n";
  echo "</table>";
}
page_end();
?>
