<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";

switch ($_POST[action])
{
  case "login":
    user_login();
    break;
  case "regnick":
    include "./funcs/$_POST[action].php";
    break;
  default:
    default_page();
    break;
}

function
default_page()
{
  global $MYSQL;
  page_start();
  echo "You do not appear to be logged in currently. <form method=\"POST\" action=\"$_SERVER[PHP_SELF]\">";
  echo "<input type=\"hidden\" name=\"action\" value=\"regnick\">If you have a registered nickname, please log-in above<br>or<br>\n";
  echo "<input id=\"submit\" type=\"submit\" value=\"click here\"><br>to register a nickname.";
  echo "</form>";
/*
  echo "<table cellspacing=\"2\" cellpadding=\"1\" border=\"0\" width=\"300\" id=\"login\">\n";
  echo "<form method=\"POST\" action=\"$_SERVER[PHP_SELF]\">";
  echo "<tr id=\"header\"><td colspan=\"2\">Log-in to PeopleChat</td></tr>";
  echo "<input type=\"hidden\" name=\"action\" value=\"login\" \>";
  echo "<tr><td id=\"label\">nickname</td><td><input id=\"q\" type=\"text\" name=\"nickname\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td id=\"label\">password</td><td><input id=\"q\" type=\"password\" name=\"password\" size=\"15\" maxlength=\"255\" /></td></tr>";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" value=\"Log In\"></td></tr>";
  echo "</form>";
  echo "<tr><td colspan=\"2\">&nbsp;</td></tr>\n";
  echo "<tr id=\"header\"><td colspan=\"2\">New to PeopleChat?</td></tr>";
  echo "<tr><td colspan=\"2\">&nbsp;</td></tr>\n";
  echo "<form method=\"POST\" action=\"$_SERVER[PHP_SELF]\">";
  echo "<input type=\"hidden\" name=\"action\" value=\"regnick\">\n";
  echo "<tr><td colspan=\"2\"><input id=\"submit\" type=\"submit\" value=\"Register a nickname\"></td></tr>";
  echo "</form>";
  echo "</table>";
  */
  page_end();
}

function
user_login()
{
  global $MYSQL, $NICK;
  if ($_POST[nickname] && $_POST[password])
  {
    $status = user_authenticate($_POST[nickname], $_POST[password]);

    // This means we had a good nick/pass pair.
    if ($status == 1)
    {
      session_start();
      session_register("SESSION");
      $_SESSION[SESSION] = array("nickname" => $_POST[nickname]);
      event_log("$_POST[nickname] successfully logged into web-interface.");
      $NICK->updateLogin();
      header("Location: /base.php");
      exit();
    }
    else
    {
      event_log("$_POST[nickname] failed logging into the web-interface.");
      header("Location: /error.php?e=$status");
      exit();
    }
  }
}
?>

