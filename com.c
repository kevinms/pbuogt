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
#include <signal.h>

#include "com.h"
#include "list.h"
#include "utils.h"

#define BUFS 2048

list_t *serv_list;
int e_pipe, o_pipe;

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
 * and COM Handler to receive from \
 * OUT-FIFO */
void *init_Opipe ()
{
	printf ("init_Opipe()\n");
	/* Creating a FIFO Pipe for scripts to use.*/
	char *name = "./pbuogt-out";
	init_fifo (name);
	
	/* Open pipe for reading */
	//pipe = fopen(name, "r" );
	o_pipe = open(name, O_RDONLY);
	
	link_t *ln;
	irc_t *irc = NULL;
	size_t size;
	char *server_name, *request_type;
	char *ned_serv = "SERV";
	char *ned_conn = "CONN";
	char *ned_nick = "NICK";
	char *buf = (char *)malloc (sizeof (*buf) * BUFS);
	//char *buf_orig = buf;
	memset (buf, '\0', BUFS);
	int n;
	while ((n = read(o_pipe, buf, BUFS)) >= 0)
	{
		//printf ("N: %d\n", n);
		if (n != 0)
		{
		
		char *resp = NULL;
		char *tok = NULL;
		resp = strtok (buf, " ");
		while (resp != NULL)
		{
			printf ("<== %s", resp);
			
			//request_type = p_strcpy (resp); /* find what type of request is being sent */
			//resp = strtok (NULL, " ");
			//server_name = p_strcpy (resp);  /* find which network to send the packet to */
			
			/* I/O Handler is requesting a msg be sent to a server */
			if (strstr (resp, ned_serv) != NULL)
			{
				
				resp = strtok (NULL, " "); /* get network name */
				printf (" %s", resp);
				
				/* Find the struct holding info the about the network */
				ln = serv_list->head;
				while (ln != NULL && strcmp (((irc_t *)ln->item)->server, resp) != 0)
					ln = ln->next;
				
				resp = strtok (NULL, "\n"); /* get text to send */
				printf (" %s\n", resp);
				if (ln != NULL)
				{
					//printf("GOING TO SEND!!\n");
					irc = (irc_t *)ln->item;
					//printf ("sending irc->server: %s\n", irc->server);
					send(irc->sockfd, resp, strlen(resp), 0);
					//printf ("bites: %d\n", bites);
				}
			}
			/* I/O Handler is requesting a nick change on a server */
			else if (strstr (resp, ned_nick) != NULL)
			{
				resp = strtok (NULL, " "); /* get network name */
				
				/* Find the struct holding info the about the network */
				ln = serv_list->head;
				while (ln != NULL && strcmp (((irc_t *)ln->item)->server, resp) != 0)
					ln = ln->next;
				
				if (ln != NULL)
				{
					resp = strtok (NULL, "\r\n"); /* get nick */
					((irc_t *)ln->item)->p_nick = p_strcpy (resp);
				}
			}
			/* I/O Handler is requesting a connection be made to a server */
			else if (strstr (resp, ned_conn) != NULL)
			{
				
				resp = strtok (NULL, "\n"); /* get text */
				printf (" %s\n", resp);
				
				/* Connect to new irc network */
				irc = fill_irc (resp);
				
				/* make sure that we are not already connected to this network */
				printf ("%s\n", irc->server);
				ln = serv_list->head;
				while (ln != NULL && strcmp (((irc_t *)ln->item)->server, irc->server) != 0)
					ln = ln->next;
				
				if (ln == NULL)
				{
					irc->pt_id = pthread_create( &irc->pt, NULL, init_connection, (void *)irc);
					//ADD: free the irc struct!!
				}
				irc = NULL;
			}
			else
			{
				resp = strtok (NULL, "\n");
			}
			
			resp = strtok (NULL, " ");
		}
		
		}
		memset (buf, '\0', BUFS);
		//buf = buf_orig;
		
	}
	
	pthread_exit (NULL);
	
	return (NULL);
}

/* create a FIFO pipe for COM Handler to send things to
 * and I/O Handler to receive from 
 * EVENT-FIFO */
void *init_Epipe ()
{
	char *name = "./pbuogt-event";
	init_fifo (name);
	
	/* Open pipe for reading */
	
	//pipe = fopen(name, "w" );
	e_pipe = open(name, O_WRONLY);

	return (NULL);
}

void *init_connection (void *ircv)
{
	printf ("init_connection()\n");
	
	irc_t *irc = (irc_t *)ircv;
	list_add (serv_list, (void *)irc);
	
	char data[BUFS];
	int bytes_sent;
	
	// first, load up address structs with getaddrinfo():
	memset(&irc->hints, 0, sizeof (irc->hints));
	irc->hints.ai_family = AF_UNSPEC;
	irc->hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(irc->server, irc->port, &irc->hints, &irc->res);
	
	// make a socket:
	irc->sockfd = socket(irc->res->ai_family, irc->res->ai_socktype, irc->res->ai_protocol);
	
	// connect!
	connect(irc->sockfd, irc->res->ai_addr, irc->res->ai_addrlen);
	
	memset (data, '\0', BUFS);
	sprintf (data, "NICK %s\r\n", irc->nick[0]);
	printf ("%s\n", data);
	bytes_sent = send(irc->sockfd, data, strlen(data), 0);
	
	memset (data, '\0', BUFS);
	sprintf (data, "USER %s 8 * :%s\r\n", irc->nick[0], irc->real_name);
	bytes_sent = send(irc->sockfd, data, strlen(data), 0);
	
	memset (data, '\0', BUFS);
	char *resp = NULL;
	char *ping = NULL;
	char fdat[BUFS];
	memset (fdat, '\0', BUFS);
	while ( recv(irc->sockfd, data, BUFS, 0) != 0)
	{
		
		resp = strtok (data, "\n");
		while (resp != NULL)
		{
			printf ("==> %s\n", resp);
			if (resp[0] == ':')
			{
				sprintf (fdat, "%s %s %s\n", irc->server,irc->p_nick,resp);
				write (e_pipe, fdat, strlen(fdat));
				memset (fdat, '\0', BUFS);
			}
			//if ((ping = strstr (resp, "PING")) != NULL)
			else if (strncmp (resp, "PING", strlen ("PING")) == 0)
			{
				resp[1] = 'O';
				bytes_sent = send(irc->sockfd, resp, strlen(resp), 0);
				printf ("<== %s\n", resp);
			}
			else if (strncmp (resp, "NOTICE", strlen ("NOTICE")) == 0)
			{
				//write (o_pipe, "Received Notice!\n", strlen("Received Notice!\n"));
			}
			else if (strncmp (resp, "ERROR", strlen ("ERROR")) == 0)
			{
				//write (o_pipe, "Received Error!\n", strlen("Received Error!\n"));
			}
			
			resp = strtok (NULL, "\n");
		}
		resp = NULL;
		memset (data, '\0', BUFS);
		
	}
	printf("Connection to '%s' has ended!\n", irc->server);
	
	//ADD: remove_server ();
	//list_del_element (serv_list, (void *)irc);
	
	return (NULL);
}

irc_t *fill_irc (char *buf)
{
	printf ("fill_irc()\n");
	irc_t *irc = (irc_t *)malloc (sizeof (*irc));

	//ADD: Parse that buf, oh yeah!
	//DATA_ORDER: server port nick nick2 nick3 login real_name channels

	irc->server = p_strcpy (strtok (buf, " "));
	irc->port = p_strcpy (strtok (NULL, " "));
	irc->nick[0] = p_strcpy (strtok (NULL, " "));
	irc->p_nick = irc->nick[0];
	irc->nick[1] = p_strcpy (strtok (NULL, " "));
	irc->nick[2] = p_strcpy (strtok (NULL, " "));
	irc->login = p_strcpy (strtok (NULL, " "));
	irc->real_name = p_strcpy (strtok (NULL, " "));
	irc->channels = p_strcpy (strtok (NULL, " "));

	return irc;
}

int main(int argc, char *argv[])
{
	signal (SIGPIPE, SIG_IGN);
	
	/* Create a list to hold info for each irc network we connect to. */
	serv_list = list_init ();
	
	/* Init FIFO Pipes */
	pthread_t o_fifo, e_fifo;
	int o_id, e_id;
	o_id = pthread_create( &o_fifo, NULL, init_Opipe, NULL);
	e_id = pthread_create( &e_fifo, NULL, init_Epipe, NULL);
	
	pthread_join (o_fifo, NULL);
	pthread_join (e_fifo, NULL);
	
	return 0;
}
