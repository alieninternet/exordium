<?
class Channel
{
  var $chanbot = "";

  function
  Channel()
  {

  }

  function
  chan_mode_checkboxes()
  {
    global $MYSQL;

    $column = 0;
    if ($r = $MYSQL->db_query("SELECT `id` , `mode` , `desc` , `html` FROM `chanmodes` ORDER BY id"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        echo "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\">";
        while ($c = $MYSQL->db_fetch_object($r))
        {
          if ($column == 0) { echo "<tr>\n"; }
          echo "<td>";
          $c->html = stripslashes($c->html);
          echo "<small><input id=\"q\" type=\"checkbox\" name=\"chanmode[$c->mode]\"> <b>$c->mode</b> $c->desc</small><br>";
          echo "</td>";
          $column++;
          if ($column >= 2)
          {
            $column = 0;
            echo "</td>\n";
          }
        }
        echo "</table>\n";
      }
    }
  }
  
  function
  is_registered($name)
  {
    global $MYSQL;
    
    if ($r = $MYSQL->db_query("SELECT name FROM chans WHERE name='$name'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
        return 0;
      else
        return 1;
    }
    return 0;
  }

  function
  has_chans()
  {
    global $MYSQL;
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $MYSQL->db_query("SELECT name FROM chans WHERE owner='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
        return 1;
      else
        return 0;
    }
    return 0;
  }
  
  function
  print_users_chans()
  {
    global $MYSQL;
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $MYSQL->db_query("SELECT name FROM chans WHERE owner='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        while ($c = $MYSQL->db_fetch_object($r))
        {
          echo "$c->name<br>";
        }
      }
      else
        echo "no registered channels.<br>";
    }
  }
  
  
  
}
  
