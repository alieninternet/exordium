<?
$CHAN = new Channel();
page_start();
if ($_POST[sub])
{
  $CHAN->listChannelUsers($_POST[chan_id]);
}
page_end();
?>
