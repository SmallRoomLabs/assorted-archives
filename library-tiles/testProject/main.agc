SetWindowSize( 1024, 768, 0 )
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 0,0, 1, 1 )

#option_explicit
#include "../library-tiles.agc"
#include "tmx_square.agc"
#include "tmx_isometric.agc"
#include "tmx_isometric2.agc"

i as integer:layers as integer:s as integer

map as TILMap
//
//	Comment or uncomment to switch between the demo maps.
//	Note controls do not yet work correctly for isometric even though the iso code is right.

//TILSetup_Square(map)
//TILSetup_Isometric_grass_and_water(map)
TILSetup_Isometric(map)

rnd as TILRender
rnd.baseDepth = 1000
rnd.x = 0
rnd.y = 200
rnd.width = GetVirtualWidth()
rnd.height = GetVirtualHeight()-rnd.y

mini as TILRender[4]
for i = 1 to mini.length
	mini[i].x = (i-1) * 200 + 20
	mini[i].y = 20
	mini[i].width = 180
	mini[i].height = 100+i*20
	mini[i].baseDepth = 10
	mini[i].tileWidth = 8+(i-1)*4
	mini[i].tileHeight = mini[i].tileWidth
next i

SetPrintSize(24)

x as float
y as float 
AddVirtualJoystick(1,GetVirtualWidth()-128,GetVirtualHeight()-128,256)

repeat
	rnd.forceRepaint = 1
	x = x + GetVirtualJoystickX(1)/6
	y = y + GetVirtualJoystickY(1)/6
    TILSetScroll(map,rnd,x,y)
    for i = 1 to mini.length
		TILSetScroll(map,mini[i],x,y)
		DrawBox(mini[i].x,mini[i].y,mini[i].x+mini[i].width,mini[i].y+mini[i].height,-1,-1,-1,-1,0)
	next i
    Print( str(ScreenFPS())+" "+str(x,1)+","+str(y,1))

    Sync()
until GetRawKeyState(27)

// Isometric : coarse scrolling only.
// Isometric : no background colour.

// TODO: Isometric different POV ?
// TODO: Optimisation (always :) )
// TODO: Objects (copy into renderer, don't change the map ?)
// TODO: Copy map into render and only use the render to (err...) render ? Or have a single map ? Single Map I think
// TODO: Layer on/off.
// TODO: CSV Import / Base64 import


