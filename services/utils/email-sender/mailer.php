#!/usr/local/bin/php -q
<?
$link = mysql_connect("localhost","root","be3tHov3n");
if($link==FALSE)
{
    echo "i'm a buggy piece of shit\r\n";
    return;
}
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
		mail($row[1],$row[2],$row[3],"From: PeopleChat Administration <admin@peoplechat.org>\r\n"."Reply-To: admin@peoplechat.org\r\n");
		$q = "DELETE FROM emails where id='$row[0]'";
		$r = mysql_query($q);
	        echo "Email successfully sent to $row[1]\r\n";
	}
?>

