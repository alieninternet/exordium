<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";
check_auth();
switch ($_POST[action])
{
// Help
  case "addhelp":
    include "./funcs/$_POST[action].php";
    break;
  case "modhelp":
    include "./funcs/$_POST[action].php";
    break;
  case "delhelp":
    include "./funcs/$_POST[action].php";
    break;
  case "listhelp":
    include "./funcs/$_POST[action].php";
    break;
// Build
  case "addbuild":
    include "./funcs/$_POST[action].php";
    break;
  case "modbuild":
    include "./funcs/$_POST[action].php";
    break;
  case "delbuild":
    include "./funcs/$_POST[action].php";
    break;
  case "listbuilds":
    include "./funcs/$_POST[action].php";
    break;
// Commands
  case "addcommand":
    include "./funcs/$_POST[action].php";
    break;
  case "modcommand":
    include "./funcs/$_POST[action].php";
    break;
  case "delcommand":
    include "./funcs/$_POST[action].php";
    break;
  case "listcommands":
    include "./funcs/$_POST[action].php";
    break;
  case "listlog":
    include "./funcs/$_POST[action].php";
    break;
// User Info
  case "moduserinfo":
    include "./funcs/$_POST[action].php";
    break;
// Channel Activities
  case "regchan":
    include "./funcs/$_POST[action].php";
    break;

// LogOut
  case "logout":
    header("Location: /logout.php");
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
  echo "<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"500\" id=\"normtable\">\n";
  echo "<form method=\"POST\" action=\"$_SERVER[PHP_SELF]\">";
  $nick = $_SESSION[SESSION][nickname];
  $addr = $_SERVER[REMOTE_ADDR];
  echo "<tr id=\"header\"><td colspan=\"3\">Welcome to PeopleChat $nick ($addr)</td></tr>";
  echo "<tr><td colspan=\"3\"><br></td></tr>";
  echo "<tr><td colspan=\"3\" id=\"header\">User Commands</td></tr>";
  echo "<tr><td colspan=\"3\"><br></td></tr>";
  echo "<tr>";
  echo "  <td>";
  echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
  echo "      <tr>";
  echo "        <td>\n";
  echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
  echo "            <tr id=\"header\"><td>Edit Your Information</td></tr>";
  echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"moduserinfo\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Edit My Info\"></td></tr></form>";
  echo "          </table>";
  echo "        </td>";
  echo "      </tr>";
  echo "    </table><br>";
  echo "  </td>";
  echo "  <td width=\"50\" bgcolor=\"#ffffff\">&nbsp;</td>";
  echo "  <td>";
// stuff
  echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
  echo "      <tr>";
  echo "        <td>\n";
  echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
  echo "            <tr id=\"header\"><td>Chat Room</td></tr>";
  echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"regchan\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Register a Room\"></td></tr></form>";
  echo "          </table>";
  echo "        </td>";
  echo "      </tr>";
  echo "    </table><br>";
  echo "  </td>";
  echo "</tr>";
  echo "<tr><td colspan=\"3\"><br></td></tr>";
  if (has_serv_access())
  {
    echo "<tr><td colspan=\"3\" id=\"header\">Services Admin Commands</td></tr>";
    echo "<tr><td colspan=\"3\"><br></td></tr>";
  }
  echo "<tr>";
  echo "  <td>";
  if (has_serv_access())
  {
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Logs</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listlog\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"View Log\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table><br>";
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Build Info Management</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listbuilds\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"List Builds\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"addbuild\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Add Build\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"modbuild\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Modify Build\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"delbuild\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Delete Build\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table><br>";
  }
  echo "  </td>";
  echo "  <td width=\"50\" bgcolor=\"#ffffff\">&nbsp;</td>";
  echo "  <td>";
  if (has_serv_access())
  {
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Command Management</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listcommands\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"List Commands\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"addcommand\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Add Command\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"modcommand\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Modify Command\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"delcommand\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Delete Command\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table><br>";
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" bgcolor=\"#006699\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Help File Management</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listhelp\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"List Help\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"addhelp\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Add Help\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"modhelp\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Modify Help\"></td></tr></form>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"delhelp\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Delete Help\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table>";
  }
  echo "  </td>";
  echo "</tr>";
  echo "<tr><td colspan=\"3\"><br></td></tr>";
  echo "<tr><td colspan=\"3\"><form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"logout\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Log Out\"></form></td></tr>";
  echo "</table>";
  page_end();
}
?>
