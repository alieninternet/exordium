#!/usr/local/bin/php
<?
$link = mysql_connect("localhost","root","be3tHov3n");
mysql_select_db("services");
$now = time();
$lastdone = $now - 86400;
$fquery = "SELECT name from chanopts where clog=1";
$fres = mysql_query($fquery);
$fnum = mysql_num_rows($fres);
if($fnum<1)
{
    return;
}
for ($fcount = 1; $frow = mysql_fetch_row($fres); ++$fcount)
{
    $chan = $frow[0];
    $aquery = "SELECT owner from chans where name='$chan'";
    $ares = mysql_query($aquery);
    $arow = mysql_fetch_row($ares);
    $owner = $arow[0];
    $bquery = "SELECT email from nicks where nickname='$owner'";
    $bres = mysql_query($bquery);
    $brow = mysql_fetch_row($bres);
    $email = $brow[0];
    $cquery = "SELECT clog from chans where name='$chan'";
    $cres = mysql_query($cquery);
    $crow = mysql_fetch_row($cres);
    $lastlogged = $crow[0];
    $query = "select * from log where unix_timestamp(did) > $lastlogged AND cname='$chan'";
    $result = mysql_query($query);
    $num = mysql_num_rows($result);
    $txt = "Below are the channel logs for the channel $chan \r\n";
    $txt = "$txt pertaining to the dates $lastlogged upto $now\r\n\r\n\r\n";
    if($num<1)
	{
		return;
	}
    for ($count = 1; $row = mysql_fetch_row($result); ++$count)
	{

	    $id = $row[0];
	    $nicknames = $row[1];
	    $ident = $row[2];
	    $hostname = $row[3];
	    $service = $row[4];
	    $when = $row[5];
	    $what = $row[6];
	    $txt = "$txt $nicknames $ident $hostname $service $when $what\r\n";
	    
	}
    $txt = "$txt\r\n\r\n\r\nIf you have any questions regarding this log, feel free to pop into #help or email services@exordium.org\r\nKind Regards\r\nServices Administration";
    $newtxt = mysql_escape_string($txt);
    $newchan = mysql_escape_string($chan);
    $zquery = "INSERT into emails values ('','$email','[Exordium/Channel Log] for $newchan','$newtxt')";
    mysql_query($zquery) or die("$zquery fucked up");
    echo "Completed email report for $chan - sent to $owner @ $email\r\n";
    $query = "UPDATE chans set clog='$now' where name='$chan'";
    mysql_query($query) or die("Arghghghgh! Im dead DEAD DEAD I TELL YOU");
}


?>

