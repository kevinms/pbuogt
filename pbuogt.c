#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>

const char *server = "irc.quakenet.org";
const char *nick = "pbuogt";
const char *login = "pbuogt";
const char *port = "6668";
//unsigned short port = 6668;

static char *channel = "#kirby.nexuiz";

void connect_to_network (const char *serv, const char *chan)
{
	
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	int sockfd;
	
	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(server, port, &hints, &res);
	
	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	// connect!
	connect(sockfd, res->ai_addr, res->ai_addrlen);
	
	int bytes_sent;
	bytes_sent = send(sockfd, "NICK pbuogt\r\n", strlen("NICK pbuogt\r\n"), 0);
	
	bytes_sent = send(sockfd, "USER pbuogt 8 * :pughAIr\r\n", strlen("USER pbuogt 8 * :pughAIr\r\n"), 0);

	/* Creating a FIFO Pipe for scripts to use.*/
	//int status;
	//status = mkfifo("/home/spock/pbuogt-pipe", S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	//printf ("MKFIFO_STATUS: %d\n", status);

	//FILE *pipe;
	//pipe = fopen( "~/pbuogt-pipe", "r" );

	char buf[2000];
	memset (buf, '\0', 2000);
	int i = 0;
	int TALK = 1;
	while ( recv(sockfd, buf, 2000, 0) != 0)
	{
		printf ("%s\n", buf);
		char *resp = NULL;
		
		if ((resp = strstr (buf, "PING")) != NULL)
		{
			resp[1] = 'O';
			send(sockfd, resp, strlen(resp), 0);
			printf ("SENDING: %s\n", resp);
		}
		if (strstr (buf, "221") != NULL)
		{
			i++;
			printf ("Now Connected:  ATTEMPT=%d\n", i);
			send(sockfd, "JOIN #stroggs.nexuiz,#kirby.nexuiz\r\n", strlen("JOIN #stroggs.nexuiz,#kirby.nexuiz\r\n"), 0);
		}
		
		if (strstr (buf, "TOGGLE-TALK") != NULL)
		{
			TALK = (TALK + 1) % 2;
			if (TALK)
				send(sockfd, "PRIVMSG #kirby.nexuiz :Speech enabled!\r\n", strlen("PRIVMSG #kirby.nexuiz :Speech enabled!\r\n"), 0);
			else
				send(sockfd, "PRIVMSG #kirby.nexuiz :Speech disabled!\r\n", strlen("PRIVMSG #kirby.nexuiz :Speech disabled!\r\n"), 0);
		}
		
		else if (TALK == 1) 
		{
		if (strstr (buf, ":AnnKittyKat") != NULL)
		{
			char *chk;
			if ((chk = strstr (buf, "PRIVMSG")) != NULL && strstr (chk, ":AnnKittyKat") != NULL)
			{
				send(sockfd, "PRIVMSG #kirby.nexuiz :AnnKittyKat FTW!\r\n", strlen("PRIVMSG #kirby.nexuiz :AnnKittyKat FTW!\r\n"), 0);
			}
		}
		if (strstr (buf, ":hi") != NULL || strstr (buf, ":hey") != NULL || strstr (buf, ":hello") != NULL || strstr (buf, ":hola") != NULL || strstr (buf, ":howdy") != NULL || strstr (buf, ":hewwo") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :Howdy!\r\n", strlen("PRIVMSG #kirby.nexuiz :Howdy!\r\n"), 0);
		}
		if (strstr (buf, ":dance:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :<(^^<)\r\n", strlen("PRIVMSG #kirby.nexuiz :<(^^<)\r\n"), 0);
			send(sockfd, "PRIVMSG #kirby.nexuiz :^(^^)^\r\n", strlen("PRIVMSG #kirby.nexuiz :^(^^)^\r\n"), 0);
			send(sockfd, "PRIVMSG #kirby.nexuiz :(>^^)>\r\n", strlen("PRIVMSG #kirby.nexuiz :(>^^)>\r\n"), 0);
		}
		if (strstr (buf, ":happy:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :=D\r\n", strlen("PRIVMSG #kirby.nexuiz :=D\r\n"), 0);
		}
		if (strstr (buf, ":mad:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :>:(\r\n", strlen("PRIVMSG #kirby.nexuiz :>:(\r\n"), 0);
		}
		if (strstr (buf, ":suprised:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :O_O\r\n", strlen("PRIVMSG #kirby.nexuiz :O_O\r\n"), 0);
		}
		if (strstr (buf, ":flip:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :.-.\r\n", strlen("PRIVMSG #kirby.nexuiz :.-.\r\n"), 0);
		}
		if (strstr (buf, ":surb ip:") != NULL)
		{
			send(sockfd, "PRIVMSG #kirby.nexuiz :99.12.194.205\r\n", strlen("PRIVMSG #kirby.nexuiz :99.12.194.205\r\n"), 0);
		}
		}
		memset (buf, '\0', 2000);
	}
	
	return 0;
}
