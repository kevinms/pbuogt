#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "plugin.h"

/* load all plugins that are in the list */
int
plugin_dlload_all(list_t *list)
{
	struct plugin_t *plug;
	char buf[FILENAME_MAX] = PLUGIN_DIR;
	int i = 0;

	if(loaded_plugins == NULL)
		loaded_plugins = list_init();

	if(list == NULL)
		return -1;

	list_set_list_to_iterate(list);
	while(list_peek_link()!=NULL) {
		strcpy(buf,PLUGIN_DIR);
		strcat(buf,(char *)list_peek_item());

		plug = plugin_dlload(buf);

		if(plug) {
			list_add(loaded_plugins,plug);
			i++;
		}

		list_next_link();
	}

	return i;
}

/* Attempts to load a plugin */
struct plugin_t *
plugin_dlload(char *libname)
{
	char *error;

	/* create a plugin struct */
	struct plugin_t *plug;
	plug = (struct plugin_t *)malloc(sizeof(*plug));
	if(plug == NULL)
		return NULL;
	plug = (struct plugin_t *)memset(plug,0,sizeof(*plug));

	/* open lib */
	plug->handle = dlopen(libname,RTLD_LAZY);
	if(plug->handle == NULL) {
		fprintf(stderr,"%s dlopen failure: %s\n",libname,dlerror());
		plugin_free(plug);
		return NULL;
	}

	/* load symbols */
	plug->load = dlsym(plug->handle,"plugin_load");
	if((error = dlerror()) != NULL) {
		fprintf(stderr,"%s\n",error);
		dlclose(plug->handle);
		plugin_free(plug);
		return NULL;
	}
	
	plug->unload = dlsym(plug->handle,"plugin_unload");
	if((error = dlerror()) != NULL) {
		fprintf(stderr,"%s\n",error);
		dlclose(plug->handle);
		plugin_free(plug);
		return NULL;
	}
	
	plug->info = (struct plugin_info_t *)dlsym(plug->handle,"info");
	if((error = dlerror()) != NULL) {
		fprintf(stderr,"%s\n",error);
		dlclose(plug->handle);
		plugin_free(plug);
		return NULL;
	}
	
	/* check compatability */
	if(!IS_COMPATABILE(plug->info->MAJOR_V,plug->info->MINOR_V)) {
		fprintf(stderr,"Plugin is not compatible:  BOTVERSION:%d.%d, NEEDED:%d.%d\n",
				PBUOGT_MAJOR_VERSION,PBUOGT_MINOR_VERSION,
				plug->info->MAJOR_V,plug->info->MINOR_V);
		dlclose(plug->handle);
		plugin_free(plug);
		return NULL;
	}

	return plug;
}

int
plugin_dlunload_all()
{
	int i = 0;

	while(loaded_plugins->head) {
		plugin_dlunload((struct plugin_t *)loaded_plugins->head->item);
		i++;
	}

	return i;
}

void
plugin_dlunload(struct plugin_t *plug)
{
	(*(plug->unload))();
	dlclose(plug->handle);
	plugin_free(plug);

	list_del_item(loaded_plugins,plug);
}

void
plugin_free(struct plugin_t *plug)
{
	if(plug!=NULL)       free(plug);
}
