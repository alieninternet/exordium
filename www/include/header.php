<?
header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          // HTTP/1.0
echo "
<html>
<head><title>$site[title]</title></head>
<link rel=\"stylesheet\" type=\"text/css\" href=\"/main.css\" />
<body>
<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"100%\">
  <tr>
    <td align=\"center\" id=\"nav\">
      <table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"100%\">
        <tr><td colspan=\"2\" class=\"logo\"><img src=\"/images/peoplechat.gif\" border=\"0\"></td></tr>
        <tr><td><a href=\"$_GLOBALS[PHP_SELF]\">Home</a></td></tr>
      </table>
    </td>
  </tr>
  <tr>
    <td align=\"center\"><br>\n";
if ($debug)
{
  echo "<br><hr width=\"500\"><br><pre>\n";
  echo "<br><br>\$_SESSION<br>";
  print_r($_SESSION);
  echo "<br><br>\$_GET<br>";
  print_r($_GET);
  echo "<br><br>\$_POST<br>";
  print_r($_POST);
  echo "<br><br>\$_SERVER<br>";
  print_r($_SERVER);
  echo "</pre><br><hr width=\"500\"><br>\n";
}


?>
