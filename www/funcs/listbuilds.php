<?
page_start();
if ($r = $MYSQL->db_query("SELECT id, build, LEFT(notes, 15) as notes FROM builds ORDER BY id"))
{
  if ($MYSQL->db_numrows($r) > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"5\">Current Builds and Information</td></tr>";
    echo "<tr id=\"header\"><td>&nbsp;</td><td>build</td><td>notes</td></tr>";
    while ($row = $MYSQL->db_fetch_object($r))
    {
      $notes = stripslashes($row->notes);
      echo "<tr><td>[<a href=\"$_SERVER[PHP_SELF]?action=modbuild&id=$row->id\">mod</a> | <a href=\"$_SERVER[PHP_SELF]?action=delbuild&id=$row->id\">del</a>]</td><td>$row->build</td><td>$notes</td></tr>\n";
    }
    echo "</table>";
  }
  else
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
    echo "<tr><td id=\"field\">There currently are no entries in this database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addbuild\">Click Here</a> to add build information.<br>";
    echo "</table>";
  }
}
page_end();
?>
