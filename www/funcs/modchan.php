<?
$CHAN = new Channel();
page_start();
if ($_POST[submit])
{
  $c=0;
    $nick = $_SESSION[SESSION][nickname];
    if (is_array($_POST[chanmode]))
    {
      $mode_str = "+";
      foreach ($_POST[chanmode] as $k => $v)
      {
        $mode_str .= $k;
      }
      if ($_POST[chanmode][k])
        $mode_str .= " $_POST[chankey]";
      if ($_POST[chanmode][l])
        $mode_str .= " $_POST[chanlimit]";
    }
    $_POST[topic] = addslashes($_POST[topic]);
    $_POST[cdesc] = addslashes($_POST[cdesc]);
    $_POST[url] = addslashes($_POST[url]);
    $_POST[entrymsg] = addslashes($_POST[entrymsg]);
    $_POST[partmsg] = addslashes($_POST[partmsg]);
    $sql = "UPDATE chans SET topic='$_POST[topic]', modes='$mode_str', cdesc='$_POST[cdesc]', url='$_POST[url]', entrymsg='$_POST[entrymsg]', partmsg='$POST[partmsg]', clog='$_POST[clog]' WHERE id='$_POST[chan_id]'";
    if ($debug)
      echo $sql;
    if ($MYSQL->db_query($sql))
    {

      event_log("$nick modified the channel $_POST[name].");
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Successfully modified your room.</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully modified your room named <b>$_POST[name]</b><br>";
      echo "</td></tr></table>";
    }
}
else if ($_POST[chan_id])
{
  if ($r = $MYSQL->db_query("SELECT id, name, owner, topic, modes, cdesc, url, entrymsg, partmsg, clog FROM chans WHERE id='$_POST[chan_id]'"))
  {
    if ($MYSQL->db_numrows($r) > 0)
    {
      $c = $MYSQL->db_fetch_object($r);
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"modchan\"><input type=\"hidden\" name=\"chan_id\" value=\"$_POST[chan_id]\"><input type=\"hidden\" name=\"name\" value=\"$c->name\">\n";
      echo "<tr id=\"header\"><td colspan=\"2\">modify $c->name</td></tr>";
      echo "<tr><td id=\"label\">topic</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"topic\" size=\"25\" maxlength=\"255\" value=\"".stripslashes($c->topic)."\"/></td></tr>\n";
      echo "<tr><td id=\"label\">room description</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"cdesc\" size=\"25\" maxlength=\"255\"  value=\"".stripslashes($c->cdesc)."\"/></td></tr>\n";
      echo "<tr><td id=\"label\">room url</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"url\" size=\"25\" maxlength=\"255\" value=\"".stripslashes($c->url)."\"/></td></tr>\n";
      echo "<tr><td id=\"label\">room entry message</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"entrymsg\" size=\"25\" maxlength=\"255\" value=\"".stripslashes($c->entrymsg)."\"/></td></tr>\n";
      echo "<tr><td id=\"label\">room part message</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"partmsg\" size=\"25\" maxlength=\"255\" value=\"".stripslashes($c->partmsg)."\"/></td></tr>\n";
      echo "<tr><td id=\"label\" colspan=\"2\">set your channel modes</td></tr>";
      echo "<td id=\"field\" colspan=\"2\">";
      $CHAN->chan_mode_mk_checkboxes();
      if ($c->clog == "1")
        $ckd = "checked";
      else
        $ckd = "";
      echo "<input id=\"q\" type=\"checkbox\" name=\"clog\" value=\"1\" $ckd> enable services logging?<br>";
      echo "</td></td>\n";
      echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"modify this room\"></form>";
      echo "</table>";
    }
  }
}
page_end();
?>
