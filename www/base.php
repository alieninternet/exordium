<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";
check_auth();

if ($_POST[action])
  $axion = $_POST[action];
else
  $axion = $_GET[action];
  
switch ($axion)
{
// Help
  case "addhelp":
    include "./funcs/$axion.php";
    break;
  case "modhelp":
    include "./funcs/$axion.php";
    break;
  case "delhelp":
    include "./funcs/$axion.php";
    break;
  case "listhelp":
    include "./funcs/$axion.php";
    break;
// Build
  case "addbuild":
    include "./funcs/$axion.php";
    break;
  case "modbuild":
    include "./funcs/$axion.php";
    break;
  case "delbuild":
    include "./funcs/$axion.php";
    break;
  case "listbuilds":
    include "./funcs/$axion.php";
    break;
// Commands
  case "addcommand":
    include "./funcs/$axion.php";
    break;
  case "modcommand":
    include "./funcs/$axion.php";
    break;
  case "delcommand":
    include "./funcs/$axion.php";
    break;
  case "listcommands":
    include "./funcs/$axion.php";
    break;
  case "listlog":
    include "./funcs/$axion.php";
    break;
// User Info
  case "moduserinfo":
    include "./funcs/$axion.php";
    break;
// Channel Activities
  case "listchan":
    include "./funcs/$axion.php";
    break;

  case "managechan":
    include "./funcs/$axion.php";
    break;
    
  case "modchan":
    include "./funcs/$axion.php";
    break;

  case "listchanusers":
    include "./funcs/$axion.php";
    break;

  case "addchanuser":
    include "./funcs/$axion.php";
    break;

  case "modchanuser":
    include "./funcs/$axion.php";
    break;

  case "delchanuser":
    include "./funcs/$axion.php";
    break;

  case "regchan":
    include "./funcs/$axion.php";
    break;

  case "deregchan":
    include "./funcs/$axion.php";
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
  echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
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
  echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
  echo "      <tr>";
  echo "        <td>\n";
  echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
  echo "            <tr id=\"header\"><td>Chat Room</td></tr>";
  $CHAN = new Channel();
  if ($CHAN->has_chans())
  {
    echo "<tr><td id=\"centerfield\">You currently own";
    echo $CHAN->print_user_mod_chans();
    echo "</td></tr>";
  }
  else
  {
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"regchan\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"Register a Room\"></td></tr></form>";
  }
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
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Logs</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listlog\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"View Log\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table><br>";
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
    echo "      <tr>";
    echo "        <td>\n";
    echo "          <table cellspacing=\"2\" cellpadding=\"2\" border=\"0\" width=\"200\" id=\"normtable\">\n";
    echo "            <tr id=\"header\"><td>Current Channels</td></tr>";
    echo "            <form action=\"$_SERVER[PHP_SELF]\" method=\"POST\"><tr><td id=\"centerfield\"><input type=\"hidden\" name=\"action\" value=\"listchan\"><input id=\"submit\" type=\"submit\" name=\"sub\" value=\"List Channels\"></td></tr></form>";
    echo "          </table>";
    echo "        </td>";
    echo "      </tr>";
    echo "    </table><br>";
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
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
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
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
    echo "    <table cellspacing=\"2\" cellpadding=\"0\" border=\"0\" width=\"200\" id=\"normtable\">";
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
