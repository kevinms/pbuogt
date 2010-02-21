#ifndef _IO_H_
#define _IO_H_

//:Surb!~surb@Surb.users.quakenet.org PRIVMSG #kirby.nexuiz :heh

typedef struct msg_type
{
	/* ****************** *\
	 * Message structure  *
	\* ****************** */
	// <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>

	char *prefix;
	char *command; // <letter> { <letter> } | <number> <number> <number>
	char *params;
	
	/* <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ] */
	char *from; // <servername> | <nick>
	char *user; // [ '!' <user> ]
	char *host; // [ '@' <host> ]
	
	/* params */
	char *target;
	char *to;
	char *channel;
	char *servername;
	char *p_host;
	char *nick;
	char *mask;
	char *chstring;
	
	char *sender;
	char *text;
	int type;
	char *bot_name;
	char *p_nick;
	
	// COM adds this to the beginning of the 
	// msg to denote what server it came from
	char *msg_servername;
} msg_t;

enum 
{
	CMD_PRIVMSG = 0,
	CMD_NICK,
	CMD_MODE,
} ;

int init_fifo (char *name);
void *init_Opipe ();
void *init_Epipe ();
void *init_Spipe ();

msg_t *fill_msg (char *buf);
void free_msg (msg_t *msg);
void handle_privmsg (msg_t *msg, char *buf);
void handle_nick (msg_t *msg);
void parse_privmsg (msg_t *msg, char *par);
void parse_nick (msg_t *msg, char *par);

#endif /* !_IO_H_ */
