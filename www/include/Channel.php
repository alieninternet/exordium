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
    if ($r = $MYSQL->db_query("SELECT `id` , `mode` , `desc` , `html` FROM `chanmodes` ORDER BY id"))
    {
      if ($MYSQL->db_numrows($r) > 0)
      {
        while ($c = $MYSQL->db_fetch_object($r))
        {
          $c->html = stripslashes($c->html);
          echo "<input id=\"q\" type=\"checkbox\" name=\"chanmode[$c->mode]\"> <b>$c->mode</b> $c->desc<br>";
        }
      }
    }
  }
}
  
