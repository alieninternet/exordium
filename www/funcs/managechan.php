<?
$CHAN = new Channel();
page_start();
echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
echo "<tr id=\"header\"><td colspan=\"2\">manage your room</td></tr>";
echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\">set your channel description, topic, modes, etc.<br><br><input type=\"hidden\" name=\"action\" value=\"modchan\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"modify your room settings\"><br></td></tr></form>\n";
echo "<tr><td colspan=\"2\"><br></td></tr>";
echo "<tr id=\"header\"><td colspan=\"2\">manage your room's access</td></tr>";
echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\"><input type=\"hidden\" name=\"action\" value=\"addchanuser\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"add a user\"><br></td></tr></form>\n";
echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\"><input type=\"hidden\" name=\"action\" value=\"modchanuser\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"modify a user\"><br></td></tr></form>\n";
echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\"><input type=\"hidden\" name=\"action\" value=\"delchanuser\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"remove a user\"><br></td></tr></form>\n";
echo "</table>";
page_end();
?>
