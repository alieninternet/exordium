<?
page_start();
if ($r = $MYSQL->db_query("SELECT id, service, word, parm, lang FROM help ORDER BY service, word"))
{
  if ($MYSQL->db_numrows($r) > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"5\">Current User Help Files</td></tr>";
    echo "<tr id=\"header\"><td>&nbsp;</td><td>service</td><td>command</td><td>optional parm</td><td>language</td></tr>";
    while ($row = $MYSQL->db_fetch_object($r))
    {
      echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=modhelp&id=$row->id\">mod</a> | <a href=\"$_SERVER[PHP_SELF]?action=delhelp&id=$row->id\">del</a>]</td><td>$row->service</td><td>$row->word</td><td>$row->parm</td><td>$row->lang</td></tr>\n";
    }
    echo "</table>";
  }
  else
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
    echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addhelp\">Click Here</a> to add help entries.<br>";
    echo "</table>";
  }
}
page_end();
?>
