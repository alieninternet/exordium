<?
header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          // HTTP/1.0
?>
<html><head>
<meta http-equiv="Content-Language" content="en-us">
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<link href="/new.css" type="text/css" rel="stylesheet" />
<title>People Chat</title>
</head>

<BODY topmargin="0" leftmargin="0" marginWIDTH="0" marginHEIGHT="0" rightmargin="0" bottommargin="0" bgcolor="#FFFFFF">
<font face="tahoma">
<table border="0" width="100%" id="table1" cellspacing="0" cellpadding="0" bgcolor="#CCCCCC">
	<tr>
		<td valign="top" align="center">
		<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table2">
			<tr>
				<td width="50%" height="34" background="http://www.peoplechat.org/images/row1/leftfill.png"></td>
				<td align="center" width="160" height="18">
				<img border="0" src="http://www.peoplechat.org/images/row1/leftside.png" width="160" height="34"></td>
				<td align="center" width="458" height="34">
				<img border="0" src="http://www.peoplechat.org/images/row1/middle.png" width="458" height="34"></td>
				<td align="center" width="160" height="34">
				<img border="0" src="http://www.peoplechat.org/images/row1/rightside.png" width="160" height="34"></td>
				<td width="50%" height="34" background="http://www.peoplechat.org/images/row1/rightfill.png"></td>
			</tr>
		</table>
		</td>
	</tr>
	<tr>
		<td valign="top" align="center">
		<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table3">
		</table>
		</td>
	</tr>
	<tr>
		<td valign="top" align="center">
		<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table9">
			<tr>
				<td width="100%" align="right" valign="bottom">
				<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table10">
					<tr>
						<td height="46" background="http://www.peoplechat.org/images/universal/background.png" valign="bottom">

						<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table11" height="100%">
							<tr>
								<td height="46">
								<img border="0" src="http://www.peoplechat.org/images/universal/logo.png" width="240" height="35"></td>
								<td width="100%" align="right" valign="bottom">

								<table border="0" cellpadding="0" id="table14">
									<tr>
<?
if (session_is_registered("SESSION"))
{
  echo "<td valign=\"top\">&nbsp;<small><b>logged in as</b>: ".$_SESSION[SESSION][nickname]."&nbsp;[<a href=\"/logout.php\">logout</a>]</small></td>";
}
else
{
  echo "<form method=\"POST\" action=\"http://admin.peoplechat.org/index.php\">";
  echo "<td><img border=\"0\" src=\"http://www.peoplechat.org/images/universal/login.png\" width=\"40\" height=\"18\"></td>";
  echo "<td valign=\"top\">";
  echo "<input type=\"hidden\" name=\"action\" value=\"login\" \>";
  echo "<input type=\"text\" name=\"nickname\" size=\"7\" maxlength=\"15\" style=\"border: 1 solid #0099CC\"></td>";
  echo "<td valign=\"top\"><input type=\"password\" name=\"password\" size=\"7\" maxlength=\"15\" style=\"border: 1 solid #0099CC\"></td>";
  echo "<td><input type=\"image\" src=\"http://www.peoplechat.org/images/universal/log_arrow.png\" border=\"0\" width=\"14\" height=\"14\"></td>";
  echo "</form>";
}										
?>
								
															</tr>
								</table>
								<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table12">
									<tr>
										<td></td>
									</tr>
									<tr>
										<td align="right" valign="bottom">
										<table border="0" cellspacing="0" cellpadding="0" height="17" id="table13">
											<tr>
												<td width="15" height="17" background="http://www.peoplechat.org/images/tabs/gray_far_left.png"></td>
												<td width="70" height="17" background="http://www.peoplechat.org/images/tabs/gray_back.png" align="center">
												<img border="0" src="http://www.peoplechat.org/images/tabs/text/main_down.png" width="68" height="15"></td>
								
                    		<td width="15" height="17" background="http://www.peoplechat.org/images/tabs/gray_right.png"></td>
												<td background="http://www.peoplechat.org/images/tabs/gray_back.png" width="70" height="17" align="center">
								
												<img border="0" src="http://www.peoplechat.org/images/tabs/text/about_down.png" width="68" height="15"></td>
												<td background="http://www.peoplechat.org/images/tabs/gray_right.png" width="15" height="17"></td>
												<td width="70" height="17" background="http://www.peoplechat.org/images/tabs/gray_back.png" align="center">
								
                				<img border="0" src="http://www.peoplechat.org/images/tabs/text/archives_down.png" width="68" height="15"></td>
										
											  <td width="15" height="17" background="http://www.peoplechat.org/images/tabs/yellow_mid_left.png"></td>
												<td width="70" height="17" background="http://www.peoplechat.org/images/tabs/yellow_back.png" align="center">
												<a href="/base.php"><img border="0" src="http://www.peoplechat.org/images/tabs/text/members_up.png" width="68" height="15"></a></td>
												<td width="15" height="17" background="http://www.peoplechat.org/images/tabs/yellow_mid_right.png"></td>
												<td background="http://www.peoplechat.org/images/tabs/gray_back.png" width="70" height="17" align="center">
												<img border="0" src="http://www.peoplechat.org/images/tabs/text/help_down.png" width="68" height="15"></td>
												<td background="http://www.peoplechat.org/images/tabs/gray_far_right.png" width="15" height="17"></td>
											</tr>
										</table>
										</td>
									</tr>
								</table>
								</td>
							</tr>
						</table>
						</td>
					</tr>
				</table>
				</td>
			</tr>
		</table>
		</td>
	</tr>
	<tr>
		<td valign="top" align="center" bgcolor="#CCFF33">
		&nbsp;</td>
	</tr>
	<tr>
		<td bgcolor="#FFFFFF" height="1" background="http://www.peoplechat.org/images/universal/divider_vert.png">
		<img border="0" src="http://www.peoplechat.org/images/universal/spacer.png" width="1" height="1"></td>
	</tr>
</table>
<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table6">
	<tr>
		<td width="5%" valign="top">
		<!-- MSTableType="layout" -->
		<table border="0" width="50%" cellspacing="0" cellpadding="0" id="table17">
			<tr>
				<td valign="top">
				<img border="0" src="http://www.peoplechat.org/images/universal/spacer.png" width="75" height="1"></td>
			</tr>
		</table>
		</td>
		<td valign="top">
		<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table7">
			<tr>
				<td width="1" background="http://www.peoplechat.org/images/universal/divider.png">
				<img border="0" src="http://www.peoplechat.org/images/universal/spacer.png" width="1" height="1"></td>
				<td valign="top">
				<table border="0" width="100%" cellspacing="0" cellpadding="0" id="table16">
					<tr>
						<td valign="top">
						<img border="0" src="http://www.peoplechat.org/images/universal/spacer.png" width="225" height="1"></td>
					</tr>
					<tr>
						<td valign="top" align="center"><br>
<?
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
