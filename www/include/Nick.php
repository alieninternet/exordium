<?
class Nick extends Mysql
{
  var $nick_id = "";

  function
  Nick()
  {
//    $this->nick_id = $this->get_nick_id();
  }

  function
  getNickName($nickid)
  {
    if ($rs = $this->db_query("SELECT nickname FROM nicks WHERE id='$nickid'"))
    {
      if ($this->db_numrows($rs) > 0)
      {
        $n = $this->db_fetch_object($rs);
        return $n->nickname;
      }
      else
        return FALSE;
    }
  }

  function
  isNickRegistered($nick)
  {
    $nick = strtolower($nick);
    if ($r = $this->db_query("SELECT id FROM nicks WHERE nickname='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        $row = $this->db_fetch_object($r);
        if ($row->id > 0)
          return $row->id;
        else
          return FALSE;
      }
      else
        return FALSE;

    }
  }

  function
  getRegisteredNickID($nick = "")
  {
    if (!$nick)
      $nick = $_SESSION[SESSION][nickname];
    if ($r = $this->db_query("SELECT id FROM nicks WHERE nickname='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
      {
        $n = $this->db_fetch_object($r);
        return $n->id;
      }
      else
        return 0;
    }
    return 0;
  }
  
  function
  updateLogin()
  {
    $host = $_SERVER[REMOTE_ADDR];
    $nick = $_SESSION[SESSION][nickname];
    $this->db_query("UPDATE nicks SET lastid=NOW(), lasthost='webuser@$host' WHERE nickname='$nick'");
  }
    
  
  function
  is_registered($nick)
  {
    if ($r = $this->db_query("SELECT nickname FROM nicks WHERE nickname='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
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
    if ($r = $this->db_query("SELECT nickname FROM nickspending WHERE nickname='$nick'"))
    {
      if ($this->db_numrows($r) > 0)
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
