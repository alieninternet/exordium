<?
page_start();
if ($_POST[submit])
{
  unset($_POST[submit]);
  unset($_POST[action]);
  $sql = "SELECT id, nicknames, ident, hostname, service, did, ntext, cname FROM log WHERE ";
  foreach ($_POST as $n => $t)
  {
    if ($t == "")
      unset($_POST[$n]);
  }
  $count = count($_POST);
  if ($count == 0)
  {
    $nick = $_SESSION[SESSION][nickname];
    $sql .= "nicknames LIKE '%$nick' ";
  }
  $i=1;
  while (list($n, $v) = each($_POST))
  {
    $sql .= "$n LIKE '%$v%' ";
    if ($i != $count)
        $sql .= " AND ";
    $i++;
  }
  $sql .= " ORDER BY did";
  if ($debug) echo $sql;
  if ($r = $MYSQL->db_query($sql))
  {
    $numrows = $MYSQL->db_numrows($r);
    if ($numrows > 0)
    {
      echo "<table cellspacing=\"1\" cellpadding=\"1\" border=\"0\" bgcolor=\"#006699\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td colspan=\"7\">Current log Info ($numrows result(s))</td></tr>";
      echo "<tr id=\"field\"><td colspan=\"7\">";
      echo "</td></tr>";
      echo "<tr id=\"header\"><td>&nbsp;</td><td>nickname</td><td>ident</td><td>hostname</td><td>service</td><td>log entry</td><td>channel name</td></tr>";
      while ($row = $MYSQL->db_fetch_object($r))
      {
        echo "<tr id=\"field\"><td>$row->id</td><td>$row->nicknames</td><td>$row->ident</td><td>$row->hostname</td><td>$row->service</td><td>$row->ntext</td><td>$row->cname</td></tr>\n";
      }
      echo "</table><br><form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"listlog\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Search Again\"></form>";
    }
    else
    {
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
      echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br>";
      echo "</table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"listlog\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Search the log</td></tr>";
  echo "<tr><td id=\"label\">search in nickname</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"nicknames\" size=\"15\" maxlength=\"50\" /></td></tr>";
  echo "<tr><td id=\"label\">search in ident<p>(enter: 'web-user' for web based logins)</p></td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"ident\" size=\"10\" maxlength=\"50\" /></td></tr>";
  echo "<tr><td id=\"label\">search in hostname</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"hostname\" size=\"25\" maxlength=\"50\" /></td></tr>";
  echo "<tr><td id=\"label\">search in service</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"service\" size=\"15\" maxlength=\"50\" /></td></tr>";
  echo "<tr><td id=\"label\">search in channels</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"cname\" size=\"15\" maxlength=\"50\" /></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Search\"></form>";
  echo "</table>";
}
page_end();
?>
