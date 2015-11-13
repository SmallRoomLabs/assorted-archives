CC      = g++
EXE	= 
CFLAGS  = -O2 
LIBS	=  -L/usr/local/lib

TARGETS = arithmetic_test$(EXE)

all: $(TARGETS)
	copy  /Y lgp_arithmetic.* ..\emulator

arithmetic_test$(EXE): arithmetic_test.c lgp_arithmetic.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
