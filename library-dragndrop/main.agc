//
//	This is a demonstration program for dragging and dropping.
//
//	I hope it's fairly clear what's happening here.
//
SetWindowTitle( "dragdropdemo" )												// Set up the screen.
SetWindowSize( 800,600, 0 )
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )

#option_explicit																// Good practice
#include "library-dragndrop.agc"												// Include library.

//
//	Changing these globals allows you to experiment - whether we can drop on the shop or not, on the desktop or not.
//
global canDropOnDesktop = 0 													// Globals control the drop handler
global canDropOnShop = 1														// just for experimenting !

#constant FONT_IMAGE 			1												// Font Identifier
LoadImage(FONT_IMAGE,"demofont.png")											// Set it up.
SetTextDefaultFontImage(FONT_IMAGE)

global logInfo as string[5]														// Contains logging information
global i as integer	

for i = 1 to 3 																	// 3 sprites, 1001 to 1003 (stars with numbers)
	CreateSprite(1000+i,LoadImage("s"+str(i)+".png"))							// Load Image
	SetSpriteSize(1000+i,64+i*16,-1)											// Size and position.
	SetSpritePosition(1000+i,256-32-i*8,192*i-32-i*8)							
next i

for i = 1 to 3 																	// 3 Text objects, 2001 to 2003
	CreateText(2000+i,"["+str(2000+i)+"]")										// Create it
	SetTextSize(2000+i,48)														// Size and position.
	SetTextPosition(2000+i,512-GetTextTotalWidth(2000+i)/2,192*i-GetTextTotalHeight(2000+i)/2)
next i

CreateSprite(3000,LoadImage("t1.png")) 											// 3000 is the shop graphic.
SetSpriteSize(3000,192,-1)														// Size and position that.
SetSpritePosition(3000,1024-32-GetSpriteWidth(3000),384-GetSpriteHeight(3000)/2)
SetSpriteDepth(3000,11)

global dc as DNDControl															// Drag and Drop control object
global dei as DNDEventInfo 														// Return information from DNDHandlePointer goes here.

DNDInitialise(dc)																// Initialise the DND control object.

//
//	Now tell it about what we can drag, drop and click. Note you can still detect clicks the normal way
//	in AGK2.
//
DNDAddObject(dc,"C",DND_SPRITE,1001)											// 1001 is clickable
DNDAddObject(dc,"D",DND_SPRITE,1002)											// 1002 is draggable, not clickable
DNDAddObject(dc,"CD",DND_SPRITE,1003)											// 1003 is draggable and clickable.

DNDAddObject(dc,"C",DND_TEXT,2001)												// 2001 is clickable
DNDAddObject(dc,"D",DND_TEXT,2002)												// 2002 is draggable, not clickable
DNDAddObject(dc,"CD",DND_TEXT,2003)												// 2003 is draggable and clickable.

if canDropOnShop <> 0 then DNDAddObject(dc,"T",DND_SPRITE,3000)					// 3000 (the shop) is targettable

if canDropOnDesktop <> 0 then DNDAddObject(dc,"T",DND_DISPLAY,0)				// If drop on desktop then it must be targettable.

//
//	Main loop
//
repeat
    for i = 1 to logInfo.length:print(str(i)+":"+logInfo[i]):next i				// Print the simple log.
		
	i = DNDHandlePointer(dc,dei)												// This call does everything
	if i <> 0 																	// Returns non-zero if something happened  
		LogBasic("Call returns "+str(i))										// It will either be a click or a drag/drop.
		DropDescriber(dei)														// Describe what happened.
	endif
    Sync()
until GetRawKeyState(27) > 0

//
//		This handles/dumps successful drops.. drops onto targets are ignored, drops on to the display cause the object
//		in question (sprite or text) to be moved there.
//

function DropDescriber(info ref as DNDEventInfo)
	if info.event = DND_CLICK													// If we clicked, log that we clicked.
		LogBasic("Click "+str(info.clickedObjectID)+":"+str(info.clickedObjectType))
	else																		// Drop code, first log data structure.
		LogBasic(str(info.draggedObjectID)+":"+str(info.draggedObjectType)+"->"+str(info.droppedObjectID)+" "+str(info.offset.x)+","+str(info.offset.y))
	
		if canDropOnDesktop <> 0 and info.droppedObjectID = DND_DISPLAY 		// Was it dropped onto the display, and are we doing this ?
			if info.draggedObjectType = DND_SPRITE								// Differentiate if dragged object was sprite or text
				SetSpritePosition(info.draggedObjectID,info.offset.x,info.offset.y)	// Reposition sprite
			else
				SetTextPosition(info.draggedObjectID,info.offset.x,info.offset.y)	// Reposition text
			endif
		endif
	
		if canDropOnShop <> 0 and info.droppedObjectID = 3000 					// Was it dropped onto the shop, and are we doing this ?
			x as float:y as float
			x = info.offset.x + GetSpriteX(3000)								// Work out the actual position - offset is relative
			y = info.offset.y + GetSpriteY(3000)								// to the top level of the dropped object.
			if info.draggedObjectType = DND_SPRITE								// sprite or text ?
				SetSpritePosition(info.draggedObjectID,x,y)
			else
				SetTextPosition(info.draggedObjectID,x,y)
			endif
		endif
	endif
endfunction

//
//		World's worst logger. (Possibly Michael Palin in the Lumberjack Song ?)
//
function LogBasic(msg as string)
	i as integer
	for i = logInfo.length-1 to 1 step -1:logInfo[i+1] = logInfo[i]:next i
	logInfo[1] = msg
endfunction


