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
