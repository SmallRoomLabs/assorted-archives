SetWindowTitle( "library-tiles Demonstration" )
SetWindowSize( 1024, 768, 0 )
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 0,0, 1, 1 )
SetPrintSize(32)


#option_explicit
#include "library-tiles.agc"
#include "test_tilecode.agc"

i as integer
map as TILMap
TileSetup_test(map)

rnd as TILRender
rnd.x = 32:rnd.y = 32:rnd.width = GetVirtualWidth()-64:rnd.height = GetVirtualHeight()-64
TILPrepareRender(map,rnd,1000)

rnd2 as TILRender[3]
for i = 1 to rnd2.length
	rnd2[i].x = GetVirtualWidth()-256-64-(i-1)*320:rnd2[i].y = 64:rnd2[i].width = 256:rnd2[i].height = 192
	if i = 2 then rnd2[i].y = GetVirtualHeight()-240
	rnd2[i].tileWidth = 16:rnd2[i].tileHeight = rnd2[i].tileWidth
	TILPrepareRender(map,rnd2[i],3700+(i-1)*1000)
	rnd2[i].layerDepth[1] = 14:rnd2[i].layerDepth[2] = 13
	CreateSprite(i,LoadImage("frame.png"))
	SetSpriteSize(i,256*1.05,192*1.05)
	SetSpritePosition(i,rnd2[i].x-8,rnd2[i].y-6)
next i

//rnd2[2].isLockedToMap = 1
//rnd2[2].isCentreOfRender = 1

AddVirtualJoystick(1,900,650,200)

x1 as float:y1 as float:s as float = 5.0
repeat
	x1 = x1 + GetVirtualJoystickX(1)/s
	y1 = y1 + GetVirtualJoystickY(1)/s
	TILMove(map,rnd,x1,y1)
	for i = 1 to rnd2.length
		TILMove(map,rnd2[i],x1,y1) 
	next i
    Print( ScreenFPS() )
    print(str(x1)+","+str(y1))
    Sync()
until GetRawKeyState(27) > 0 
End

// 4 options for drawing - corner, centred on, corner clipped, centred on clipped.
// Code to Levels turned off and on - off must blank level sprites.
// Look at offset/isometric ?
// Tile to Sprite and Sprite to tile positions. Other ideas possibly nicked from MTE ?
// Create map imaging code.
