/// @name 	library-tiles
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix TIL
/// @provides tiles
/// @requires
/// @version 0.1 
/// @created 28-Feb-15
/// @updated 10-Mar-15
/// @module

#constant _TIL_HFLIP 	0x40000000												// Bitmasks for various parts of tile ID.
#constant _TIL_VFLIP 	0x80000000
#constant _TIL_HVFLIP 	0xC0000000
#constant _TIL_MASK 	0x3FFFFFFF

///	This represents a collection of integer data. It is used for the decompression code, hence the next
///	write position, allowing the data to be extended arbitrarily.

type _TILObject
	nextWritePosition as integer												/// Where next expanded write goes
	data as integer[1]															/// data array.
endtype

///	This represents a single map with multiple layers (e.g. in practice a .tmx file). This is not the same as a visible
///	map ; a map can have multiple different renders.

type TILMap
	agkToTileMapID as _TILObject 												/// Maps converted IDs to original IDs.
	layerCount as integer 														/// Number of layers.
	uniqueTileCount as integer 													/// Number of unique tiles.
	layers as _TILObject[1]														/// Layers
	layerNames as string[1]														/// Array of layer names
	tileWidth as integer														/// tile width pixels
	tileHeight as integer														/// tile height pixels
	mapWidth as integer															/// map width tiles
	mapHeight as integer														/// map height tiles.
	background as integer[3]													/// RGB background.
	tileImage as integer 														/// ID of the atlas tile image.
	tileImages as integer[1]													/// IDs of tiles sub images
endtype

///	Rendering of a map. There is one extra row rendered on the right and bottom for scrolling.
/// The offset and multiplier is arranged so that (level) * levelMultiplier + (y) * rowMultiplier + (x) + offset
/// is an index into tileID[]

type TILRender
	x as integer																/// top left position of render window
	y as integer																/// top right position of render window
	width as integer															/// width of render window
	height as integer															/// height of render window
	x2 as integer 																/// RHS of render window
	y2 as integer 																/// Bottom of render window
	xTile as integer 															/// Top left pixel contains this tile (horizontal)
	xOffset as integer 															/// Pixel offset of top left in tile (horizontal)
	yTile as integer 															/// Top left pixel contains this tile (vertical)
	yOffset as integer 															/// Pixel offset of top left in tile (horizontal)
	tileWidth as integer 														/// Rendering tile width (0 = default in map)
	tileHeight as integer 														/// Rendering tile height (0 = default in map)
	xTileCount as integer 														/// No of tiles across screen horizontally (round up)
	yTileCount as integer 														/// No of tiles across screen vertically (round up)
	wrapsX as integer 															/// Non zero if map wraps horizontally
	wrapsY as integer 															/// Non zero if map wraps vertically
	tileID as integer[1]														/// Sprite Tile IDs (not Sprite IDs)
	baseSpriteID as integer 													/// Sprite ID base value.
	offset as integer 															/// Sprite offset
	rowMultiplier as integer 													/// Sprite multiplier (row)
	lvlMultiplier as integer 													/// Sprite multiplier (level)
	backgroundSprite as integer 												/// Background sprite (solid for bgr colour)
	forceCompleteRedraw as integer 												/// When non-zero redraws whole thing irrespective.
	layerDepth as integer[1] 													/// Depth at each layer.
	layerEnabled as integer[1]													/// Non-zero if layer is enabled.
	isLockedToMap as integer 													/// Non zero if the map does not scroll outside its area.
	isCentreOfRender as integer 												/// Non-zero if the map should be centred on move point,if possible.
endtype

///	Prepare the renderer for rendering
///	@param 	map 	Map data
///	@param 	rnd 	Rendering data
/// @param  baseID 	Base sprite - enough sprite for everything.

function TILPrepareRender(map ref as TILMap,rnd ref as TILRender,baseID as integer)
	rnd.baseSpriteID = baseID 													// Save the base Sprite ID.
	_TILSetupRender(map,rnd)													// Set up the maths, the defaults etc.
	rnd.forceCompleteRedraw = 1 												// Forces a full render, not a scrolling one
endfunction

/// Functionally equivalent to TILMove() except that it tries to keep the map area in the display.
///	@param 	map 	Map data
///	@param 	rnd 	Rendering data
///	@param 	xf 		X Scroll position
///	@param 	yf 		Y Scroll position

function TILMoveLock(map ref as TILMap,rnd ref as TILRender,xf as float,yf as float)

endfunction

///	Adjust the display map so that the given scroll position is the top left corner of the display area.  The render
/// operates roughly as follows - an array of sprites, big enough to use the tile area are positioned in a grid. As the 
/// map scrolls, the positions and contents of these sprites are changed. Speed is obtained by only updating the sprite
/// contents when neccessary, fine scrolling of the sprite position is used otherwise. X and Y can be way outside the 
/// sprite boundaries, positions are not adjusted to fit.
///	@param 	map 	Map data
///	@param 	rnd 	Rendering data
///	@param 	xf 		X Scroll position
///	@param 	yf 		Y Scroll position

function TILMove(map ref as TILMap,rnd ref as TILRender,xf as float,yf as float)

	if rnd.isCentreOfRender 													// If coordinates are render centre.
		xf = xf - floor(rnd.xTileCount / 2)
		yf = yf - floor(rnd.yTileCount / 2)
	endif
	
	if rnd.isLockedToMap 														// Try to keep the rendering "all map"
		if xf + rnd.xTileCount > map.mapWidth then xf = map.mapWidth-rnd.xTileCount	// Lock to right/bottom
		if yf + rnd.yTileCount > map.mapHeight then yf = map.mapHeight-rnd.yTileCount
		if xf < 0 then xf = 0 														// Lock to left/top
		if yf < 0 then yf = 0
	endif
	SetSpriteDepth(rnd.backgroundSprite,rnd.layerDepth[1]+1)					// Put background immediately below lowest layer (first)
	
	xTile as integer:yTile as integer:xOffset as integer:yOffset as integer		// New position.
	xTile = floor(xf):yTile = floor(yf)											// Convert floats to new position.
	xOffset = (xf-xTile)*rnd.tileWidth:yOffset = (yf-yTile)*rnd.tileHeight
	if xOffset = rnd.tileWidth 
		xOffset = 0:inc xTile
	endif
	fullRedraw as integer = 1													// Set to '1' when complete repaint.	
	if xTile = rnd.xTile and yTile = rnd.yTile and rnd.forceCompleteRedraw = 0 	// Same square tile, just need to adjust positions.
		if xOffset = rnd.xOffset and yOffset = rnd.yOffset then exitfunction	// Hasn't change at all, so just exit.
		fullRedraw = 0															// We just need to reposition the current tiles.
	endif
	rnd.forceCompleteRedraw = 0 												// and clear the flag.
	
	rnd.xTile = xTile:rnd.yTile = yTile				 							// Copy tile position into the rendering structure
	rnd.xOffset = xOffset:rnd.yOffset = yOffset									// so we know where we are.

	xFrom as integer:xTo as integer:yFrom as integer:yTo as integer 			// Range of X,Y to render.
	xRightTrim as integer:yBottomTrim as integer 								// Trim x,y after this.
	xFrom = 0:yFrom = 0:xTo = rnd.xTileCount:yTo = rnd.yTileCount 				// Default values are the whole tile things + edge
	xRightTrim = xTo
	
	if xTile < 0 																// Off left of display.
		xFrom = -xTile															// Skip this many tiles.
		if xFrom >= rnd.xTileCount then xFrom = rnd.xTileCount+1 				// Erase at most this many, in case of major scrolling.
	endif
	if xTo + rnd.xTile  >= map.mapWidth											// Right hand side exceeds map width ?
		xRightTrim = map.mapWidth - rnd.xTile - 1								// Trim this many characters.
		xTo = xRightTrim
	endif
	if yTile < 0 																// Off top of display
		yFrom = -yTile															// Skip this many rows.
		if yFrom >= rnd.yTileCount then yFrom = rnd.yTileCount+1 				// Only this many at most.
	endif
	if yTo + rnd.yTile >= map.mapHeight 										// off bottom of display.
		yTo = map.mapHeight - rnd.yTile - 1
		if yTo < -1 then yTo = -1
	endif
	layer as integer:x as integer:y as integer									// Physical position on map.
	tile as integer 															// Tile at physical position
	mapIndex as integer															// Index into map
	spriteID as integer															// Current sprite ID
	tilePtr as integer 															// Index into tile table in renderer.
	xScreen as integer:yScreen as integer 										// Draw position on screen.
	depth as integer 															// Depth for this layer.
	n as integer

	for layer = 1 to map.layerCount 											// Work through each layer
		if rnd.layerEnabled[layer] <> 0											// If it is enabled.
			depth = rnd.layerDepth[layer]										// Saves an array look up :)
		
			if yFrom > 0 														// Blank space above
				for n = 0 to yFrom-1 											// So blank those rows.
					_TILErase(rnd,layer,0,n,rnd.xTileCount+1)
				next n
			endif
			for y = yFrom To yTo 												// Work through each row on this layer
																				// Index of tile entry, 1st tile
				tilePtr = rnd.lvlMultiplier * layer + rnd.rowMultiplier * y + xFrom + rnd.offset
				spriteID = tilePtr + rnd.baseSpriteID 							// Sprite ID, 1st tile
				mapIndex = xFrom+xTile+(y+yTile)*map.mapWidth					// Index into map, 1st tile
				xScreen = rnd.x+xFrom*rnd.tileWidth-xOffset						// Horizontal draw position, first tile.
				yScreen = rnd.y+rnd.tileHeight*y-yOffset						// Vertical draw position
				
				if xFrom > 0 then _TILErase(rnd,layer,0,y,xFrom)				// Erase LHS if scrolled off.

				for x = xFrom to xTo 											// For each tile
					tile = map.layers[layer].data[mapIndex] 					// What is on the map there ?
					if fullRedraw <> 0 and tile <> rnd.tileID[tilePtr]  		// If it is different to the tile currently there.
						if tile <> 0 											// Changed to something solid.
							if GetSpriteExists(spriteID)						// Sprite already there ?
								SetSpriteVisible(spriteID,1)					// Make visible.
								SetSpriteImage(spriteID,map.tileImages[tile])	// Update image.
							else
								CreateSprite(spriteID,map.tileImages[tile])		// No sprite, so create one.
								SetSpriteScissor(spriteID,rnd.x,rnd.y,rnd.x2,rnd.y2) // Clip to the window.
								SetSpriteDepth(spriteID,depth)					// Set depth and size correctly.
								SetSpriteSize(spriteID,rnd.tileWidth,rnd.tileHeight)
							endif
						else
							if GetSpriteExists(spriteID)						// Hide sprite if it exists, as changed to 0.
								SetSpriteVisible(spriteID,0)
							endif
						endif
						rnd.tileID[tilePtr] = tile 								// Update the tile table.
					endif
					if tile<>0 then SetSpritePosition(spriteID,xScreen,yScreen)	// Update position if a tile is present.
					inc spriteID 												// Move all indexes to the next tile.
					inc mapIndex
					inc tilePtr
					xScreen = xScreen + rnd.tileWidth 							// Screen position to next cell.
				next x
				if xRightTrim < rnd.xTileCount									// Erase RHS if scrolled off.
					if xTile >= map.mapWidth then xRightTrim = -1 				// Off screen completely ?
					_TILErase(rnd,layer,xRightTrim+1,y,rnd.xTileCount-xRightTrim) // Erase the tiles.
				endif
			next y
			for n = yTo+1 to rnd.yTileCount										// Blank any bottom rows.
				_TILErase(rnd,layer,0,n,rnd.xTileCount+1)
			next n
		endif
	next layer
endfunction

function _TILErase(rnd ref as TILRender,layer as integer,x as integer,y as integer,count as integer)
	pos as integer:n as integer
	pos = rnd.offset + rnd.lvlMultiplier * layer + rnd.rowMultiplier * y + x
	while count > 0
		SetSpriteVisible(pos+rnd.baseSpriteID,0)
		rnd.tileID[pos] = -1
		inc pos
		dec count
	endwhile
endfunction

//	Set up defaults, values, sprite table etc.
//	@param 	map 	Map data
//	@param 	rnd 	Rendering data

function _TILSetupRender(map ref as TILMap,rnd ref as TILRender)
	if rnd.width = 0 then rnd.width = GetVirtualWidth()							// Default values for width, height
	if rnd.height = 0 then rnd.height = GetVirtualHeight()		
	rnd.xTile = 0:rnd.yTile = 0:rnd.xOffset = 0:rnd.yOffset = 0 				// Reset to top left.
	if rnd.tileWidth = 0 then rnd.tileWidth = map.tileWidth						// Default values for tile width and height
	if rnd.tileHeight = 0 then rnd.tileHeight = map.tileHeight
	
	rnd.x2 = rnd.x + rnd.width 													// Calculate RHS and bottom
	rnd.y2 = rnd.y + rnd.height
	
	rnd.xTileCount = (rnd.width+rnd.tileWidth-1) / rnd.tileWidth 				// Visible map size (not including edging)	
	rnd.yTileCount = (rnd.height+rnd.tileHeight-1) / rnd.tileHeight

	rnd.rowMultiplier = rnd.xTileCount + 2 										// 2 extra tiles per row.
	rnd.lvlMultiplier = rnd.rowMultiplier * (rnd.yTileCount + 2)				// 2 extra tile rows per column.
	rnd.offset = rnd.rowMultiplier + 1 - rnd.lvlMultiplier 						// Adjust because levels index from one, allow boundary
	
	rnd.tileID.length = map.layerCount * rnd.lvlMultiplier 						// allocate memory for tileID table
	i as integer
	for i = 0 to rnd.tileID.length:rnd.tileID[i] = 0:next i 					// Clear tileID table.
		
	rnd.backgroundSprite = CreateSprite(0)										// Create solid sprite.
	SetSpritePosition(rnd.backgroundSprite,rnd.x,rnd.y)							// Create sprite used for background colour.
	SetSpriteSize(rnd.backgroundSprite,rnd.width,rnd.height)
	SetSpriteDepth(rnd.backgroundSprite,1)
	SetSpriteColor(rnd.backgroundSprite,map.background[1],map.background[2],map.background[3],255)
	
	rnd.layerDepth.length = map.layerCount 										// Array of depths for the various layers
	rnd.layerEnabled.length = map.layerCount									// Enabled flags for the layers
	for i = 1 to map.layerCount													// Initialise layer information.
		rnd.layerDepth[i] = 1000-(i-1) * 100
		rnd.layerEnabled[i] = 1 
	next i 	
endfunction

//	Unpack the RLE data in string into the TILObject. This is used by generated code (see test_tilecode.agc example)
//	@param obj 		TILObject , expanding data store
//	@param isCompressed non zero if the data is RLE compressed, if zero it's just copied verbatim.
//	@param data 	String which is a series of integer constants seperated by comma.

function _TILExpand(obj ref as _TILObject,isCompressed as integer,data as string)
	size as integer:p as integer:n as integer:c as integer
	size = CountStringTokens(data,",")											// How much data to write ?
	p = 1																		// Position in number list.
	while p <= size 															// while more to unpack.
		c = Val(GetStringToken(data,",",p)):n = 1:p = p + 1						// Read next token.
		if c < 0 and isCompressed <> 0 											// Repeat
			n = -c
			c = Val(GetStringToken(data,",",p))									// Get the thing that repeats.
			p = p + 1 															// Skip over them.
		endif 
		while n > 0
			if obj.data.length = obj.nextWritePosition  						// Expand array to fit in chunks
				obj.data.length = obj.data.length + 256
			endif
			obj.data[obj.nextWritePosition] = c 								// Write data there.
			obj.nextWritePosition = obj.nextWritePosition + 1 					// Advance write position.
			n = n - 1
		endwhile
	endwhile
endfunction
