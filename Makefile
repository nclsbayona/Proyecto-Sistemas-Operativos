CC=gcc  #compiler
TIME=10 #target file name
PIPEPUBLISHERS=pipeSP #pipe publishers
PIPESUBSCRIBERS=pipeSS #pipe subscribers

# Maybe for the file in publishers
# args = `arg="$(filter-out $@,$(MAKECMDGOALS))" && echo $${arg:-${1}}`
# To use the value $(call args,defaultstring)
# https://stackoverflow.com/questions/6273608/how-to-pass-argument-to-makefile-from-command-line

# SC
sc:
	$(CC) system.c -o sistema && ./sistema -p $(PIPEPUBLISHERS) -s $(PIPESUBSCRIBERS) -t $(TIME)

# Publisher
pub:
	$(CC) publisher.c -o publicador && ./publicador -p $(PIPEPUBLISHERS) -f file -t $(TIME)

# Subscriptor
sub:
	$(CC) subscriber.c -o subscriptor && ./subscriptor -s $(PIPESUBSCRIBERS)
