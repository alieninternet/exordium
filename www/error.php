<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";
switch($e)
{
  case -1:
    $message = "No Such User.";
    break;
  case 0:
    $message = "Invalid username and/or password.";
    break;
  case 2:
    $message = "Unauthorized Access.";
    break;
  default:
    $message = "An unspecified error has occured.";
    break;
}
page_start();
echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
echo "<tr id=\"header\"><td colspan=\"2\">Error</td></tr>";
echo "<tr><td align=\"center\">";
echo "<br><font size=\"+1\">$message</font><br><br>Please <a href=\"/\">log in</a> again.<br><br>\n";
echo "</td></tr></table>\n";
page_end();
?>
