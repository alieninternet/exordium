<?
page_start();
if ($_POST[submit])
{
  $nick = $_SESSION[SESSION][nickname];
  if ($MYSQL->db_query("UPDATE nicks SET email='$_POST[email]', url='$_POST[url]', lang='$_POST[lang]', icq='$_POST[icq]', msn='$_POST[msn]', yahoo='$_POST[yahoo]', aim='$_POST[aim]', privmsg='$_POST[privmsg]', deopaway='$_POST[deopaway]', modnick='$_POST[modnick]' WHERE nickname='$nick'"))
  {
    $nick = $_SESSION[SESSION][nickname];
    event_log("$nick modified $_POST[email] $_POST[url] in the nicks table.");
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>Updated Help File</td></tr>";
    echo "<tr id=\"field\"><td>The modifications you have submitted have been updated in the database.<br><br>You will now be redirected.</td></tr>";
    echo "<meta http-equiv=\"Refresh\" content=\"2;  URL=$_SERVER[PHP_SELF]\">\n";
  }
}
else
{
  $nick = $_SESSION[SESSION][nickname];
  $n = $MYSQL->db_get("SELECT id, nickname, email, registered, lastid, lasthost, privmsg, lang, icq, msn, url, deopaway, yahoo, aim, modnick FROM nicks WHERE nickname='$nick'");
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"moduserinfo\"><input type=\"hidden\" name=\"id\" value=\"$_GET[id]\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Modify User Information</td></tr>\n";
  echo "<tr><td id=\"label\">nickname</td><td id=\"field\">$n->nickname</td></tr>\n";
  echo "<tr><td id=\"label\">email</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"email\" size=\"15\" maxlength=\"15\" value=\"$n->email\"/></td></tr>\n";
  echo "<tr><td id=\"label\">url</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"url\" size=\"25\" maxlength=\"35\" value=\"$n->url\"/></td></tr>\n";
  echo "<tr><td id=\"label\">language</td><td id=\"field\">";select_languages("lang", $c->lang);echo "</td></tr>";

  echo "<tr id=\"greyrow\"><td colspan=\"2\" id=\"label\">&middot;Nickname Toggles</td></tr>\n";
  if ($n->privmsg == "1")
  {
    $pm_y = "checked";
    $pm_n = "";
  }
  else
  {
    $pm_y = "";
    $pm_n = "checked";
  }
  echo "<tr><td id=\"label\">use privmsg</td><td id=\"field\"><input type=\"radio\" name=\"privmsg\" value=\"1\" $pm_y> Yes <input type=\"radio\" name=\"privmsg\" value=\"0\" $pm_n>No</td></tr>";
  if ($n->deopaway == "1")
  {
    $doa_y = "checked";
    $doa_n = "";
  }
  else
  {
    $doa_y = "";
    $doa_n = "checked";
  }
  echo "<tr><td id=\"label\">de-op when marked away? (-o+v)</td><td id=\"field\"><input type=\"radio\" name=\"deopaway\" value=\"1\" $doa_y> Yes <input type=\"radio\" name=\"deopaway\" value=\"0\" $doa_n>No</td></tr>";
  if ($n->modnick == "1")
  {
    $mn_y = "checked";
    $mn_n = "";
  }
  else
  {
    $mn_y = "";
    $mn_n = "checked";
  }
  echo "<tr><td id=\"label\">force nickname identification</td><td id=\"field\"><input type=\"radio\" name=\"modnick\" value=\"1\" $mn_y> Yes <input type=\"radio\" name=\"modnick\" value=\"0\" $mn_n>No</td></tr>";
  echo "<tr id=\"greyrow\"><td colspan=\"2\" id=\"label\">&middot;Instant Messengers</td></tr>\n";
  echo "<tr><td id=\"label\">icq</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"icq\" size=\"15\" maxlength=\"35\" value=\"$n->icq\"/></td></tr>\n";
  echo "<tr><td id=\"label\">msn</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"msn\" size=\"15\" maxlength=\"35\" value=\"$n->msn\"/></td></tr>\n";
  echo "<tr><td id=\"label\">yahoo</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"yahoo\" size=\"15\" maxlength=\"35\" value=\"$n->yahoo\"/></td></tr>\n";
  echo "<tr><td id=\"label\">aim</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"aim\" size=\"15\" maxlength=\"35\" value=\"$n->aim\"/></td></tr>\n";

  echo "<tr id=\"greyrow\"><td colspan=\"2\" id=\"label\">&middot;Nickname Information</td></tr>\n";
  echo "<tr><td id=\"label\">date registered</td><td id=\"field\">$n->registered</td></tr>\n";
  echo "<tr><td id=\"label\">last identification</td><td id=\"field\">$n->lastid</td></tr>\n";
  echo "<tr><td id=\"label\">last host</td><td id=\"field\">$n->lasthost</td></tr>\n";
  echo "<tr><td id=\"label\">last quit msg</td><td id=\"field\">$n->quitmsg</td></tr>\n";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Modify Your Info\"></td></tr></form>";
  echo "</table>";
}
page_end();
?>
