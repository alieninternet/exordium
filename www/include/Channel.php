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
  chan_mode_mk_checkboxes()
  {
    global $MYSQL;
    
    // pre-load the current channel vars
    if ($rm = $MYSQL->db_query("SELECT modes FROM chans WHERE id='$_POST[chan_id]'"))
    {
      if ($MYSQL->db_numrows($rm) > 0)
      {
        $row = $MYSQL->db_fetch_object($rm);
        list($modes, $key, $limit) = split(" ", $row->modes, 3);
        echo "modes: $modes key: $key limit: $limit<br>\n";
        $num = strlen($modes);
        $checked = array();
        for ($i=0; $i < $num; $i++)
        {
          if ($modes[$i] == "+")
            continue;
          $checked[$modes[$i]] = "$modes[$i]";
        }
      }
    }
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
          if (in_array($c->mode, $checked))
            $ckd = "checked";
          else
            $ckd = "";
          echo "<small><input id=\"q\" type=\"checkbox\" name=\"chanmode[$c->mode]\" $ckd> <b>$c->mode</b> $c->desc</small><br>";
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
    echo "</td></tr>\n";
    echo "<tr><td id=\"label\" colspan=\"2\">other channel settings</td></tr>";
    echo "<td id=\"field\" colspan=\"2\">";
    if (in_array("k", $checked))
      $ckd = "checked";
    else
      $ckd = "";
    echo "<input id=\"q\" type=\"checkbox\" name=\"chanmode[k]\" $ckd> set a channel key?<br>";
    echo "&nbsp;&nbsp;<input id=\"q\" type=\"text\" name=\"chankey\" size=\"10\" maxlength=\"25\" value=\"$key\"> <small>enter the key here</small><br><br>\n";
    if (in_array("l", $checked))
      $ckd = "checked";
    else
      $ckd = "";
    echo "<input id=\"q\" type=\"checkbox\" name=\"chanmode[l]\" $ckd> set a channel user limit?<br>";
    echo "&nbsp;&nbsp;<input id=\"q\" type=\"text\" name=\"chanlimit\" size=\"3\" maxlength=\"4\" value=\"$limit\"> <small>enter the limit here</small><br><br>\n";
  }
  
  function
  is_registered($name)
  {
    global $MYSQL;
    
    if ($r = $MYSQL->db_query("SELECT name FROM chans WHERE name='$name'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
        return 1;
      else
        return 0;
    }
    return 0;
  }

  function
  has_access($nickid, $chanid)
  {
    global $MYSQL;
    
    if ($r = $MYSQL->db_get("SELECT chanid, nickid FROM chanaccess WHERE nickid='$nickid' AND chanid='$chanid'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
        return 1;
      else
        return 0;
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
  
  function
  print_user_mod_chans()
  {
    global $MYSQL;
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $MYSQL->db_query("SELECT id, name FROM chans WHERE owner='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        echo "<p align=\"center\"><table cellspacing=\"0\" cellpadding=\"0\" border=\"0\">";
        while ($c = $MYSQL->db_fetch_object($r))
        {
          echo "<tr><td id=\"header\">$c->name</td></tr>";
          echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td align=\"center\"><input type=\"hidden\" name=\"action\" value=\"managechan\"><input type=\"hidden\" name=\"chan_id\" value=\"$c->id\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"manage your room\"><br></td></tr></form>\n";
        }
        echo "</table></p>";
      }
      else
        echo "no registered channels.<br>";
    }
  }
  
}
  
