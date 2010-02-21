#ifndef _COM_H_
#define _COM_H_

typedef struct irc_type
{
	pthread_t pt;
	int pt_id;
	
	char *server;
	char *nick[3];  /* 3 Attempts at joining the server */
	char *p_nick;
	char *login;
	char *port;
	char *real_name;
	char *channels;
	
	int sockfd;
	struct addrinfo hints;
	struct addrinfo *res;
} irc_t;

void *init_Opipe ();
void *init_Epipe ();
void *init_Spipe ();

void *init_connection (void *ircv);
irc_t *fill_irc (char *buf);
char *p_strcpy (char *str);

#endif /* !_COM_H_ */
