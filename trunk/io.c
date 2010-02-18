#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "io.h"
#include "list.h"
#include "utils.h"

#define BUFS 2048

int e_pipe, o_pipe, s_pipe;

/* Even though it 'aint' a tree =] */
char *tree[] = 
{
	"agamalama", "ACTION Cuts agama's toe for making me spam!",
	"flip", ".-.",
	"sad", "º_º",
	"happy", "=]",
	"suprised", "O_O",
	"dance", "<(^^<) ^(^^)^ (>^^)>",
	"\0"
} ;

list_t *ignore_list;
list_t *admin_list;


int init_fifo (char *name)
{
	int status;
	status = mkfifo(name, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	if (!status)
		return status;
	if (status != 0)
		fprintf (stderr, "mkfifo(%s, ...) error, pathname already exists!\n", name);
	
	return (status);
}

/* create a FIFO pipe for I/O Handler to send things to
 * and COM Handler to receive from 
 * OUT-FIFO */
void *init_Opipe ()
{
	printf ("init_Opipe()\n");
	char *name = "./pbuogt-out";
	
	/* Open pipe for reading */
	
	//pipe = fopen( "./pbuogt-out", "w" );
	o_pipe = open(name, O_WRONLY);
	
	char *test = "CONN irc.quakenet.org 6668 pbuogt guppie BarbieQ pbuogt pughAIr #kirby.nexuiz";
	write (o_pipe, test, strlen(test));

	return (NULL);
}

/* create a FIFO pipe for COM Handler to send things to
 * and I/O Handler to receive from \
 * EVENT-FIFO */
void *init_Epipe ()
{
	/* Creating a FIFO Pipe for scripts to use.*/
	char *name = "./pbuogt-event";
	
	/* Open pipe for reading */
	
	//pipe = fopen( "./pbuogt-event", "r" );
	e_pipe = open(name, O_RDONLY);
	
	
	//memset (req, '\0', BUFS);
	char *buf = (char *)malloc (sizeof (*buf) * BUFS);
	memset (buf, '\0', BUFS);
	msg_t *msg;
	int n;
	
	while ((n = read(e_pipe, buf, BUFS)) >= 0)
	{
		if (n != 0)
		{
			printf ("==> %s\n", buf);
			if (strstr (buf, "221") != NULL)
			{
				printf ("Now Connected:  ATTEMPT=NULL\n");
				write (o_pipe, "SERV irc.quakenet.org JOIN #stroggs.nexuiz\r\n", strlen("SERV irc.quakenet.org JOIN #stroggs.nexuiz\r\n"));
			}
			
			msg = fill_msg (buf);
			printf ("msg->type:%d\n", CMD_NICK);
			if (msg->type == CMD_PRIVMSG)
				handle_privmsg (msg, buf);
			if (msg->type == CMD_NICK)
				handle_nick (msg);
			//ADD: free_msg (msg);
			//printf (":%s:\n", msg->command);
		}
		memset (buf, '\0', BUFS);
	}
	return (NULL);
}

/* create a FIFO pipe for Scripts to send things to
 * and I/O Handler to receive from \
 * SCRIPT-FIFO */
void *init_Spipe ()
{
	char *name = "./pbuogt-script";
	init_fifo (name);
	
	/* Open pipe for reading */
	//pipe = fopen(name, "r" );
	s_pipe = open(name, O_RDONLY);
	
	char *buf = (char *)malloc (sizeof (*buf) * BUFS);
	memset (buf, '\0', BUFS);
	int n;
	while ((n = read(s_pipe, buf, BUFS)) >= 0)
	{
		
		if (n != 0)
		{
			printf ("%s\n", buf);
			write (o_pipe, buf, BUFS);
		}
		memset (buf, '\0', BUFS);
	}
	
	return (NULL);
}

void handle_privmsg (msg_t *msg, char *buf)
{
	char req[BUFS];
	char *pos;
	char *pos2;
	int TALK = 1;
	int i;
	
	if (TALK && !list_lookup(ignore_list, msg->prefix))
	{
		pos = strtok (msg->text, " ");
		if (pos != NULL && strcmp (pos, msg->bot_name) == 0)
		{
			printf ("moo\n");
			pos = strtok (NULL, " ");
			//if ( strstr (msg->prefix, "calhoun.resnet.clemson.edu") != NULL)
			if (list_lookup(admin_list, msg->prefix))
			{
				printf ("moo0\n");
				if (pos != NULL && strcmp (pos, "join") == 0)
				{
					pos = strtok (NULL, " ");
					sprintf (req, "SERV %s JOIN %s\r\n", msg->msg_servername, pos);
					write (o_pipe, req, strlen(req));
				}
				else if (pos != NULL && strcmp (pos, "part") == 0)
				{
					pos = strtok (NULL, " ");
					sprintf (req, "SERV %s PART %s\r\n", msg->msg_servername, pos);
					write (o_pipe, req, strlen(req));
				}
				else if (pos != NULL && strcmp (pos, "nick") == 0)
				{
					pos = strtok (NULL, " ");
					sprintf (req, "SERV %s NICK %s\r\n", msg->msg_servername, pos);
					write (o_pipe, req, strlen(req));
				}
				else if (pos != NULL && strcmp (pos, "tell") == 0)
				{
					pos = strtok (NULL, " ");
					pos2 = strtok (NULL, "\n");
					sprintf (req, "SERV %s PRIVMSG %s :%s\r\n", msg->msg_servername,pos,pos2);
					write (o_pipe, req, strlen(req));
				}
				else if (pos != NULL && strcmp (pos, "ignore") == 0)
				{
					pos = strtok (NULL, " ");
					list_add (ignore_list, (void *)p_strcpy (pos));
					//printf ("ignoring/%s/\n", pos);
					//sprintf (req, "SERV::irc.quakenet.org:<IGNORE>:%s\r\n", pos);
					sprintf (req, "SERV %s %s %s :[IGNORE] %s\r\n",msg->msg_servername,msg->command,msg->to,pos);
					write (o_pipe, req, strlen(req));
				}
				else if (pos != NULL && strcmp (pos, "ignore_list") == 0)
				{
					link_t *ln = ignore_list->head;
					while (ln != NULL)
					{
						sprintf (req, "SERV %s %s %s :[IGNORE] %s\r\n",msg->msg_servername,msg->command,msg->from,(char *)ln->item);
						write (o_pipe, req, strlen(req));
						ln = ln->next;
					}
				}
			}
			if (pos != NULL && strcmp (pos, "punch") == 0)
			{
				pos = strtok (NULL, " ");
				sprintf (req, "SERV %s %s %s :%cACTION punches %s%c\r\n",msg->msg_servername,msg->command,msg->to,'\x01',pos,'\x01');
				write (o_pipe, req, strlen(req));
			}
		}
		else if (pos != NULL && pos[0] == ':' && strstr (pos+1, ":") != NULL)
		{
			pos = strtok (pos, ":");
			i = search_match (tree, pos);
			if (i != -1)
			{
				sprintf (req, "SERV %s %s %s :%s\r\n",msg->msg_servername,msg->command, msg->to, tree[i+1]);
				write (o_pipe, req, strlen(req));
			}
		}
	}
}

void handle_nick (msg_t *msg)
{
	char str[512];
	printf ("[%s]\n", msg->from);
	printf ("[%s]\n", msg->bot_name);
	if (strcmp (msg->from, msg->bot_name) == 0)
	{
		sprintf (str, "NICK %s %s\n", msg->msg_servername,msg->p_nick);
		write (o_pipe, str, strlen(str));
	}
}

void free_msg (msg_t *msg)
{
	/*
	if (msg->sender != NULL)
	{
		free (msg->sender);
		msg->sender = NULL;
	}
	*/
	if (msg->prefix != NULL)
	{
		free (msg->prefix);
		msg->prefix = NULL;
	}
	if (msg->command != NULL)
	{
		printf (":%s:\n", msg->command);
		free (msg->command);
		msg->command = NULL;
	}
	if (msg->to != NULL)
	{
		free (msg->to);
		msg->to = NULL;
	}
	if (msg->text != NULL)
	{
		free (msg->text);
		msg->text = NULL;
	}
	
	if (msg != NULL)
	{
		free (msg);
		msg = NULL;
	}
}

/* Parse incoming message and fill up the msg struct. */
msg_t *fill_msg (char *buf)
{
	msg_t *msg = (msg_t *)malloc (sizeof (*msg));
	memset (msg, '\0', sizeof (*msg));
	char *tok;
	//:pughar!~blackice@130-127-79-243.calhoun.resnet.clemson.edu NICK :pughar|coding
	//:pughar!~blackice@130-127-79-243.calhoun.resnet.clemson.edu PRIVMSG #kirby.nexuiz :100% uptime
	
	/* get server the message was received from */
	tok = strtok (buf, " ");
	msg->msg_servername = p_strcpy (tok);
	
	/* get the name of the bot on the server the msg is from */
	tok = strtok (NULL, " ");
	printf ("TOK: %s\n", tok);
	msg->bot_name = p_strcpy (tok);
	
	/* get the prefix/from */
	tok = strtok (NULL, " ");
	size_t s = strcspn (tok+1, "! ");
	msg->from = p_strncpy (tok+1, s);
	msg->prefix = p_strcpy (tok+1+s);
	
	/* get the command and parse in relation */
	tok = strtok (NULL, " ");
	msg->command = p_strcpy (tok);
	if (strcmp (msg->command, "PRIVMSG") == 0)
	{
		msg->type = CMD_PRIVMSG;
		parse_privmsg (msg, tok+strlen(tok)+1);
	}
	else if (strcmp (msg->command, "NICK") == 0)
	{
		msg->type = CMD_NICK;
		parse_nick (msg, tok+strlen(tok)+1);
	}
	
	return msg;
}

void parse_privmsg (msg_t *msg, char *par)
{
	char *tok;
	
	tok = strtok (par, " ");
	if (tok[0] == '#')
	{
		msg->channel = p_strcpy (tok);
		msg->to = msg->channel;
	}
	else
	{
		msg->nick = p_strcpy (tok);
		msg->to = msg->from;
	}
	tok = strtok (NULL, "\r");
	msg->text = p_strcpy (tok+1);
}

void parse_nick (msg_t *msg, char *par)
{
	//:pug|pthread!~blackice@130-127-126-47.calhoun.resnet.clemson.edu NICK :pughar|inet
	char *tok;
	tok = strtok (par, "\r\n");
	printf ("TOK: %s\n", tok+1);
	
	msg->p_nick = p_strcpy (tok+1);
}

int main(int argc, char *argv[])
{
	ignore_list = list_init ();
	admin_list = list_init ();
	
	list_add (admin_list, (void *)p_strcpy ("calhoun.resnet.clemson.edu"));
	list_add (admin_list, (void *)p_strcpy ("triad.res.rr.com"));
	list_add (admin_list, (void *)p_strcpy ("hsd1.ca.comcast.net"));
	list_add (admin_list, (void *)p_strcpy ("Surb.users.quakenet.org"));
	list_add (admin_list, (void *)p_strcpy ("130.127."));
	
	
	/* Init FIFO Pipes */
	pthread_t o_fifo, e_fifo, s_fifo;
	int o_id, e_id, s_id;
	o_id = pthread_create( &o_fifo, NULL, init_Opipe, NULL);
	e_id = pthread_create( &e_fifo, NULL, init_Epipe, NULL);
	s_id = pthread_create( &s_fifo, NULL, init_Spipe, NULL);
	
	pthread_join (o_fifo, NULL);
	pthread_join (e_fifo, NULL);
	pthread_join (s_fifo, NULL);
	
	return 0;
}
