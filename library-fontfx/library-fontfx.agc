/// <p>This is a library for animating fonts easily. It is not dissimilar to the CreateText() option, except in
/// that if you want things to animate, you do not have to bother with all the tweens and chains, you just tell it
/// what you want to do. Examples are in the main.agc file. All of these use the same font (the standard 'subimages' font)
/// but you can use as many fonts as you like. It is best to start by looking at the 5 demos (change the gosub)
/// to pick another demo, and try to see how they work, especially Demo2 which gives an overview of the built in
/// animation commands. </p><p>
///	It is perhaps not too advisable to use this for too much action in a high speed game. The reason for this is 
/// primarily that it is written in AGK Basic itself, so in a high speed scenario there could be a lot of other things
/// going on, and processor power is not infinite, especially on some cheaper Android tablets and phones. It may be 
/// fine to animate a "Get Ready!" message at the start of a level, but it is probably not a great idea to use it
/// for the score which is being continually updated and animated. It might work better than I think, mind. It should
/// be remembered that once an animation has completed FFSync() calls do almost nothing, so you could animate the 
/// score into position and do it, but I wouldn't advise animating the score while the game is playing.</p><p>
/// As with many AGK features, it has a 'sync' call which should be called using the main Sync() loop to update everything.
/// This is FFXSync()</p><p>
/// Animations are sequences of commands, seperated by colons. Each command can have parameters, currently there are
/// four possible parameters, time (how long one animation takes) repeat (how many times it should repeat, 0 = for ever),
/// tween, which tween, if any should be used in the animation, 
/// and mod1 and mod2 which control the animation. Generally mod1 controls the 'aggressiveness' of the animation, so in
/// curve for example (which makes the text follow a sine curve) it adjusts the deflection in the curve.  The built in 
/// animations are documented in full later. Each part is seperated by a comma (e.g. "curve,repeat=2,time=2,mod1=4" is 
/// a single animation command.</p><p>
/// You can have as many of these commands sequentially as you want. If you want one to repeat endlessly, just have a final
/// command "loop" (see Demo3)</p><p>
///	There is one catch. Some animations (curve is one) repeat infinitely by default. If you want to run it for just (say)
/// five seconds, you need to do "curve,repeat=1,time=4.0", if you leave the repeat out it will default to zero and will
/// repeat the curve forever.</p><p>
/// Additional animations can be hard coded, there is an example of this in Demo4, which attempts to replicate (approximately)
/// the level end text from Rainbow Islands (Goal In!). The exact code is explained there, but the basic idea is each character
/// is called in turn, and you decide what to do with it - so in 'curve' for example you can move it up or down dependent on 
/// a sine calculation. You can make each character any size you want, make it go anywhere you want, set it to any angle 
/// and alpha. There is a helper function FFXOffsetSprite which assists with this.</p>
/// <p>One detail is that AGK doesn't really do optional extensibility well. The FFXEXTENSION constant works round this problem.
/// If you have no 'user defined animations' it can be set to zero (see main.agc), but if you have user defined ones (the demo has
/// one called "goalin") then this is defined to a function call, this function is then responsible for processing the 
/// animation. The FFXEXTENSION must be present either as a 0 or a call, if not the library won't compile.</p>
/// <p> the FFXCharacter and FFXText structures are documented, but unless you are hand coding animations in AGK Basic, you
/// should have no need of them</p>
/// <p>The tween functions in this library were derived from libclaw documentation developed by Julien Jorge</p>

/// @name 	library-fontfx
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix FFX
/// @provides fontfx
/// @requires
/// @version 0.1 
/// @created 08-Feb-15
/// @updated 17-Feb-15
/// @module

/// This type represents a single character. It provides all the information for that character to be animated 
/// if necessary. This is only used when writing hard coded animations.

type FFXCharacter
	spriteID as integer 												/// Sprite ID - still kept when not in use.
	x as float 															/// Horizontal position of the sprite
	y as float															/// Vertical position of the sprite
	width as float														/// Default sprite width
	height as float 													/// Default sprite height
	charIndex as integer 												/// Character number (count from 1)
	wordIndex as integer 												/// Word number (count from 1)
endtype

///	This type represents a single animateable string. It does not use AGK2 Strings because their sub characters
/// can't be independently scaled.<br>The developer should <b>not</b> manipulate this directly but use the methods.
/// provided. It can be accessed for values needed for hard coded animations.

type FFXText 
	x as float 															/// Horizontal position
	y as float															/// Vertical position
	xTopLeft as float 													/// Top left of string x
	yTopLeft as float 													/// Top left of string y
	charWidth as float 													/// Required char width
	charHeight as float 												/// Required char height
	maxWidth as float													/// Largest width of character
	maxHeight as float 													/// Largest height of character
	anchorPointX as integer 											/// Horizontal Anchor point (-1 left, 0 centre, 1 right)
	anchorPointY as integer 											/// Vertical Anchor point (-1 top,0 centre, 1 bottom)
	currentString as string 											/// Current string value
	characters as FFXCharacter[0]										/// Array of FFX characters.
	isAnimationRunning as integer 										/// Set to non-zero when animation is running.
	fullWidth as float 													/// Width of whole string 
	fullHeight as float 												/// Height of whole string 
	fontImageId as integer 												/// Image ID of font.
	wordCount as integer 												/// Number of words.
	charCount as integer 												/// Characters in whole String
	animationStartTime as integer 										/// System clock when animation started (milliseconds)
	animationRepeatCount as integer 									/// Repeat counter for animation.
	animationTime as integer											/// Time it runs for (overridden or default, milliseconds)
	animationPosition as integer 										/// Animation position : 0->1000
	animationModifier1 as float 										/// Animation modifiers 1
	animationModifier2 as float 										/// Animation modifiers 2
	animationCommand as string 											/// Complete animation command.
	animationCommandNo as integer 										/// Index of animation number.
	animationCommandCount as integer 									/// Animations in current command
	animationCurrentCommand as string 									/// Current single animation being executed
	animationTween as string 											/// Current tween for animation.
endtype 

//
//	Descriptor for Animation.
//
type _FFXAnimationInfo
	defaultTime as float												// How long it takes by default
	defaultRepeat as integer											// How many times it repeats by default.
	defaultModifier1 as float 											// Default modifiers
	defaultModifier2 as float
	tween as string 													// Tween to use if any.
endtype

#constant _FFXANIM_COMMAND_SEP 			":"								// Seperates commands in animations
#constant _FFXANIM_PARAM_SEP 			","								// Seperates commands/parameters in animations.

#constant _FFXCHAR_CODE_MIN 			32 								// Lowest ASCII code
#constant _FFXCHAR_CODE_MAX 			127 							// Highest ASCII code

global _FFXError_Fired as integer = 0 									// Set to 1 when error fired


///	Create a new animated text object. This is built using a single font. You cannot change the font used.
/// (just delete it and recreate it with the new font). The font is created filling most of the screen width
/// and centred horizontally and vertically.
/// @param txt 	Font Text Object
/// @param text Text to store in there.
/// @param fontImageID Image ID of font to draw with.

function FFXCreateText(txt ref as FFXText,text as String,fontImageID as integer)
	txt.fontImageID = fontImageID 										// save the image ID.
	txt.x = -99999:txt.y = txt.x 										// unlikely values so moves when centred initially
	txt.anchorPointX = 0:txt.anchorPointY = 0							// Anchor in the middle.
	txt.isAnimationRunning = 0											// Animation is not running.
	txt.currentString = "" 												// No current string as yet.
	txt.animationModifier1 = 1.0:txt.animationModifier2 = 1.0			// Reset the modifiers.
	_FFXFindFontMaximums(txt) 											// Find max height and width.
	_FFXSetText(txt,text)												// Set text, create and update the sprites.
	_FFXSizeText(txt,GetVirtualWidth()/len(text),-1) 					// Size to 70% of screen width, very roughly.
	_FFXMoveText(txt,GetVirtualWidth()/2,GetVirtualHeight()/2)			// Move text to screen centre.
endfunction

/// Delete a font text object and tidy up sprites used.
/// @param txt 	Font Text Object

function FFXDeleteText(txt ref as FFXText)
	i as integer
	for i = 1 to txt.characters.length 									// Delete all associated sprites.
		DeleteSprite(txt.characters[i].spriteID)
	next i
	txt.characters.length = 0
	txt.currentString = "" 												// And erase the array back to zero length.
endfunction 

/// Prepare to run an animation on a text object. This associates the command with the text object
///	setting it up to run. Subsequent calls to FFXSync() do the actual animation.
/// @param txt 	Font Text Object
/// @param animation Animation to run.

function FFXSetAnimation(txt ref as FFXText,animation as string)
	txt.animationCommandNo = 1											// Set the command number to 1 and count no of commands
	txt.animationCommand = animation 									// Save animation command
	txt.animationCommandCount = CountStringTokens(animation,_FFXANIM_COMMAND_SEP)
	_FFXStartAnimation(txt,animation,1)									// Start this as a new command.
endfunction

///	Restart the current animation on the given text object. This can be done at any time. The FFXSync() 
/// command returns 0 if the animation is no longer running.
/// @param txt 	Font Text Object

function FFXRestart(txt ref as FFXText)
	FFXSetAnimation(txt,txt.animationCommand)
endfunction

///	Stop an animation from running.
/// @param txt 	Font Text Object

function FFXStop(txt ref as FFXText)
	txt.isAnimationRunning = 0
endfunction

///	Update the given animation as per the system clock.  This should be called as part of the usual Sync() loop
/// (see the examples).
/// @param txt 	Font Text Object
/// @return 0 if the animation is complete, 1 otherwise.

function FFXSync(txt ref as FFXText)
	if _FFXError_Fired <> 0 then exitfunction 0 						// Do nothing if error occurred
	if txt.isAnimationRunning = 0 then exitfunction 0					// Do nothing if stopped.
	
	msTime as integer:msTime = GetMilliseconds() 						// Get curent milliseconds time.
		
	propTime as integer 												// This is the animation position in thousandths
	propTime = (msTime - txt.animationStartTime)*1000/txt.animationTime // like a percentage, but with thousandths.
	if propTime > 1000 then propTime = 1000 							// At most 1000.
	if txt.animationTween <> "" 										// Is there a tween.
		propTime = _FFXTween(propTime/1000.0,txt.animationTween) * 1000 // Then apply it.
	endif
	txt.animationPosition = propTime 									// Save in the structure
	
	i as integer
	for i = 1 to len(txt.currentString) 								// For each character.
		if mid(txt.currentString,i,1) <> " "							// Don't animate spaces, nothing to see.
			_FFXSetAnimationCharacter(txt.characters[i],txt,txt.animationCurrentCommand)
		endif
	next i
	
	if msTime >= txt.animationStartTime + txt.animationTime 			// Has it timed out ?
		if txt.animationRepeatCount <> 1 								// is it continuing
			_FFXStartAnimation(txt,txt.animationCommand,0)				// Then restart this current animation.
		else 
			txt.animationCommandNo = txt.animationCommandNo + 1 		// Go to the next command.
			if txt.animationCommandNo > txt.animationCommandCount 		// Is this more than the number of commands.
				txt.isAnimationRunning = 0 								// Animation is no longer running
			else 
				_FFXStartAnimation(txt,txt.animationCommand,1) 			// Do the next command
			endif 
		endif
	endif
endfunction txt.isAnimationRunning 										// Return still running flag.

// Start an animation running.
// @param txt 	Font Text Object
// @param animationCode Animation to run.

function _FFXStartAnimation(txt ref as FFXText,animationCode as string,firstRun as integer)
	animationCmd as string
	if Lower(Left(animationCode,4)) = "loop"   							// Special case where first command is loop.
		_FFXError("First command cannot be loop")						// Error
		txt.animationTime = 1.0											// This stops a divide by zero error.
		exitfunction
	endif
	repeat	
																		// Get the current command out.
		animationCmd = lower(GetStringToken(animationCode,_FFXANIM_COMMAND_SEP,txt.animationCommandNo))
		animationCmd = _FFXStrip(animationCmd)							// Remove spaces.
		
																		// Get the command part of the current command.
		txt.animationCurrentCommand = _FFXStrip(GetStringToken(animationCmd,_FFXANIM_PARAM_SEP,1))
		if txt.animationCurrentCommand = "loop" 						// loop round.
			txt.animationCommandNo = 1									// back to start
		endif
	until txt.animationCurrentCommand <> "loop"							// If loop do it again.
		
	info as _FFXAnimationInfo
	_FFXGetAnimationInfo(txt.animationCurrentCommand,info)				// Get Animation Information
	
	paramCount as integer:i as integer
	paramCount = CountStringTokens(animationCmd,_FFXANIM_PARAM_SEP)		// How many parameter elements are there ?
	if paramCount > 1													// If more than 1 (the first is the command)
		for i = 2 to paramCount											// Apply them all to the animation information
			_FFXProcessParameter(info,GetStringToken(animationCmd,_FFXANIM_PARAM_SEP,i))
		next i
	endif
	
	if firstRun <> 0  													// First run of animation
		txt.animationRepeatCount = info.defaultRepeat					// Set repeat count
	else
		txt.animationRepeatCount = txt.animationRepeatCount - 1 		// Decrement the repeat count.
		if txt.animationRepeatCount < 0 then txt.animationRepeatCount=0	// <0 is infinite (zero)
	endif
	
	txt.animationStartTime = GetMilliseconds()							// Store start clock and time in milliseconds. 					
	txt.animationTime = info.defaultTime * 1000.0						// Set animation time.
	txt.isAnimationRunning = 1 											// It is now running.
	txt.animationModifier1 = info.defaultModifier1						// Copy modifiers.
	txt.animationModifier2 = info.defaultModifier2
	txt.animationTween = info.tween 									// and tween.
endfunction
	
function _FFXProcessParameter(info ref as _FFXAnimationInfo,setter as string)
	if CountStringTokens(setter,"=") <> 2 								// Looking for x = n
		_FFXError("Syntax error in setter "+setter)
	endif 
	paramName as String													// Get parameter name.
	paramName = _FFXStrip(GetStringToken(setter,"=",1))
	value as float
	value = ValFloat(GetStringToken(setter,"=",2))						// Evaluate the RHS.
	select (paramName)													// Check the RHS and copy into the structure.
		case "time"
			info.defaultTime = value
		endcase
		case "repeat"
			info.defaultRepeat = value
		endcase
		case "mod1"
			info.defaultModifier1 = value
		endcase
		case "mod2"
			info.defaultModifier2 = value
		endcase
		case "tween"
			info.tween = _FFXStrip(GetStringToken(setter,"=",2))
		endcase
		case default													// Unknown
			_FFXError("Syntax error in setter "+setter)
		endcase
	endselect
endfunction

/// Set the text ARGB colours - same as SetSpriteColor() functionality in AGK2. All values are 0-255 and are
/// a fractional multiplier of the ARGB values. It is not possible to increase the ARGB of a sprite pixel using
/// this method. Used judiciously, this will allow one font to have several 'looks'.
///	@param txt 	Font Text Object
///	@param r	Colour Red
///	@param g	Colour Greem
///	@param b	Colour Blue
///	@param a	Alpha

function FFXSetColor(txt ref as FFXText,r as integer,g as integer,b as integer,a as integer)
	i as integer
	for i = 1 to len(txt.currentString)
		SetSpriteColor(txt.characters[i].spriteID,r,g,b,a)
	next i
endfunction

///	Update the text and resize/position accordingly.	
/// @param txt 	Font Text Object
/// @param text Text to store in there.

function FFXSetText(txt ref as FFXText,text as String)
	if text <> txt.currentString										// If moved
		_FFXSetText(txt,text)											// Update everything.
		_FFXSizeText(txt,txt.charWidth,txt.charHeight)
		_FFXMoveText(txt,txt.x,txt.y)
	endif
endfunction

/// Change the font size. This is the biggest the font can get, e.g. the biggest character
/// possible will fit in this space
/// @param txt 	Font Text Object
/// @param width Width in units (-1 to calculate from A/R)
/// @param height Height in units (-1 to calculate from A/R)

function FFXSetFontSize(txt ref as FFXText,width as float, height as float)
	if height > -1 or width > -1 										// Not leave unchanged
		_FFXSizeText(txt,width,height)									// If changing update size
		_FFXMoveText(txt,txt.x,txt.y)									// Reposition
	endif
endfunction

/// Reposition the text, taking note of anchorage points. 
/// @param txt 	Font Text Object
/// @param x new position horizontal
/// @param y new position vertical 

function FFXSetPosition(txt ref as FFXText,x as float,y as float)
	_FFXMoveText(txt,x,y) 												// Anchorage could change 
endfunction 

///	Set the anchor point for the text, e.g. the 'target' for SetPosition. If you set it to -1,-1
/// then SetPosition(x,5,8) will put the top left corner at (5,8). 
/// @param txt 	Font Text Object
/// @param x horizontal anchor point (-1 left,0 centre, 1 right)
/// @param y vertical anchor point (-1 top,0 centre, 1 bottom)

function FFXSetAnchorPoints(txt ref as FFXText,x as integer,y as integer)
	txt.anchorPointX = x												// Update anchors 
	txt.anchorPointY = y
	_FFXMoveText(txt,txt.x,txt.y)										// Reposition
endfunction
	
//	Update the sprites to show the text, adding sprites or hiding them as required. This "bodges" position
//	and size which are set by different methods.
// @param txt 	Font Text Object
// @param text Text to store in there.

function _FFXSetText(txt ref as FFXText,text as String)
	i as integer:char as String
	if len(text) > txt.characters.length 								// Is the character array too short.
		i = txt.characters.length+1 									// i points to the newest element.
		txt.characters.length = len(text) 								// Make the array the correct length.
		while i <= txt.characters.length 								// For each new one.
			txt.characters[i].spriteID = CreateSprite(LoadSubImage(txt.fontImageID,"42"))
			SetSpritePosition(txt.characters[i].spriteID,i*32,64) 		// this is just for testing.
			i = i + 1
		endwhile
	endif
	if len(text) < len(txt.currentString) 								// if the text is shorter.
		for i = len(text)+1 to txt.characters.length					// hide all the extra sprites for now.
			SetSpriteVisible(txt.characters[i].spriteID,0)
		next i
	endif
	word as integer = 1													// for word counting.
	for i = 1 to len(text)												// for each character.
		spr as integer:spr = txt.characters[i].spriteID 				// shortcut !	
		char = mid(text,i,1) 											// Get the character
		if char <> " "													// If not a space
			SetSpriteVisible(spr,1) 									// Show sprite.
			SetSpriteImage(spr,LoadSubImage(txt.fontImageID,str(asc(char)))) // Set the character
			SetSpriteSize(spr,-1,-1)									// Set back to default size.
			SetSpriteColor(spr,255,255,255,255)							// Reset Sprite ARGB
		else
			SetSpriteVisible(spr,0)										// It's a space, so hide it, speeds rendering a bit
		endif
		txt.characters[i].charIndex = i 								// Save character info.
		txt.characters[i].wordIndex = word 								// Save word index.
		if char = " " then word = word + 1								// increment word number if space found.	
	next i
	txt.wordCount = word 												// Save final word count
	txt.currentString = text 											// Finally update current text value
	txt.charCount = len(text)											// Update the character count
endfunction

// Resize the text as required. Scaling is done proportionally - it works out a scaling relative to the maximum
// width and height for the font and applies it appropriately.
// @param txt 	Font Text Object
// @param width Width in units 
// @param height Height in units

function _FFXSizeText(txt ref as FFXText,width as float, height as float)
	
	if height < 0 and width < 0  										// Must provide at least one size to work with.
		_FFXError("Cannot autosize both width and height")
		exitfunction
	endif

	if height < 0 then height = width * txt.maxHeight / txt.maxWidth 	// Calculate width and height if required.
	if width < 0 then width = height * txt.maxWidth / txt.maxHeight 	

	txt.charWidth = width:txt.charHeight = height 						// Required width and height
	
	xScale as float:yScale as float 									// This is how much we upscale from actual size.
	xScale = width / txt.maxWidth										// So calculate it with reference to the big char.
	yScale = height / txt.maxHeight  

	txt.fullWidth = 0 													// Calculate the total width as we go.
	i as integer:sp as integer
	for i = 1 to len(txt.currentString)
		sp = txt.characters[i].spriteID
		SetSpriteSize(sp,-1,-1)											// Back to standard size.
		txt.characters[i].width = GetSpriteWidth(sp) * xScale 			// Calculate what we need to scale it by
		txt.characters[i].height = GetSpriteHeight(sp) * yScale 
		SetSpriteSize(sp,txt.characters[i].width,txt.characters[i].height) // and set it.
		txt.fullWidth = txt.fullWidth + txt.characters[i].width 		// Add to total width
	next i 
	txt.fullHeight = yScale * txt.maxHeight 							// Save the full height
endfunction

// 	Reposition the text, taking note of anchorage points.
// 	@param txt 	Font Text Object
//  @param x new position horizontal
//  @param y new position vertical 

function _FFXMoveText(txt ref as FFXText,x as float,y as float)
	txt.x = x:txt.y = y 												// Save position
	if txt.anchorPointX > -1 											// Anchored anything other than left ?
		x = x - (1 + txt.anchorPointX) * txt.fullWidth / 2				// adjust it accordingly
	endif
	if txt.anchorPointY > -1 											// Similar for top/bottom
		y = y - (1 + txt.anchorPointY) * txt.fullHeight / 2
	endif
	txt.xTopLeft = x:txt.yTopLeft = y 									// Save top left position.
	i as integer
	for i = 1 to len(txt.currentString) 								// For each character.
		txt.characters[i].x = x:txt.characters[i].y = y 				// Save position.
		SetSpritePosition(txt.characters[i].spriteID,x,y) 				// Move the sprite 
		x = x + txt.characters[i].width 								// Move next space left.
		SetSpriteColorAlpha(txt.characters[i].spriteID,255)				// Reset alpha and angle
		SetSpriteAngle(txt.characters[i].spriteID,0)
	next i
endfunction

// Find the largest character widths and height in the font, this is used for scaling everything
// equivalently.
// @param txt 	Font Text Object

function _FFXFindFontMaximums(txt ref as FFXText)
	workSprite as integer:c as integer 
	workSprite = CreateSprite(LoadSubImage(txt.fontImageID,"32"))		// Acquire a sprite, any sprite (space)
	for c = _FFXCHAR_CODE_MIN to _FFXCHAR_CODE_MAX						// work through all the characters.
		SetSpriteImage(workSprite,LoadSubImage(txt.fontImageID,str(c)))	// Update the sprite
		SetSpriteSize(workSprite,-1,-1)									// Reset the sprite size, update min and max
		if GetSpriteWidth(workSprite) > txt.maxWidth then txt.maxWidth = GetSpriteWidth(workSprite)
		if GetSpriteHeight(workSprite) > txt.maxHeight then txt.maxHeight = GetSpriteHeight(workSprite)
	next c
	DeleteSprite(workSprite)											// throw the sprite used for this back.
endfunction

//	Character animation method.
// 	@param char 	Character to animate.
//	@param txt 		Circumstances under which it is animated.
//	@param animator How it is being animated.

function _FFXSetAnimationCharacter(char ref as FFXCharacter,txt ref as FFXText,animation as string)
	_FFXSystemAnimation(char,txt,animation)								// Call it 
endfunction

//	Display an error. Only the first error is displayed, because if there is an error (say) in the name of
// 	an animator it will fire repeatedly.
//	@param msg Error message

function _FFXError(msg as string)
	if _FFXError_Fired = 0 												// Only fire one error as can't abort.
		Message("[FontFX] "+msg)										// Display it
		_FFXError_Fired = 1												// No more.
	endif
endfunction
	
//	Remove leading and trailing spaces.
//	@param s 	string to strip
//	@return 	stripped string

function _FFXStrip(s as string)
	while s <> "" and left(s,1) = " ":s = mid(s,2,-1):endwhile
	while s <> "" and right(s,1) = " ":s = mid(s,1,len(s)-1):endwhile
endfunction s

//	Evaluate a tween/easing [name][in/out/inout]
//	@param t 	position from 0-1
// 	@param tween name of tween
//  @return new position from 0-1

function _FFXTween(t as float,tween as string)
	tween = lower(tween)
	if right(tween,5) = "inout" 
		tOrg as float:tOrg = t
		if tOrg > 0.5 then t = 1 - t
		t = _FFXBasicEasing(2*t,mid(tween,1,len(tween)-5)) / 2
		if tOrg > 0.5 then t = 1 - t
	elseif right(tween,3) = "out"
		t = 1-_FFXBasicEasing(1-t,mid(tween,1,len(tween)-3))		
	elseif right(tween,2) = "in"
		t = _FFXBasicEasing(t,mid(tween,1,len(tween)-2))
	else
		_FFXError("Unknown tween '"+tween+"'")
	endif
endfunction t

//	Evaluate a basic tween/easing - e.g. with no in or out
//	@param t 	position from 0-1
// 	@param tween name of tween
//  @return new position from 0-1
	
function _FFXBasicEasing(t as float,tween as string)
	
	select Lower(tween)
		
		case "back"
			t = t*t * (2.70158 * t - 1.70158)
		endcase

		case "bounce"
			t = _FFXBounce(t)
		endcase

		case "circ"
			t = 1 - sqrt(1 - t*t)
		endcase

		case "cubic"
			t = t * t * t
		endcase

		case "elastic"
			v as float:v = t - 1:p as float = 0.3
			t = -pow(2, 10 * v) * sinrad( (v - p / 4) * 2 * 3.14159 / p )
		endcase

		case "expo"
			if t <> 0 then t = pow(2,10 * (t - 1))
		endcase

		case "linear"
			t = t
		endcase

		case "quad"
			t = t*t
		endcase

		case "quart"
			t = t*t*t*t
		endcase

		case "quint"
			t = t*t*t*t*t
		endcase

		case "sine"
			t = 1 - cosrad(t * 3.1415 / 2)
		endcase
		
		case default
			_FFXError("Unknown base tween '"+tween+"'")
		endcase
		
	endselect
	
endfunction t

//	Implementation of bounce easing
//	@param t 	position from 0-1
//	@return 	new position

function _FFXBounce(t as float)
	
	v as float:c as float:d as float
	v = 1.0 - t
	
	if ( v < (1 / 2.75) )
		c = v
		d = 0
	elseif ( v < (2 / 2.75) )
		c = v - 1.5 / 2.75
		d = 0.75
	elseif ( v < (2.5 / 2.75) )
		c = v - 2.25 / 2.75
		d = 0.9375
	else
		c = v - 2.625 / 2.75
		d = 0.984375
	endif
	
endfunction 1 - (7.5625 * c * c + d)

// **************************************************************************************************************************
//								System hard coded animations go here onwards
// **************************************************************************************************************************

//	These are system animations - to add one, if the time is not 1.0 or the repeat is not 1 by default
//	change the _FFXGetAnimationInfo() appropriately, then add a "case" group to _FFXSystem_Animation()
//	If you write something useful it can be added to the defaults.

function _FFXGetAnimationInfo(animation as string,info ref as _FFXAnimationInfo)
	info.defaultRepeat = 1
	info.defaultTime = 1.0
	info.defaultModifier1 = 1.0
	info.defaultModifier2 = 1.0
	info.tween = ""
	if animation = "wobble" or animation = "jagged" or animation = "none" then info.defaultTime = 0.05
	if animation = "reset" then info.defaultTime = 0.01
	if animation = "wobble" or animation = "curve" or animation = "attention" then info.defaultRepeat = 0
	if animation = "scale" or animation = "pulser" or animation = "wpulser" then info.defaultRepeat = 0
endfunction

//	Hard coded system animations. Note that animations much reposition each character, do not assume the character is
//	at its normal position (e.g. not animated) previously. Character positions are only reset by the library if you change
//	things like text/size/postiion that require it. If you look at _FFXApplyPulse for example even if the character is not
// 	being pulsed it still resets it to its original position using the equivalent of FFXOffsetSprite(char,0,0,1,1) which
// 	means "put this one where it should be".
// 	@param char 	Character to animate.
//	@param txt 		Circumstances under which it is animated.
//	@param animator How it is being animated.

function _FFXSystemAnimation(char ref as FFXCharacter,txt ref as FFXText,animation as String)
	spr as integer:temp as integer:last as integer:ftemp as float
	spr = char.spriteID													// Sprite number
	
	select animation 													// These names correspond to the initialise ones above.

		case "reset"													// Resets sets it back to the default.
			FFXOffsetSprite(char,0,0,1,1)								// Reset position and size.
			SetSpriteAngle(spr,0):SetSpriteColorAlpha(spr,255)			// Reset angle and alpha.
		endcase
		
		case "wobble"													// Wobble makes it .... wobble to varying degrees
			temp = 1 * txt.animationModifier1							// Wobble amount for pos, size, angle
			SetSpritePosition(spr,char.x+Random2(-temp,temp),char.y+Random2(-temp,temp))	
			SetSpriteSize(spr,char.width * (Random2(-temp,temp)/10+1),char.height * (Random2(-temp,temp)/10+1))
			SetSpriteAngle(spr,Random2(-temp,temp))
		endcase
		
		case "jagged"													// Jagged makes it look look askew
			temp = 10 * txt.animationModifier1							// Alternate directions
			if mod(char.charIndex,2) = 0 then temp = -temp
			SetSpriteAngle(spr,temp)
		endcase
		
		case "fadein"													// Fades in over 1.5s
			temp = 255 * txt.animationPosition / 1000					// Work out interim alpha
			if last then temp = 255 									// End value is 255
			SetSpriteColorAlpha(spr,temp)								// Set Alpha
		endcase

		case "fadeout"													// Fades out over 1.5s
			temp = 255 * txt.animationPosition / 1000					// Same as above but reverses final alpha value.
			if last then temp = 255 
			SetSpriteColorAlpha(spr,255-temp)
		endcase
		
		case "attention" 												// Gently scales, pulsing gently.
			ftemp = 1.0 + sin(txt.animationPosition*360/1000) * 0.075 * txt.animationModifier1
			FFXOffsetSprite(char,0,0,ftemp,ftemp)
		endcase
		
		case "curve"													// Moves up and down in a wave
			ftemp = _FFXGetCurve(txt,char) - 0.5						// Get the curve, tweaked using M2.
			ftemp = ftemp * char.height * 0.4 * txt.animationModifier1 	// work out how much to adjust it by.
			FFXOffsetSprite(char,0,ftemp,1,1)
		endcase
		
		case "scale"													// Changes size vertically in a wave
			ftemp = _FFXGetCurve(txt,char)								// Get the curve, tweaked using M2.
			ftemp = ftemp * txt.animationModifier1 * 0.4 + 1			// work out how much to adjust it by.
			FFXOffsetSprite(char,0,0,1,fTemp)
		endcase

		case "pulser"													// Letters pulse individually.
			temp = 1000 / txt.charCount 								// Number of entries per pulse.
			_FFXApplyPulse(char,char.charIndex,txt.animationPosition/temp+1,mod(txt.animationPosition,temp) * 1.0 / temp,txt.animationModifier1)
		endcase

		case "wpulser"													// Words pulse individually
			temp = 1000 / txt.wordCount 								// Number of entries per pulse.
			_FFXApplyPulse(char,char.wordIndex,txt.animationPosition/temp+1,mod(txt.animationPosition,temp) * 1.0 / temp,txt.animationModifier1)
		endcase

		case "zoomin"													// Zoom In.
			_FFXZoom(char,txt,1000-txt.animationPosition)				// it's a reversed zoom out :)
		endcase
		
		case "zoomout"													// Zoom Out.
			_FFXZoom(char,txt,txt.animationPosition)
		endcase
			
		case "none"														// Back to default
		endcase
		
		case "movein" 													// Move into position
			_FFXMoveInOut(char,txt,1000-txt.animationPosition)
		endcase

		case "moveout" 													// Move out from position
			_FFXMoveInOut(char,txt,txt.animationPosition)
		endcase
		
		case default													// Error for everything else.
			temp = FFXEXTENSION 										// access this, could be a function.
			if temp = 0 then _FFXError("Unknown animation "+animation)	// Didn't work either.
		endcase
	endselect
endfunction

///	Offset sprite by a position and scale it, keeping position correct - required because of top/left
///	coordinates. Helper function for character manipulation. DO NOT USE for anything else.
///	@param 	ref 	Character object
///	@param  xOffset	Horizontal offset position.
///	@param  yOffset Vertical offset position.
///	@param  xScale 	Horizontal scaling (1.0 = unchanged, -ve = flip)
///	@param  yScale 	Vertical scaling (1.0 = unchanged, -ve = flip)

function FFXOffsetSprite(char ref as FFXCharacter,xOffset as float,yOffset as float,xScale as float,yScale as float)
	SetSpriteSize(char.spriteID,char.width*abs(xScale),char.height*abs(yScale))		// Set size of sprite
	SetSpriteFlip(char.spriteID,xScale < 0,yScale < 0)								// Set flip of sprite
																					// Set position of sprite.
	SetSpritePosition(char.spriteID,char.x-char.width*(abs(xScale)-1)/2+xOffset,char.y-char.height*(abs(yScale)-1)/2+yOffset)
endfunction

//	This calculates a curve position for a character, using animation modifier 2 to change the number of waves.
//	@param	txt		Text object
//	@param 	ref 	Character object

function _FFXGetCurve(txt ref as FFXText,char as FFXCharacter)
	adjPos as integer:retVal as float
	adjPos = mod(txt.animationPosition + 1000 * char.charIndex / txt.charCount * txt.animationModifier2,1000)
	retVal = sin(adjPos * 180.0 / 1000.0)
endfunction retVal

//	Decide whether to pulse a given character or not.
//	@param 	char 	Character object
//	@param 	n 		Item number
//	@param  curr 	Current item number derived from time
//	@param  position how far through the current pulse are we ?
//	@param  modifier modifier for how zoomy the pulse is.

function _FFXApplyPulse(char ref as FFXCharacter,n as integer,curr as integer,position as float,modifier as float)
	scale as float = 1.0													// Normally zoom to 1.0 e.g. normal
	if n = curr 															// Is this one pulsing ?
		scale = 1.0+position * 0.5 * modifier								// Calculate the zoom
	endif
	FFXOffsetSprite(char,0,0,scale,scale)									// And set the sprite scale.
endfunction

//	Function for handling zooming.
//	@param	txt		Text object
//	@param 	char 	Character object
//	@param  position how far through the current zoom are we ?
		
function _FFXZoom(char ref as FFXCharacter,txt ref as FFXText,position as integer)
	zoom as float:offset as float
	position = position * txt.animationModifier1 							// Scale by mod1
	zoom = 10*position/1000.0+1												// Zoom position
	offset = (char.charIndex + 1.0) / txt.charCount - 0.5					// Calculate offset from centre
	offset = offset * txt.fullWidth / txt.charCount * position / 5 			// Make it a x offset.
	FFXOffsetSprite(char,offset,0,zoom,zoom)								// Reposition the sprite
endfunction

//	Function for handling moving.
//	@param	txt		Text object
//	@param 	char 	Character object
//	@param  position how far through the current move are we ?
		
function _FFXMoveInOut(char ref as FFXCharacter,txt ref as FFXText,position as integer)
	mSin as float:mCos as float												// Sine and Cosine of angle
	mSin = Sin(txt.animationModifier1):mCos = Cos(txt.animationModifier1) 	
	x as integer:y as integer 	
	x = txt.xTopLeft + txt.fullWidth/2: y = txt.yTopLeft + txt.maxHeight/2	// This is near enough the centre of the string.
	if mCos < 0 then x = GetVirtualWidth()-x 								// Coming from the right.
	if mSin < 0 then y = GetVirtualHeight()-y 								// Coming from the bottom.
	if txt.animationModifier2 > 1 											// Actual distance specified.
		x = txt.animationModifier2:y = x 									// Then use that distance instead.
	endif
	x = x * position / 1000.0 * mCos * 1.75									// Work out position and scale off screen
	y = - y * position / 1000.0 * mSin * 1.75
	FFXOffsetSprite(char,x,y,1,1)											// Move it.
endfunction
