#!/usr/local/bin/php -q
<?
$mnick = "cloneymaster";
$uplink = "172.18.200.5";
$uport = 6667;
$chan = "#Exordium";
$choice = 0;
$max = 700;
$curr = 1;

function make_seed() {
    list($usec,$sec) = explode(' ', microtime());
    return (float) $sec + ((float) $usec * 100000);
}
srand(make_seed());
echo "Initialised rand() with seed of make_seed()\r\n";

function register($client,$sock)
{
    fputs($sock,"NICK $client\r\n");
    fputs($sock,"USER $client $client $client $client\r\n");
    sleep(1);
    fputs($sock,":$client JOIN #Exordium\r\n");
}

function msg($client,$sock,$chan,$txt)
{
    fputs($sock,":$client PRIVMSG $chan :$txt\r\n");
    echo "msg(): $client PRIVMSG $chan :$txt\r\n";
}

function dojoin($client,$sock,$chan)
{
    fputs($sock,":$client JOIN $chan\r\n");
    echo "Sending: :$client JOIN $chan\r\n";
}
//6590 for asc

$fp = fsockopen($uplink,$uport,$errno,$errstr,30);
stream_set_blocking($fp,FALSE);
if(!fp) {
    echo "Error!! $errstr ($errno)\n";
} else {
    register($mnick,$fp);
    msg($mnick,$fp,$chan,"Giggle! What Shall i Do?");
    while (!feof($fp)) {
	$goaway = fgets($fp,128);
	usleep(10000);
//	sleep(1);
	$choice = rand(1,5);
	//Firstly iterate over our clients and fgets their sockets
	$i = 1;
	do 
	{
	    fgets($clients[$i],128);
	    $i++;
	} while ($i<$curr);
	if($choice=="1" || $choice=="6")
	{
	    //Spew a new clone!
	    if($curr>$max)
	    {
		//msg($mnick,$fp,$chan,"Spawn: No spawn reached limit!");
	    }
	    else
	    {
		//msg($mnick,$fp,$chan,"Performing asexual reproduction!");
		$clients[$curr] = fsockopen($uplink,$uport,$errno,$errstr,30);
		stream_set_blocking($clients[$curr],FALSE);
		if(!$clients[$curr])
		{
		    msg($mnick,$fp,$chan,"Failed! :( $errno $errstr");
		}
		else
		{
		    register("clone$curr",$clients[$curr]);
		    $curr++;
		}
	    }
	}
	if($choice=="2")
	{
	    //Random joiny thing :(
	    $todo = rand(1,$curr-1);
	    $fr = "#";
	    $fr1 = chr(rand(65,90));
	    $fr2 = chr(rand(65,90));
	    $fr3 = chr(rand(65,90));
	    $fr4 = chr(rand(65,90));
	    $destc = "$fr$fr1$fr2$fr3$fr4";
	    msg($mnick,$fp,$chan,"I pick you, Mr clone$todo goto #$destc!!!");
	    dojoin("clone$todo",$clients[$todo],$destc);
	}
	
	if($choice=="3")
	{
	    //Random talky thingy :(
	    $todo = rand(1,$curr-1);
            $fr1 = chr(rand(65,90));
	    $fr2 = chr(rand(65,90));
	    $fr3 = chr(rand(65,90));
	    $fr4 = chr(rand(65,90));
	    $talk = "$fr1$fr2$fr3$fr4";
	    $talk = "$talk $talk $talk $talk";
	    msg($mnick,$fp,$chan,"$clone$todo, speakth now!");
	    msg("clone$todo",$clients[$todo],$chan,$talk);
	}
	    
	else
	{
/*	    msg($mnick,$fp,$chan,"Nothing to do for $choice!!"); */
	}
    }
    echo "dEad? :D";
    fclose($fd);
}

?>

