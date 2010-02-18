#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "version.h"
#include "list.h"

struct plugin_t {
	void *handle;
	int (*load)(void);
	int (*unload)(void);

	struct plugin_info_t *info;
};

struct plugin_info_t {
	unsigned int MAJOR_V;
	unsigned int MINOR_V;
	char *name;          /* name of plugin */
	char *version;       /* what version the plugin is at */
	char *license;       /* the plugin license */
	char *author;        /* author's name */
	char *homepage;      /* author|plugin's homepage */
	char *description;   /* brief description */
};

#define p_strcpy(dst, src) \
	dst = (char *)malloc(sizeof(src)); \
	strcpy(dst,src);

#define PLUGIN_DIR "./plugins/"

/* Globals */
list_t *loaded_plugins;

/* Manage plugins */
int plugin_dlload_all(list_t *list);
struct plugin_t *plugin_dlload(char *libname);
void plugin_dlunload(struct plugin_t *plug);
void plugin_free(struct plugin_t *plug);

/* Available for use in plugins */
//TODO: Design API for plugins to use.
void pbuogt_get_msg();
void pbuogt_put_msg();

#endif /* !PLUGIN_H_ */
