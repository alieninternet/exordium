<?
page_start();
if ($r = $MYSQL->db_query("SELECT id, name, owner, modes FROM chans ORDER BY id"))
{
  if ($MYSQL->db_numrows($r) > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"5\">Current Registered Rooms</td></tr>";
    echo "<tr id=\"header\"><td>room name</td><td>owner</td><td>modes</td></tr>";
    while ($row = $MYSQL->db_fetch_object($r))
    {
      echo "<tr><td>$row->name</td><td>$row->owner</td><td>$row->modes</td></tr>\n";
    }
    echo "</table>";
  }
  else
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
    echo "<tr><td id=\"field\">There currently are no entries in this database.<br>";
    echo "</table>";
  }
}
page_end();
?>
