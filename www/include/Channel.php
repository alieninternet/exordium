<?
class Channel extends Mysql
{
  var $chanbot = "";

  function
  Channel()
  {
    
  }

  function
  chan_mode_checkboxes()
  {
    $column = 0;
    if ($r = $this->db_query("SELECT `id` , `mode` , `tdesc` , `html` FROM `chanmodes` ORDER BY id"))
    {
      if ($this->db_numrows($r) > 0)
      {
        echo "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\">";
        while ($c = $this->db_fetch_object($r))
        {
          if ($column == 0) { echo "<tr>\n"; }
          echo "<td>";
          $c->html = stripslashes($c->html);
          echo "<small><input id=\"q\" type=\"checkbox\" name=\"chanmode[$c->mode]\"> <b>$c->mode</b> $c->tdesc</small><br>";
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
    // pre-load the current channel vars
    if ($rm = $this->db_query("SELECT modes FROM chans WHERE id='$_POST[chan_id]'"))
    {
      if ($this->db_numrows($rm) > 0)
      {
        $row = $this->db_fetch_object($rm);
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
    if ($r = $this->db_query("SELECT `id` , `mode` , `tdesc` , `html` FROM `chanmodes` ORDER BY id"))
    {
      if ($this->db_numrows($r) > 0)
      {
        echo "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\">";
        while ($c = $this->db_fetch_object($r))
        {
          if ($column == 0) { echo "<tr>\n"; }
          echo "<td>";
          $c->html = stripslashes($c->html);
          if (in_array($c->mode, $checked))
            $ckd = "checked";
          else
            $ckd = "";
          echo "<small><input id=\"q\" type=\"checkbox\" name=\"chanmode[$c->mode]\" $ckd> <b>$c->mode</b> $c->tdesc</small><br>";
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
  options_chanaccess_levels($name, $selected = "FALSE")
  {
    if ($r = $this->db_query("SELECT id, level, ldesc FROM chanlevels WHERE level<500 ORDER BY level"))
    {
      if ($this->db_numrows($r) > 0)
      {
        while ($row = $this->db_fetch_object($r))
        {
          echo "<input type=\"radio\" name=\"$name\" value=\"$row->level\"";
          if ($row->level == $selected)
            echo " checked";
          echo "> <b>$row->level</b>&nbsp;$row->ldesc<br>\n";
        }
      }
    }
  }
  
  function
  listChannelUsers($chanid)
  {
    global $MYSQL, $NICK;
    if ($r = $this->db_query("SELECT chanid, nickid, access FROM chanaccess WHERE chanid='$chanid' ORDER BY access DESC"))
    {
      if ($this->db_numrows($r) > 0)
      {
        echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
        echo "<tr id=\"header\"><td colspan=\"5\">Current users with access to ".$this->getChanName($_POST[chan_id])."</td></tr>";
        echo "<tr id=\"header\"><td>&nbsp;</td><td>nickname</td><td>access</td></tr>";
        while ($row = $this->db_fetch_object($r))
        {
          echo "<tr><td>";
          if ($row->access == 500)
            echo "<b>channel owner</b>";
          else
            echo "[<a href=\"$_SERVER[PHP_SELF]?action=modchanuser&chanid=$row->chanid&nickid=$row->nickid\">mod</a> | <a href=\"$_SERVER[PHP_SELF]?action=delchanuser&chanid=$row->chanid&nickid=$row->nickid\">del</a>]";
          echo "</td><td>";
          echo $NICK->getNickName($row->nickid);
          echo "</td><td>$row->access</td></tr>\n";
        }
        echo "</table>";
      }
      else
      {
        echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
        echo "<tr id=\"header\"><td>No Current Entries</td></tr>";
        echo "<tr><td id=\"field\">There currently are no entries in this room's database.<br><br><a href=\"$_SERVER[PHP_SELF]?action=addchanuser&chanid=$_POST[chan_id]\">Click Here</a> to add a new user to your room.<br>";
        echo "</table>";
      }
    }
  }

  function
  getOnlineChanID($chan)
  {
    $name = strtolower($name);
    if ($r = $this->db_query("SELECT id FROM onlinechan WHERE name='$name'"))
    {
      if ($this->db_numrows($r) < 1)
      {
        return 0;
      }
      else
      {
        $row = $this->db_fetch_object($r);
        return $row->id;
      }
    }
  }

  function
  isChanRegistered($name)
  {
    if ($r = $this->db_query("SELECT id FROM chans WHERE name='$name'"))
    {
      if ($this->db_numrows($r) < 1)
        return FALSE;
      $row = $this->db_fetch_object($r);
      if ($row->id > 0)
        return TRUE;
      else
        return FALSE;
    }
    return 0;
  }

  function
  isChanRegisteredID($chanid)
  {
    if ($r = $this->db_query("SELECT id FROM chans WHERE id='$id'"))
    {
      if ($this->db_numrows($r) < 1)
        return FALSE;
      else
      {
        $row = $this->db_fetch_object($r);
        return $row->id;
      }
    }
    return FALSE;
  }

  function
  getChanAccess($chan, $nick)
  {
    $NICK = new Nick();
    $nickid = $NICK->getRegisteredNickID($nick);
    if (!is_numeric($chan))
      $chanid = $this->getChanID($chan);
    else
      $chanid = $chan;
      
    if ($r = $this->db_query("SELECT chanid, nickid, access FROM chanaccess WHERE nickid='$nickid' AND chanid='$chanid'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        $row = $this->db_fetch_object($r);
        return $row->access;
      }
      else
        return FALSE;
    }
    return FALSE;
  }

  function
  ifChanExists($name)
  {
    if ($r = $this->db_query("SELECT id FROM onlinechan WHERE name='$name'"))
    {
      if ($this->db_numrows($r) < 1)
        return FALSE;
      else
      {
        $row = $this->db_fetch_object($r);
        if ($row->id > 0)
          return TRUE;
        else
          return FALSE;
      }
    }
    return FALSE;
  }

  function
  getChanName($chanid)
  {
    if($r = $this->db_query("SELECT name FROM chans WHERE id='$chanid'"))
    {
      if ($this->db_numrows($r) < 1)
      {
        return FALSE;
      }
      else
      {
        $row = $this->db_fetch_object($r);
        return $row->name;
      }
    }
  }

  function
  getChanID($name)
  {
    $name = strtolower($name);
    if ($r = $this->db_query("SELECT id FROM chans WHERE name='$name'"))
    {
      if ($this->db_numrows($r) < 1)
      {
        return 0;
      }
      else
      {
        $row = $this->db_fetch_object($r);
        return $row->id;
      }
    }
  }


  function
  AddAccess($name, $nick, $level)
  {
    global $MYSQL, $NICK;
    $nickid = $NICK->getRegisteredNickID($nick);
    $chanid = $this->getChanID($name);
    if ($this->db_query("INSERT INTO chanaccess VALUES('$chanid', '$nickid', '$level')"))
      return TRUE;
    else
      return FALSE;
  }

  function
  DelAccess($name, $nick)  
  {
    $nickid = $NICK->getRegisteredNickID($nick);
    $chanid = $this->getChanID($name);
    $this->db_query("DELETE FROM chanaccess WHERE chanid='$chanid' AND nickid='$nickid'");
  }

  /* Get the channel owner for the given numerical registered chan id */
  function
  getChanOwner($chanid)
  {
    if ($chanid)
    {
      if ($r = $this->db_query("SELECT owner FROM chans WHERE id='$chanid'"))
      {
        if ($this->db_numrows($r) < 1)
        {
          return FALSE;
        }
        else
        {
          $row = $this->db_fetch_object($r);
          return $row->owner;
        }
      }
      else
      {
        return FALSE;
      }
    }  
  }

  /* Register a channel with the given owner */
  function
  registerChannel($name)
  {
    global $MYSQL, $debug;
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

    $sql = "INSERT INTO chans (id, name, owner, topic, modes, cdesc, url, entrymsg, partmsg, clog) VALUES('', '$name', '$nick', '$_POST[topic]', '$mode_str', '$_POST[cdesc]', '$_POST[url]', '$_POST[entrymsg]', '$_POST[partmsg]', '$_POST[clog]')";
    if ($debug)
      echo $sql;
    if ($this->db_query($sql))
    {
      event_log("$nick registered the channel $_POST[name].");
//      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Successfully registered your room.</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully registered your room named <b>$_POST[name]</b><br>";
      if ($this->AddAccess($name, $nick, "500"))
        echo "Added you as <b>owner</b> in the room's channel access database.<br>";
      else
        echo "Failure adding you as owner to the room's channel access database.<br>";
      echo "</td></tr></table>";
    }
    else
      return FALSE;
/*
don't know about setting the channel mode.. and getting it into services queue.

   services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " + name + " +r");
same with setting the topic..

   setTopic(name, String("This channel has just been registered by ")+owner);

   services.getDatabase().dbInsert("chanopts", "'','"+name.IRCtoLower()+"',1,1,0,0,0");
*/
  }

  function
  deregisterChannel($chanid)
  {
    if ($chanid)
    {
      $name = $this->getChanName($chanid);

      $this->db_query("DELETE FROM chans WHERE name='$name'");
      $this->db_query("DELETE FROM chanaccess WHERE chanid='$chanid'");
      $this->db_query("DELETE FROM chanopts WHERE name='$name'");
      $this->db_query("DELETE FROM chanbans WHERE chan='$chanid'");
      /*
      of course need to do this :D
      services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " +
        name + " -r");
      */
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  function
  has_chans()
  {
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $this->db_query("SELECT name FROM chans WHERE owner='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
        return 1;
      else
        return 0;
    }
    return 0;
  }

  function
  select_users_chans($name, $selected = "FALSE")
  {
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $this->db_query("SELECT id, name FROM chans WHERE owner='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        echo "<select name=\"$name\">\n";
        while ($c = $this->db_fetch_object($r))
        {
          echo "<option value=\"$c->id\"";
          if ($c->id == $selected)
            echo " selected";
          echo ">$c->name</option>\n";
        }
        echo "</select>\n";
      }
    }
  }
  
  function
  print_users_chans()
  {
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $this->db_query("SELECT name FROM chans WHERE owner='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        while ($c = $this->db_fetch_object($r))
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
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $this->db_query("SELECT id, name FROM chans WHERE owner='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        echo "<p align=\"center\"><table cellspacing=\"0\" cellpadding=\"0\" border=\"0\">";
        while ($c = $this->db_fetch_object($r))
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
?>
