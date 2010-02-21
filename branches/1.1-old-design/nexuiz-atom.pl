#!/usr/bin/perl

$LAST=0;

sub check_atom ()
{
	$BUF=`echo -n "GET /repositories/revisions/nexuiz?format=atom HTTP/1.0\nHost: dev.alientrap.org\n\n" | nc -w 10 dev.alientrap.org 80`;
	#printf 'GET /repositories/revisions/nexuiz?format=atom HTTP/1.0\nHost: dev.alientrap.org\n\n' | nc -w 10 dev.alientrap.org 80
	
	$BUF=~s/\<\/entry\>.*//gsm;
	$BUF=~s/.*\<entry\>//gsm;
	#print $BUF;
	
	$BUF=~/(Revision \d+)/gm;
	$REV=$1;
	$REV=~/(\d+)/gm;
	$NUM=$1;
	#print $REV;
	
	$BUF=~/\<name\>(.*)\<\/name\>/gsm;
	$NAME=$1;
	#print $NAME;
	
	# Make substitutions for HTML special entities
	$BUF=~(s/&lt;/</g);
	$BUF=~(s/&gt;/>/g);
	
	$BUF=~/\<content type="html"\>(.*)\<\/content\>/gsm;
	$CONTENT=$1;
	$CONTENT=~(s/^\n//g);
	$CONTENT=~(s/\<p\>//g);
	$CONTENT=~(s/\<\/p\>//g);
	#print $CONTENT;
	
	if ($NUM > $LAST)
	{
		$LAST=$NUM;
		#print "SERV::irc.quakenet.org:PRIVMSG #stroggs.nexuiz :\x038Revision \x034$NUM\x033:\x0314 $CONTENT \x0311$NAME\r\n";
		print "SERV irc.quakenet.org PRIVMSG #kirby.nexuiz,#stroggs.nexuiz :\cc8Revision \cc5$NUM\cc9:\cc1 $CONTENT \cc10$NAME\r\n";
		
		print "SERV irc.quakenet.org PRIVMSG #kirby.nexuiz,#stroggs.nexuiz :http://dev.alientrap.org/repositories/revision/nexuiz/$NUM\r\n"
	}
}

while (1)
{
	&check_atom;
	sleep(5);
}
