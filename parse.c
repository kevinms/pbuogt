
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

	if (strcmp (msg->from, msg->bot_name) == 0) {
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
msg_t *parse_msg (char *buf)
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
