<?
$CHAN = new Channel();
page_start();
echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
echo "<tr id=\"header\"><td colspan=\"2\">manage your room</td></tr>";
echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\">set your channel description, topic, modes, etc.<br><br><input type=\"hidden\" name=\"action\" value=\"modchan\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"modify your room settings\"><br></td></tr></form>\n";
echo "</table>";
page_end();
?>
