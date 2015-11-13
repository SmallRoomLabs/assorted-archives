CC=gcc
CCFLAGS=-g -Wall -ansi -pedantic
PDP8ASM_OBJS=pdpnasm.c symtab.c

all: pdpnasm

pdpnasm: $(PDP8ASM_OBJS) 
	$(CC) $(CCFLAGS) $(PDP8ASM_OBJS) -o pdpnasm.exe

    