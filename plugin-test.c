#include <stdio.h>
#include <dlfcn.h>

#include "plugin.h"

int
main(int argc, char **argv)
{
/*
	struct plugin_t *plug;
	plug = plugin_dlload("./plugins/webui.so");
	if(plug==NULL)
		return 1;

	(*(plug->load))();
	(*(plug->unload))();

	dlclose(plug->handle);
*/

	list_t *l = list_init();
	list_add(l,"webui.so");

	int i;
	i = plugin_dlload_all(l);

	fprintf(stderr,"Loaded %d plugins.\n",i);

	struct plugin_t *plug = (struct plugin_t *)loaded_plugins->head->item;
	(*(plug->load))();
	(*(plug->unload))();

	/* unload webui plugin */
	plugin_dlunload(plug);
	if(loaded_plugins->head == NULL)
		fprintf(stderr,"Head is NULL\n");

	list_del_item(loaded_plugins, loaded_plugins->head->item);

	return 0;
}
