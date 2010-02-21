#include "utils.h"

int p_strsplit (char *buf, char **seg)
{
	char *x;
	int i = 0;
	seg = NULL;
	
	x = strtok (buf, "\n");
	while (x != NULL)
	{
		i++;
		seg = (char **)realloc ((char *)seg, sizeof (char *));
		seg[i-1] = x;
		x = strtok (NULL, "\n");
	}
	
	return i;
}

char *p_strcpy (char *str)
{
	char *new = malloc (sizeof (char) * strlen (str));
	strcpy (new, str);
	return new;
}

char *p_strncpy (char *str, int len)
{
	char *new = malloc (sizeof (char) * len);
	strncpy (new, str, len);
	return new;
}
