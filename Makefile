CC=gcc  #compiler
TIMEF=10 #wait time (system)
TIMEN=1 #wait time (publisher)
PIPEPUBLISHERS=pipeSP #pipe publishers
PIPESUBSCRIBERS=pipeSS #pipe subscribers

# Maybe for the file in publishers
# args = `arg="$(filter-out $@,$(MAKECMDGOALS))" && echo $${arg:-${1}}`
# To use the value $(call args,defaultstring)
# https://stackoverflow.com/questions/6273608/how-to-pass-argument-to-makefile-from-command-line

# SC
sc:
	$(CC) -pthread system.c -o sistema && ./sistema -p $(PIPEPUBLISHERS) -s $(PIPESUBSCRIBERS) -t $(TIMEF)

# Publisher
pub:
	$(CC) -pthread publisher.c -o publicador && ./publicador -p $(PIPEPUBLISHERS) -f file -t $(TIMEN)

pub2:
	$(CC) -pthread publisher.c -o publicador && ./publicador -p $(PIPEPUBLISHERS) -f fil3 -t $(TIMEN)

# Subscriptor
sub:
	$(CC) -pthread subscriber.c -o subscriptor && ./subscriptor -s $(PIPESUBSCRIBERS)
