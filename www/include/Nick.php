<?
class Nick
{
  var $nick_id = "";

  function
  Nick()
  {
//    $this->nick_id = $this->get_nick_id();
  }

  function
  getRegisteredNickID()
  {
    global $MYSQL;
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $MYSQL->db_query("SELECT id FROM nicks WHERE nickname='$nick'"))
    {
      if ($MYSQL->db_numrows($r) < 1)
        return FALSE;
      else
      {
        $row = $MYSQL->db_fetch_row($r);
        return $row->id;
      }
    }
    return FALSE;
  }

  function
  isNickRegistered($nick)
  {
    global $MYSQL;
    $nick = strtolower($nick);
    if ($r = $MYSQL->db_query("SELECT id FROM nicks WHERE nickname='$nick'"))
    {
      if ($MYSQL->db_numrows($r) < 1)
        return FALSE;
      else
      {
        $row = $MYSQL->db_fetch_object($r);
        if ($row->id > 0)
          return TRUE;
        else
          return FALSE;
      }
    }
  }

  function
  get_nick_id()
  {
    global $MYSQL;
    $nick = $_SESSION[SESSION][nickname];
    if ($r = $MYSQL->db_query("SELECT id FROM nicks WHERE nickname='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        $n = $MYSQL->db_fetch_object($r);
        return $n->id;
      }
      else
        return 0;
    }
    return 0;
  }
  
  function
  is_registered($nick)
  {
    global $MYSQL;
    if ($r = $MYSQL->db_query("SELECT nickname FROM nicks WHERE nickname='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
    return 0;
  }
  
  function
  is_confirm($nick)
  {
    global $MYSQL;
    if ($r = $MYSQL->db_query("SELECT nickname FROM nickspending WHERE nickname='$nick'"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
    return 0;
  }
}
