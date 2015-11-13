
// Project: Test1802 
// Created: 2015-03-10

// set window properties
SetWindowTitle( "Test1802" )
SetWindowSize( 1024, 768, 0 )
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
#option_explicit
#include "library-rca1802.agc"

function RCAAccessHardware(id as integer,data as integer)
endfunction 0

cycles as integer = 1
fps as integer
i as integer
clock as float
RCAReset()
repeat
	fps = ScreenFPS()
	if fps < 58 
		dec cycles
	else
		cycles = cycles + 20
	endif
	clock = cycles * 16 * 60 / 1000000.0
	RCAExecute(cycles)
    Print("FPS "+str(fps))
    print("Cyc "+str(cycles))
    print("Clk "+str(clock,2)+" Mhz")
    Sync()
until GetRawKeyState(27) > 0
