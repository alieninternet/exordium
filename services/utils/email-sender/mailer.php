#!/usr/local/bin/php -q
<?
$link = mysql_connect("mysql","services","");
mysql_select_db("services");
$query = "SELECT * from emails";
$result = mysql_query($query);
$num = mysql_num_rows($result);
if($num<1)
	{
		return;
	}
for ($count = 1; $row = mysql_fetch_row($result); ++$count)
	{
		mail($row[1],$row[2],$row[3],"From: IRCDome Services <admin@ircdome.org>\r\n"."Reply-To: admin@ircdome.org\r\n");
		$q = "DELETE FROM emails where id='$row[0]'";
		$r = mysql_query($q);
	        echo "Email successfully sent to $row[1]\r\n";
	}
?>

