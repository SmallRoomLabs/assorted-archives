// *****************************************************************************************************************
// *****************************************************************************************************************
//
//									Demo Program for library-fontfx.agc
//
// *****************************************************************************************************************
// *****************************************************************************************************************
//
//												Common set-up stuff
//
// *****************************************************************************************************************
SetWindowTitle( "FontDemo" )													// Set the window up as 1024x768
SetWindowSize( 1024, 768, 0 )
SetVirtualResolution( 512,384 )													// Set resolution as 512x384
#option_explicit																// Force all declarations.
#constant FONT_IMAGE	1 														// ID of the font image loaded.

//	This is used to enable user defined animations in code. FFX_EXTENSION has to be defined in anything that
//	uses the library. Zero means no user defined animations, something like is used here means "go and look in
//	that function for that animation". See "Demo4" for examples of usage (Demo1,2 and 3 do not require this).
	
//#constant FFXEXTENSION	0
#constant FFXEXTENSION 	ExtendAnimation(char,txt,animation)

#include "library-fontfx.agc"													// Include the font library
LoadImage(FONT_IMAGE,"king.png")												// Load this font I made earlier .

// *****************************************************************************************************************
//
//	Change this to switch Demos. There are five - 
//
//		Demo1	:		a simple test bed I use
//		Demo2	:		shows all the built in animations
//		Demo3	:		shows a chain demo in operation
//		Demo4 	:		The "Goal in !" demo in action.
//		Demo5 	:		The README demo
//
// *****************************************************************************************************************

gosub Demo5																		// So change this accordingly.

end

// *****************************************************************************************************************
//													Demo1 I use for testing
// *****************************************************************************************************************

Demo1:

tf as FFXText																	// We use this throughout.

FFXCreateText(tf,"This is FontFX text",FONT_IMAGE)								// Create a new text object
FFXSetFontSize(tf,33,-1)														// Set it's size.
FFXSetPosition(tf,GetVirtualWidth()/2,64)
FFXSetAnimation(tf,"moveout,time = 4,mod1=-90,mod2=150,tween=bounceinout:reset:loop")			// A typical sequence

repeat																			// Main loop
	FFXSync(tf) 																// Keep Syncing
    Print( ScreenFPS() )														// Show FPS
    Sync()																		// And the normal Sync
until GetRawKeyState(27) > 0													// Until ESC Pressed
return

// *****************************************************************************************************************
//											Demo2 shows all of them in action
// *****************************************************************************************************************

Demo2:

i as integer:x as integer:y as integer:n as integer
demoList as string																// What we are showing.
demoList = "movein,mod1=180,time=3:none,time=1;moveout,time=2:none,time=1;wobble;jagged;fadein,time=3;fadeout,time=2;"
demoList = demoList + "attention;curve,mod1=3;scale;pulser,time=4;wpulser;"
demoList = demoList + "reset;zoomin,mod1=0.2,time=3;zoomout,mod1=0.2,time=2"
	
txtObj as FFXText[0]:txtObj.length = CountStringTokens(demoList,";")			// Create array of text objects of reqd length

for i = 1 to txtObj.length 														// Now create them
	anim as string:anim = GetStringToken(demoList,";",i)						// Extract animation from demoList at semicolon
	FFXCreateText(txtObj[i],"This is '"+GetStringToken(anim,",",1)+"'.",FONT_IMAGE) // Create text
	x = (mod((i - 1),2)*2+1) * 0.25 * GetVirtualWidth()							// Work out where to position it.
	y = (i - 1) / 2:y = (y + 1) * 50 - 20
	FFXSetPosition(txtObj[i],x,y)												// And position it
	FFXSetFontSize(txtObj[i],16,28)												// Size it
	FFXSetAnimation(txtObj[i],anim)												// And set the animation.
next i
	
repeat																			// Main loop
	for i = 1 to txtObj.length													// Work through each
		if FFXSync(txtObj[i]) = 0 then FFXRestart(txtObj[i])					// If it finished (ret zero) restart it.
	next i																		// So things like zoomin loop endlessly
	Sync()
until GetRawKeyState(27) > 0
return

// *****************************************************************************************************************
//									Demo3 shows a simple chaining animation.
// *****************************************************************************************************************


Demo3:

tc as FFXText																	// We use this throughout.
animation as String																// This is the animation

// Obviously you don't have to build it up this way. I've done it this way so hopefully it is fairly clear 
// what is happening. You end up with a string of commands seperated by colons.

animation = "movein,time=2,mod1 = 90,tween=bounceout"	// Move in from the top. mod1 is the angle it comes in from (90 degrees)
animation = animation + ":fadeout, time = 0.4"			// Fade out over 0.4s
animation = animation + ":fadein, time = 0.4"			// Fade in over 0.4s
animation = animation + ":wobble, time = 1, repeat = 1"	// Wobble for a second (repeat is needed because default is forever)
animation = animation + ":curve, time = 2,mod2 = 3,mod1 = 2,repeat = 1" // Curve for 2s, 3 'humps', double the curve effect.
animation = animation + ":moveout, mod1 = -45" 			// and move out diagonally.
animation = animation + ":loop" 						// This makes it do it again - remove this and it only goes once.

FFXCreateText(tc,"This is chaining",FONT_IMAGE)									// Create a new text object
FFXSetFontSize(tc,24,-1)														// Set its size.
FFXSetAnimation(tc,animation)													// Set its animation.

repeat																			// Main loop
	FFXSync(tc) 																// Keep Syncing
    Sync()																		// And the normal Sync
until GetRawKeyState(27) > 0													// Until ESC Pressed
return

// *****************************************************************************************************************
//												Demo 4 is "Goal In !"
// *****************************************************************************************************************

Demo4:

gi as FFXText
FFXCreateText(gi,"Goal In!",FONT_IMAGE)											// Create "Goal In !" text.
FFXSetFontSize(gi,32,-1)														// Set size
FFXSetColor(gi,128,255,0,255)													// Set Colour.
FFXSetAnimation(gi,"goalin,time = 6:fadeout")									// Animate "Goal In !" style for 6s then fade.

repeat																			// Main loop
	FFXSync(gi) 																// Keep Syncing
    Sync()																		// And the normal Sync
until GetRawKeyState(27) > 0													// Until ESC Pressed

//	This is a bit like the "Goal In" animation from rainbow islands - it loops in from the left, characters 
//	spinning as this happens, eventually all characters stop rotating.
//	Time here is an abstract concept. There is 0-1000 "time units" which are the time the animation lasts for,
//	but these can represent any physical time you like - change time=6 above for example

//	0-300 		All numbers rotating
//	300-900 	Numbers become static one at a time.
//	900-1000	Static

//	This function returns '1' if it recognised the animation - so you can add it as many animations as you like. 

function ExtendAnimation(char ref as FFXCharacter,txt ref as FFXText,animation as String)
	
	if animation <> "goalin" then exitfunction 0								// not goalin, so return 0 => unknown animation.
	
	// All code from here on implements "GoalIn". The first bit does the rotatiion. The second bit (timePerChar) stops the
	// characters rotating one at a time. The third bit does the movement to the finished position.
	
	pos as integer:pos = txt.animationPosition									// Get the position in the animation.

	xScale as float 															// We are going to calculate the xScale
	xScale = mod(pos/6,40)														// Range 0-39.
	if xScale >= 20 then xScale = 39-xScale										// Make it go 0-20,20-0,0-20,20-0 ....
	xScale = xScale/10-1.0 														// Convert to a scale, sort of looks spinning.
																				// scale is actually going 1 to -1 then back again.

	timePerChar as integer:timePerChar = 600 / txt.charCount 					// How many thousandths per character stop time.	
	if pos > 300+timePerChar * (char.charIndex-1) then xScale = 1				// If past that time, then stop rotating.
																				// Will stop very suddenly, but it's simple.
																				// Note, char.charIndex is the character number.
																				
	xOffset as float = 0.0:yOffset as float = 0.0 								// Offset position.
	stopMoveTime as integer 													// This is when it stops moving.
	stopMoveTime = 200 + timePerChar * (char.charIndex-1)
	if pos < stopMoveTime 														// Not reached the time when it stops moving ?
		xOffset = -(stopMoveTime-pos)											// horizontal offset
		yOffset = -(sin(-xOffset*1.1)) * 50										// use this and trig to make it sort of bounce in
		if yOffset > 0 then yOffset = yOffset * 2								// make it steeper on the down side.
	endif
	
	FFXOffsetSprite(char,xOffset,yOffset,xScale,1)								// Finally, do the offset and scale.
	
endfunction 1																	// returning 1 here means we recognised "goalin"

// *****************************************************************************************************************
//													Demo 5 - README demo
// *****************************************************************************************************************

Demo5:

titleText as FFXText															// This is the drop down text
FFXCreateText(titleText,"Get Ready !",FONT_IMAGE)								// Create it
FFXSetAnimation(titleText,"movein,time=1.5,mod1=90,tween=bounceout:fadeout")	// MoveIn with bounce, then fadeout.
FFXSetColor(titleText,255,128,0,255)											// Recolour it.

scoreText as FFXText 															// This displays the score.
FFXCreateText(scoreText,"100",FONT_IMAGE)										// Set text and size.
FFXSetFontSize(scoreText,8,-1)
FFXSetAnimation(scoreText,"moveout,time=0.3,mod1=90,mod2=20:fadeout,time=0.3")	// Move up a bit and fade out fairly quickly.
FFXStop(scoreText)																// Stop the animation.
FFXSetPosition(scoreText,0,-100)												// Put off screen so not visible.
FFXSetColor(scoreText,0,255,255,255)											// Example of recolouring.

CreateText(1,"000000")															// This shows the actual score.
SetTextSize(1,32.0)
SetTextFontImage(1,FONT_IMAGE)
score as Integer = 0															// The score

repeat
	if GetPointerPressed() > 0 													// Mouse clicked
		FFXSetPosition(scoreText,GetPointerX(),GetPointerY())					// Move score to the click position
		FFXRestart(scoreText)													// Restart the animation
		score = score + 100														// Bump and update the score
		SetTextString(1,right("000000"+str(score),6))
	endif
	FFXSync(titleText)
	FFXSync(scoreText)
	Sync()
until GetRawKeyState(27) > 0 
