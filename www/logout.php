<?
include $_SERVER[DOCUMENT_ROOT]."/include/global.inc";
session_start();
$nick = $_SESSION[SESSION][nickname];
event_log("$nick successfully logged out.");
session_destroy();
header("Location: /");
?>
