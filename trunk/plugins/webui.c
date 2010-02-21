#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../plugin.h"

struct plugin_info_t info = {
	PBUOGT_MAJOR_VERSION,
	PBUOGT_MINOR_VERSION,
	"webui",
	"0.1",
	"GPL3",
	"Kevin M. Smith",
	"cs.clemson.edu/~kevin4",
	"Enables a webui for graphically managing your bot.  Everything \
	 from statistics, managing block lists, peering with other clients,\
	 load/unload plugins, to general chanserv commands"
};

int
plugin_load(void)
{
	fprintf(stderr,"Loading webui plugin...\n");
	return 0;
}

int
plugin_unload(void)
{
	fprintf(stderr,"Unloading webui plugin...\n");
	return 0;
}
