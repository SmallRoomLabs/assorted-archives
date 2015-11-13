
// Project: FontTest 
// Created: 2015-01-01

// set window properties
SetWindowTitle( "FontTest" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 102,76 )
SetOrientationAllowed( 1, 1, 1, 1 )
LoadImage(1,"demo.png")

for i = 1 to 5
	CreateText(i,"AC Text item "+str(i))
	SetTextPosition(i,10,(i-1)*14+5)
	SetTextDefaultFontImage(1)
	SetTextSize(i,76/(8+i*3))
next i

SetTextColor(1,255,255,0,255)
SetTextColor(2,0,255,255,255)
SetTextColor(3,255,0,255,255)
x = 0

repeat
	inc x
	for i = 1 to 5
		xp = (102-GetTextTotalWidth(i))*(abs(30-mod(x/2,60))/30.0)
		SetTextPosition(i,xp,GetTextY(i))
	next i

    Print( ScreenFPS() )
    Sync()
until GetRawKeyState(27) <> 0
