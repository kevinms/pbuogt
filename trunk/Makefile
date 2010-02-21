CC=gcc
CFLAGS=-g -Wall

# Build bot
LDFLAGS=-rdynamic -ldl
SOURCES=pbuogt.c config.c plugin.c list.c utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=pbuogt

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Build tests
T_CONFIG_SOURCES=config-test.c config.c list.c utils.c
T_CONFIG_OBJECTS=$(T_CONFIG_SOURCES:.c=.o)

T_PLUGIN_LDFLAGS=-rdynamic -ldl
T_PLUGIN_SOURCES=plugin-test.c plugin.c list.c
T_PLUGIN_OBJECTS=$(T_PLUGIN_SOURCES:.c=.o)

T_SOURCES = $(T_CONFIG_SOURCES) $(T_PLUGIN_SOURCES)
T_EXECUTABLE=test-config test-plugin

build-tests: $(T_EXECUTABLE)
	

test-config: $(T_CONFIG_OBJECTS)
	$(CC) $(LDFLAGS) $(T_CONFIG_OBJECTS) -o $@

test-plugin: $(T_PLUGIN_OBJECTS) 
	$(CC) $(T_PLUGIN_LDFLAGS) $(T_PLUGIN_OBJECTS) -o $@

# Implicit rule to compile all .c to .o
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

# Cleanup rules
clean:
	rm -fv *.o $(EXECUTABLE) pbuogt-out pbuogt-event pbuogt-script

clean-tests:
	rm -fv *.o $(T_EXECUTABLE)
