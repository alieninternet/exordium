<?
page_start();
if ($_POST[submit])
{
  global $salt;
  $NICK = new Nick();
  $_POST[nickname] = strtolower($_POST[nickname]);
  $c=0;
  if (!$_POST[nickname])
    $err[$c++] = "You must specify a nickname to register.";
  if (!$_POST[email])
    $err[$c++] = "You must specify an e-mail address to send authorization to.";
  if (!is_email($_POST[email]))
    $err[$c++] = "You must specify a valid e-mail address.";
  if (!$_POST[password1])
    $err[$c++] = "You must specify a password in the first box.";
  if (!$_POST[password2])
    $err[$c++] = "You must specify a password in the second box.";
  if ($_POST[password1] != $_POST[password2])
    $err[$c++] = "You must specify passwords that match in both boxes.";
  if ($err > 0)
  {
    echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
    echo "<tr id=\"header\"><td colspan=\"2\">Error registering your nickname</td></tr>";
    echo "<tr><td id=\"field\"><br>";
 	  echo "<ul>";
	  for ($i=0;$i < count($err); $i++)
    {
		  echo "<li>$err[$i]</li>\n";
	  }
	  echo "</ul>";
	  echo "<a href=\"javascript:history.back(1)\">Click Here</a> to go back.<br>";
	  echo "</td></tr></table>";
  }
  else
  {
    if ($NICK->is_registered($_POST[nickname]))
    {
      event_log("$_POST[nickname] .");
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Error registering nickname.</td></tr>";
      echo "<tr id=\"field\"><td>The nickname <b>$_POST[nickname]</b> is already registered with services.<br>";
      echo "</td></tr></table>";
      exit;
    }
    $str = $_POST[nickname].$_POST[password1].$_POST[email].$salt;
    $hash = md5($str);
    echo "your hash is: $hash<br>\n";
    $email = "Hello,
              This is a sample email - Needs replacing with something much better
              In order to successfully register your nickname, you need to follow
              the following link.
              
              http://admin.peoplechat.org/confirm.php?hash=$hash
              
Thank You for using PeopleChat
              
              ";

    email_notify($_POST[email], "Your nickname registration", $email);
    $sql = "INSERT INTO nickspending VALUES('', '$_POST[nickname]', '$hash')";
    if ($debug)
      echo $sql;
    if ($MYSQL->db_query($sql))
    {

      event_log("$nick registered the nickname $_POST[nickname].");
      echo "<meta http-equiv=\"Refresh\" content=\"5;  URL=$_SERVER[PHP_SELF]\">\n";
      echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
      echo "<tr id=\"header\"><td>Successfully registered your nickname.</td></tr>";
      echo "<tr id=\"field\"><td>You have successfully started the registration process for the nickname: <b>$_POST[nickname]</b><br>";
      echo "However, to confirm your nickname you will be receiving an e-mail from PeopleChat Services, this e-mail will have a url link for you to click on.  At that point you will have to confirm your nickname and password once again to complete the registration process.<br>";
      echo "</td></tr></table>";
    }
  }
}
else
{
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"regnick\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Register a nickname</td></tr>";
  echo "<tr><td id=\"label\">nickname</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"nickname\" size=\"15\" maxlength=\"25\" /></td></tr>";
  echo "<tr><td id=\"label\">e-mail address</td><td id=\"field\"><input id=\"q\" type=\"text\" name=\"email\" size=\"25\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">password</td><td id=\"field\"><input id=\"q\" type=\"password\" name=\"password1\" size=\"15\" maxlength=\"25\" /></td></tr>";
  echo "<tr><td id=\"label\">password again</td><td id=\"field\"><input id=\"q\" type=\"password\" name=\"password2\" size=\"15\" maxlength=\"25\" /></td></tr>";

  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" name=\"submit\" value=\"Register this nickname\"></form>";
  echo "</table>";
}
page_end();
?>
