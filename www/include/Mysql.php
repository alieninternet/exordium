<?
class MySQL
{
  var $socket = "";
  var $dbname = "";
  var $server = "";
  var $dbuser = "";
  var $dbpass = "";
  var $dbtimeout = "15";

  function
  MySQL()
  {
    global $sql;
    $this->dbuser = $sql["username"];
    $this->dbpass = $sql["password"];
    $this->dbname = $sql["database"];
    $this->server = $sql["hostname"];
//    echo "MySQL Class Initialized.<br>dbuser: $this->dbuser<br>dbpass: $this->dbpass<br>dbname: $this->dbname<br>server: $this->server<br>\n";
  }

  // errors
  function
  error($text)
  {
    $err_no = mysql_errno();
    $err_msg = mysql_error();
    printf("<b>MySQL ERROR:</b> %s (%d):<br>%s<br>\n", $err_msg, $err_no, $text);
    exit();
  }

  function
  db_connect()
  {
    if ($fp = fsockopen($this->server, 3306, &$errno, &$errstr, $this->dbtimeout))
    {
      fclose($fp);
      $conn = @mysql_pconnect($this->server,$this->dbuser,$this->dbpass);
      if (!$conn)
        $this->error("Error connecting to the database server ($this->server).");
      if(!mysql_select_db($this->dbname, $conn))
        $this->error("Error selecting the database.");
      $this->socket = $conn;
    }
  }
  
  function
  db_select_db()
  {
    if (!mysql_select_db($this->dbname, $this->socket))
      $this->error("Error selecting the database.");
  }

  function
  db_close()
  {
    if($this->socket) mysql_close($this->socket);
    $this->socket = "";
  }

  function
  db_query($query)
  {
    if(!($r = @mysql_query($query)))
    {
      if(error_reporting() > 0)
      {
        echo "<hr>SQL error (".mysql_error().")<br>in (<pre>$query</pre>) <hr>";
      }
      $r = 'error';
    }
    return $r;
  }

  function
  db_db_query($database, $query)
  {
    if(!($r = @mysql_db_query($database, $query)))
    {
      if(error_reporting() > 0)
      {
        echo "<hr>SQL error (".mysql_error().")<br>in (<pre>$query</pre>) <hr>";
      }
      $r = 'error';
    }
    return $r;
  }

  function
  db_free($handle)
  {
    mysql_free_result($handle);
  }

  function
  db_fetch_array($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_fetch_array($handle);
  }

  function
  db_fetch_object($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_fetch_object($handle);
  }

  function
  db_fetch_row($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_fetch_row($handle);
  }

  function
  db_fetchfields($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_fetch_fields($handle);
  }

  function
  db_enum_values($table, $field)
  {
    global $PHP_SELF;
    $r = $this->db_query("DESCRIBE $table");
    while($row = mysql_fetch_array($r))
    {
      ereg('^([^ (]+)(\((.+)\))?([ ](.+))?$',$row['Type'],$fieldTypeSplit);
      $ret_fieldName = $row['Field'];
      $fieldType = $fieldTypeSplit[1];// eg 'int' for integer.
      $fieldFlags = $fieldTypeSplit[5]; // eg 'binary' or 'unsigned zerofill'.
      $fieldLen = $fieldTypeSplit[3]; // eg 11, or 'cheese','salmon' for enum.

      if (($fieldType=='enum' || $fieldType=='set') && ($ret_fieldName==$field) )
      {
        $fieldOptions = split("','",substr($fieldLen,1,-1));
        return $fieldOptions;
      }
    }
    return FALSE;
  }

  function
  db_numrows($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_num_rows($handle);
  }

  function
  db_numfields($handle)
  {
    if(!$handle || ($handle == 0))
    {
      return;
    }
    return mysql_num_fields($handle);
  }

  function
  db_get($query)
  {
    $h = $this->db_query($query);
    $ret = $this->db_fetch_object($h);
    $this->db_free($h);
    return $ret;
  }

  function
  db_insertid($h = 0)
  {
    return mysql_insert_id();
  }

  function
  db_begin()
  {
    list($mysql_version) = db_get("select version()");
    list($major,$minor,$pl) = split("\.",$mysql_version);
    if($major >= 3 && $minor >= 23 && $pl >= 34)
    {
      db_query('begin');
    }
  }

  function
  db_commit()
  {
    list($mysql_version) = db_get("select version()");
    list($major,$minor,$pl) = split("\.",$mysql_version);
    if($major >= 3 && $minor >= 23 && $pl >= 34)
    {
      db_query('commit');
    }
  }

  function
  db_rollback()
  {
    list($mysql_version) = db_get("select version()");
    list($major,$minor,$pl) = split("\.", $mysql_version);
    if($major >= 3 && $minor >= 23 && $pl >= 34)
    {
      db_query('rollback');
    }
  }
}
?>
