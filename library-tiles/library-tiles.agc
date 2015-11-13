/// @name 	library-tiles
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix TIL
/// @provides tiles
/// @requires
/// @version 0.1 														
/// @created 13-Mar-15
/// @updated 22-Mar-15
/// @module

//	Note this version 2 of the tiles library, which has been rewritten from scratch.

#constant _TIL_HFLIP 	0x04000000												// Bitmasks for various parts of tile ID.
#constant _TIL_VFLIP 	0x08000000												// Note these are different to normal - the
#constant _TIL_HVFLIP 	0x0C000000												// flip bits are right shifted 4 because of 
#constant _TIL_MASK 	0x03FFFFFF												// maxint issues.

///
///	A collection of key/value properties which represent Tiled's Custom Properties.
///
Type TILPropertyCollection
	propertyCount as integer													/// Number of defined properties
	keys as string[1] 															/// Property keys (all lower case)
	values as string[1]															/// Property values (as string)
endtype
///
/// A structure containing information about a specific image - can be used both for tiles (in layers) and
/// in objects.
///
Type TILImage
	id as integer																/// Internal ID of image.
	gid as integer 																/// TMX file ID (e.g. original gid)
	imageID as integer 															/// Reference ID in Atlas.
	image as integer 															/// Loaded Image from Atlas
	width as integer 															///	Image width in pixels
	height as integer 															///	Image height in pixels
	properties as TILPropertyCollection 										///	custom properties associated with image
EndType
///
///	Object representation. Note that objectgroups are compressed into one entity and objects have different depths to 
/// differentiate them.
///
Type TILObject
	id as integer																///	Object ID as allocated by Tiled.
	imageID as integer 															/// Reference ID in tile/graphics file 
	image as integer 															///	Loaded image from atlas.
	depth as integer 															///	Depth index (1 = lowest layer/objectgrp)
	x as float 	  																/// x position on screen (bottom left)
	y as float 																	/// y position on screen (bottom left)
	width as float 																/// object width (0 = default)
	height as float 															///	object height (0 = default)
	rotation as float 															///	object rotation (around bottom left)
	properties as TILPropertyCollection 										/// custom properties for object
endtype
///
///	Represents a row on a map. Can be completely empty, which can be used to speed up rendering. Each row contains one 
///	extra zero tile (e.g. mapWidth+1 tiles) which is used in rendering the end tile. Note also that the flip bits have
///	been shifted four places right so as not to collide with AGKs slightly odd handling of large values close to maxint.
///
type TILRow
	tiles as integer[1] 														///	tile values with flip bits shifted four right, indexed from 0
	rowEmpty as integer 														///	non-zero if the row is empty.
	nextWrite as integer 														///	next row write address for unpack
Endtype
///
///	Represents a layer - a collection of rows.
///
type TILLayer
	name as string 																///	name of layer
	mapWidth as integer															///	width of map in tiles
	mapHeight as integer														/// height of map in tiles
	depth as integer 															///	depth (1 up, not sprite depth)
	rows as TILRow[1] 															/// Row descriptors, 0 to height-1
Endtype
///
///	Represents a map. Collections of images, objects and layers.
///
type TILMap
	name as string 																/// file name stub (e.g. no directory/tmx, lc)
	orientation as string 														/// map orientation (as in Tiled)
	layerCount as integer 														///	number of layers
	objectCount as integer 														/// number of objects (not object Groups)
	imageCount as integer														/// number of images
	layers as TILLayer[1] 														/// layers 
	objects as TILObject[1]														/// objects
	images as TILImage[1]														/// images.
	tileWidth as integer 														/// tile width
	tileHeight as integer														/// tile height
	mapWidth as integer															/// map width in tiles
	mapHeight as integer														/// map height in tiles
	background as integer[4]													///	background RGBA colours
	atlasImage as integer														///	main atlas image.
	depthCount as integer 														/// Number of depths involved.
EndType
//
//	Represents a single rendered row on the display.
//
type _TILRenderRow
	isClear as integer 															/// Non zero if this rendering row is empty.
	tiles as integer[1] 														/// tiles shown for each sprite currently. 
	sprites as integer[1]														/// sprite IDs where allocated.
endtype
//
//	Represents a single rendered layer
//
type _TILRenderLayer
	rows as _TILRenderRow[1]													/// One for each row.
endtype
///
///	Represents a single rendering of a map
///
type TILRender
	x as integer																/// top left position of rendering window
	y as integer																/// top left position of rendering window
	width as integer															/// width of window (if either width or height 0)
	height as integer															/// height of window (use full screen)
	tileWidth as integer 														/// requested tile width
	tileHeight as integer 														/// requested tile height
	renderWidth as integer 														/// Number of tiles across (rounded up + 1)
	renderHeight as integer 													/// Number of tiles down (rounded up + 1)
	baseDepth as integer 														/// Highest sprite depth of map (100)
	hasBeenProcessed as integer 												/// set to 1 when processing has taken place.
	xCoarse as integer 															/// Coarse scroll position horizontal (tiles)
	yCoarse as integer 															/// Coarse scroll position vertical (tiles)
	xFine as integer 															/// Fine scroll horizontal (pixels)
	yFine as integer 															/// Fine scroll vertical (pixels)
	xNewFine as integer 														/// New Fine scroll horizontal (pixels)
	yNewFine as integer 														/// New Fine scroll vertical (pixels)
	forceRepaint as integer 													/// If non-zero do a complete repaint.
	layers as _TILRenderLayer[1]												/// Layers, where layers exist.
	spriteDepths as integer[1]													/// Sprite Depths equivalent of various sprites.
	backgroundSprite as integer 												/// ID of background sprite.
	diamondSize as integer 														/// Isometric diamond size.
endtype

///	Set the scrolling position of a map to the origin which is either the top left position (orthogonal) or the
///	top centre position (isometric).
///	@param  map 	Map it is used with
///	@param 	rnd 	Renderer to default for.
///	@param  xf 		Horizontal Float position on map.
///	@param  yf 		Vertical Float position on map.

function TILSetScroll(map ref as TILMap,rnd ref as TILRender,xf as float,yf as float)
	if rnd.hasBeenProcessed = 0 then _TILDefaults(rnd,map)						// Process if required.
	newX as integer:newY as integer 											// Calculate coarse position.
	newX = floor(xf):newY = floor(yf) 
	rnd.xNewFine = (xf-newX)*rnd.tileWidth										// Calculate fine position.
	rnd.yNewFine = (yf-newY)*rnd.tileHeight
	if newX <> rnd.xCoarse or newY <> rnd.yCoarse or rnd.forceRepaint <> 0 		// If coarse changed, or repaint forced.
		rnd.xCoarse = newX:rnd.yCoarse = newY									// Update coarse position.
		rnd.forceRepaint = 0													// Clear forced repaint
		_TILRepaintAllRows(map,rnd,xf,yf)										// And redo all rows.
	else 
		if rnd.xNewFine <> rnd.xFine or rnd.yNewFine <> rnd.yFine 				// Has the fine scroll changed ?
			rnd.xFine = rnd.xNewFine:rnd.yFine = rnd.yNewFine 					// Update fine scroll position.
			if map.orientation = "orthogonal"
				_TILShiftAllRows(map,rnd)										// Reposition all visible rows to use new finescroll.
			else
				//_TILRepaintAllRows(map,rnd,xf,yf)
			endif
		endif
	endif
endfunction

//	Shift all rows according to fine x - this allows fine scrolling in orthogonal tiling only.
//	@param  map 	Map it is used with
//	@param 	rnd 	Renderer to default for.

function _TILShiftAllRows(map ref as TILMap,rnd ref as TILRender)
	scnRow as integer															// Current tile position vertical
	scnCol as integer															// Current tile position horizontal
	xScreen as integer 															// Pixel position horizontal
	yScreen as integer 															// Pixel position vertical.
	xIncrement as integer 														// x change per row.
	yIncrement as integer 														// y change per row. 
	layer as integer															// Current layer.
	spriteID as integer															// sprite used for current screen position.

	xIncrement = rnd.tileWidth:yIncrement = 0									// Increments for orthogonality.
	
	for layer = 1 to map.layerCount 											// Work through all layers
		for scnRow = 0 to rnd.renderHeight										// Work through all rows
			xScreen = rnd.x + 0 * rnd.tileWidth - rnd.xFine+rnd.tileWidth/2	// Calculate physical screen position - offset is
			yScreen = rnd.y + (scnRow+1) * rnd.tileHeight-rnd.yFine			// bottom middle.
			if rnd.layers[layer].rows[scnRow].isClear = 0 						// If something in that row
				for scnCol = 0 to rnd.renderWidth-1								// Work through it.
					spriteID = rnd.layers[layer].rows[scnRow].sprites[scnCol] 	// If found something
					if spriteID <> 0 											// Then reposition it
						SetSpritePositionByOffset(spriteID,xScreen,yScreen)
					endif
					xScreen = xScreen + xIncrement 								// Next screen position.
					yScreen = yScreen + yIncrement
				next scnCol
			endif
		next scnRow
	next layer
endfunction

//	Repaint all the rows - occurs when the coarse position has changed (e.g. a different tile at the origin)
//	@param  map 	Map it is used with
//	@param 	rnd 	Renderer to default for.
//	@param  xf 		Horizontal Float position on map.
//	@param  yf 		Vertical Float position on map.


function _TILRepaintAllRows(map ref as TILMap,rnd ref as TILRender,xf as float,yf as float)
	xFrom as integer															// Horizontal redraw area, screen tiles
	xTo as integer
	yFrom as integer															// Vertical redraw area, screen tiles
	yTo as integer
	scnRow as integer															// Current tile position vertical
	scnCol as integer															// Current tile position horizontal
	mapRow as integer															// Current map position vertical
	mapCol as integer															// Current map position horizontal
	xScreen as integer 															// Pixel position horizontal
	yScreen as integer 															// Pixel position vertical.
	xIncrement as integer 														// x change per row.
	yIncrement as integer 														// y change per row. 
	layer as integer															// Current layer.
	mapTileID as integer														// tile at current tile position on map
	spriteID as integer															// sprite used for current screen position.
	isIsometric as integer 														// True if isometric.

	isIsometric = map.orientation = "isometric" 								// is this isometric ?
	
	rnd.xFine = rnd.xNewFine:rnd.yFine = rnd.yNewFine 							// Update fine position.
	
	yFrom = 0:yTo = rnd.renderHeight-1 											// Range of rows for which rendering will occur.
	xFrom = 0:xTo = rnd.renderWidth-1 											// Range of cols for which rendering will occur.
	
	if isIsometric
		if rnd.diamondSize > xTo then rnd.diamondSize = xTo
		if rnd.diamondSize > yTo then rnd.diamondSize = yTo
		xTo = rnd.diamondSize-1:yTo = rnd.diamondSize-1
	endif 
	
	if rnd.yCoarse < 0 then yFrom = -rnd.yCoarse 								// Calculate bounds for the row drawing.
	if yTo + rnd.yCoarse >= map.mapHeight then yTo = map.mapHeight - rnd.yCoarse - 1
	if rnd.xCoarse < 0 then xFrom = -rnd.xCoarse								// And the column drawing.
	if xTo + rnd.xCoarse >= map.mapWidth then xTo = map.mapWidth - rnd.xCoarse - 1
	if yFrom > rnd.renderHeight then yFrom = rnd.renderHeight					// limit upwards
	if yTo < -1 then yTo = -1													// limit downwards
	if xFrom > rnd.renderWidth then xFrom = rnd.renderWidth
	if xTo < -1 then xTo = -1
	
	xIncrement = rnd.tileWidth:yIncrement = 0									// Increments for orthogonality.
	if isIsometric 																// If isometric.
		xIncrement = rnd.tileWidth/2:yIncrement = rnd.tileHeight/2
	endif
		
	for layer = 1 to map.layerCount												// For each layer on the map.
		for scnRow = 0 to yFrom-1 												// Erase rows above map if required
			_TILClearRow(rnd.layers[layer].rows[scnRow])
		next scnRow
		for scnRow = yTo+1 to rnd.renderHeight-1 								// Erase rows below map if required.
			_TILClearRow(rnd.layers[layer].rows[scnRow])
		next scnRow
		for scnRow = yFrom to yTo 												// For each row on the screen.
			if isIsometric
				xScreen = rnd.x + rnd.width / 2 - scnRow * rnd.tileWidth / 2 
				yScreen = rnd.y + (scnRow+2) * rnd.tileHeight/2
			else
				xScreen = rnd.x + xFrom * rnd.tileWidth - rnd.xFine+rnd.tileWidth/2	// Calculate physical screen position - offset is
				yScreen = rnd.y + (scnRow+1) * rnd.tileHeight-rnd.yFine				// bottom middle.
			endif
			mapRow = rnd.yCoarse + scnRow 										// Calculate row in map.
			if map.layers[layer].rows[mapRow].rowEmpty 							// Is the map row empty ?
				_TILClearRow(rnd.layers[layer].rows[scnRow])					// then clear it in the physical layer.
			else 
				rnd.layers[layer].rows[scnRow].isClear = 0 						// Row is not clear now, we are putting something there.
				_TILClearRowPart(rnd.layers[layer].rows[scnRow],0,xFrom-1)		// Erase side if required as off map.
				_TILClearRowPart(rnd.layers[layer].rows[scnRow],xTo+1,rnd.renderWidth-1)
				for scnCol = xFrom to xTo 										// For each column on the screen
					mapCol = scnCol + rnd.xCoarse 								// Calculate the map column
					mapTileID = map.layers[layer].rows[mapRow].tiles[mapCol] 	// Get the tile currently there on the map.
					spriteID = rnd.layers[layer].rows[scnRow].sprites[scnCol] 	// Get the sprite for this screen tile
					if mapTileID <> 0											// If something there on the map.
						if spriteID = 0 										// If there is no sprite here, create a new one.
							spriteID = CreateSprite(map.images[1].image) 		// Create a working sprite.
							rnd.layers[layer].rows[scnRow].sprites[scnCol] = spriteID 	// Save the sprite ID into renderer
							if isIsometric
								SetSpriteSize(spriteID,rnd.tileWidth,-1)
								SetSpriteDepth(spriteID,rnd.spriteDepths[map.layers[layer].depth+1]-(scnRow+scnCol)/2)
								SetSpriteOffset(spriteID,rnd.tileWidth/2,GetSpriteHeight(spriteID))
							else
								SetSpriteSize(spriteID,rnd.tileWidth,rnd.tileHeight) // Set width, height, depth, clip etc.
								SetSpriteDepth(spriteID,rnd.spriteDepths[map.layers[layer].depth])
								SetSpriteOffset(spriteID,rnd.tileWidth/2,rnd.tileHeight)
							endif
							SetSpriteScissor(spriteID,rnd.x,rnd.y,rnd.x+rnd.width,rnd.y+rnd.height)
							rnd.layers[layer].rows[scnRow].tiles[scnCol] = -1 	// This forces the image to change.
						else
							SetSpriteVisible(spriteID,1) 						// It may have been made invisible by being hidden.
						endif
						if mapTileID<>rnd.layers[layer].rows[scnRow].tiles[scnCol] // Has the image changed ? If so update it and flips.
							SetSpriteImage(spriteID,map.images[mapTileID && _TIL_MASK].image)
							if isIsometric										// Reset the size.
								SetSpriteSize(spriteID,rnd.tileWidth,-1)
							else
								SetSpriteSize(spriteID,rnd.tileWidth,rnd.tileHeight) 
							endif
							SetSpriteFlip(spriteID,mapTileID && _TIL_HFLIP,mapTileID && _TIL_VFLIP)
							rnd.layers[layer].rows[scnRow].tiles[scnCol]=mapTileID  // update saved image in renderer.
						endif
						SetSpritePositionByOffset(spriteID,xScreen,yScreen)
					else 														// This tile is empty on the map.
						if spriteID <> 0 										// If sprite present, hide it.
							SetSpriteVisible(spriteID,0)
						endif
						rnd.layers[layer].rows[scnRow].tiles[scnCol] = 0 		// update the tile table to reflect this change.
					endif
					xScreen = xScreen + xIncrement 								// Next screen position.
					yScreen = yScreen + yIncrement
				next scnCol
			endif
		next scnRow
	next layer
endfunction

// Clear a whole row of the display to the 'cleared' state, if required.
// @param row 	The row to be cleared.

function _TILClearRow(row ref as _TILRenderRow)
	if row.isClear = 0 															// Don't bother if already cleared
		row.isClear = 1															// Mark now as cleared
		i as integer
		for i = 0 to row.sprites.length											// Hide all sprites in that row.
			if row.sprites[i] <> 0 then SetSpriteVisible(row.sprites[i],0)
		next i
	endif
endfunction

// Clear a part of the row.
// @param row 	The row to be cleared.
// @param fromCol Column from
// @param toCol Column to

function _TILClearRowPart(row ref as _TILRenderRow,fromCol as integer,toCol as integer)
	i as integer
	for i = fromCol to toCol														// Hide all sprites that are required.
		if row.sprites[i] <> 0 then SetSpriteVisible(row.sprites[i],0)
	next i
endfunction

//	Install various defaults into TILRender, set up the various arrays.
//	@param 	rnd 	Renderer to default for.
//	@param  map 	Map it is used with

function _TILDefaults(rnd ref as TILRender,map ref as TILMap)
	if rnd.width = 0 or rnd.height = 0 											// width or height unspecified, use full screen.
		rnd.x = 0:rnd.y = 0
		rnd.width = GetVirtualWidth():rnd.height = GetVirtualHeight()
	endif 
	if rnd.baseDepth = 0 then rnd.baseDepth = 100 								// Base depth.
	if rnd.tileWidth = 0 then rnd.tileWidth = map.tileWidth						// Default tile width/height from map.
	if rnd.tileHeight = 0 then rnd.tileHeight = map.tileHeight

	rnd.renderWidth = floor((rnd.width + rnd.tileWidth - 1) / rnd.tileWidth)+1 	// Set the renders.
	rnd.renderHeight = floor((rnd.height + rnd.tileHeight - 1) / rnd.tileHeight) + 1

	if map.orientation = "isometric"
		xMax as integer:xMax = floor(floor(rnd.width / 2) / floor(rnd.tileWidth/2))
		yMax as integer:yMax = floor(rnd.height / floor(rnd.tileHeight/1)) 
		if yMax < xMax then xMax = yMax
		rnd.diamondSize = xMax
		rnd.renderHeight = rnd.diamondSize * 2
	endif
	
	i as integer:j as integer:l as integer
	rnd.layers.length = map.layerCount 											// Now create the layers
	for l = 1 to map.layerCount													// For each layer
		rnd.layers[l].rows.length = rnd.renderHeight							// Allocate row space.
		for i = 0 to rnd.layers[l].rows.length 									// then for each row.
			rnd.layers[l].rows[i].isClear = 1									// will have nothing in it.
			rnd.layers[l].rows[i].tiles.length = rnd.renderWidth				// set lengths of arrays
			rnd.layers[l].rows[i].sprites.length = rnd.renderWidth			
			for j = 0 to rnd.renderWidth										// and erase them.
				rnd.layers[l].rows[i].tiles[j] = 0
				rnd.layers[l].rows[i].sprites[j] = 0
			next j
		next i
	next l
	rnd.spriteDepths.length = map.depthCount+1 									// Calculate the sprite depths (add one for background)
	for i = 0 to rnd.spriteDepths.length
		rnd.spriteDepths[i] = rnd.baseDepth + (rnd.spriteDepths.length-i) * (rnd.renderHeight+5)
	next i
	
	if map.orientation = "orthogonal"
		rnd.backgroundSprite = CreateSprite(0)									// Create background sprite.
		SetSpritePosition(rnd.backgroundSprite,rnd.x,rnd.y)						// Position it
		SetSpriteSize(rnd.backgroundSprite,rnd.width,rnd.height)
		SetSpriteDepth(rnd.backgroundSprite,rnd.spriteDepths[0])				// Set depth and background colour.
		SetSpriteColor(rnd.backgroundSprite,map.background[0],map.background[1],map.background[2],map.background[3])
	endif
	rnd.forceRepaint = 1 														// Force repaint whatever.
	rnd.hasBeenProcessed = 1													// Has now been processed.
endfunction

//  ***************************************************************************************************************************
// 	These routines are used to unpack the data generated by tmxconvert.py - they are thus very closely tied to that
//	program.
//  ***************************************************************************************************************************
//	Unpack a row into the TILRow structure, this is added to whatever is there already. This is RLE encoded.
//	@param row	Row to unpack tile data into.
//	@param code comma seperated RLE encoded data.

function _TILRowUnpack(row ref as TILRow,code as string)
	n as integer = 1															// position in code.
	size as integer:size = CountStringTokens(code,",")							// last position
	count as integer:data as integer						
	while n <= size																// until done all code.
		count = 1																// normally it is 1 of the data here.
		data = val(GetStringToken(code,",",n))
		n = n + 1
		if data < 0																// however, a -ve value means repeat that many
			count = -data														// times (- times !) and the data to repeat follows
			data = val(GetStringToken(code,",",n))
			n = n + 1
		endif
		while count > 0 														// write count data items out.
			row.nextWrite = row.nextWrite+1 									// next write position
			row.tiles.length = row.nextWrite									// is long enough (AGK2 does this in chunks)
			row.tiles[row.nextWrite-1] = data 									// write data out
			count = count - 1													// count times.
		endwhile
	endwhile
endfunction

//	Unpack a property collection string (semicolon seperated, alternating keys and values) into a property 
//	structure.
//	@param 	propcol		property to unpack into.
//	@param 	props 		properties, encoded.
function _TILPropertyUnpack(propcol ref as TILPropertyCollection,props as string)
	i as integer
	propcol.propertyCount = CountStringTokens(props,";")/2						// there are two tokens per property (key+value)
	propcol.keys.length = propcol.propertyCount 								// assign array space.
	propcol.values.length = propcol.propertyCount
	for i = 1 to propcol.propertyCount 											// And copy them in
		propcol.keys[i] = GetStringToken(props,";",i*2-1)						// in pairs.
		propcol.values[i] = GetStringToken(props,";",i*2)
	next i
endfunction

//	Unpack an image structure (id:gid:imageID:width:height:properties) into an image structure
//	@param image 	image to unpack into.
//	@param src 		image descriptor.

function _TILImageUnpack(image ref as TILImage,src as string)
	image.id = Val(GetStringToken(src,":",1))									// Extract values
	image.gid = Val(GetStringToken(src,":",2))
	image.imageID = Val(GetStringToken(src,":",3))
	image.width = Val(GetStringToken(src,":",4))
	image.height = Val(GetStringToken(src,":",5))
	if GetStringToken(src,":",6) <> "" 											// If there are some properties
		_TILPropertyUnpack(image.properties,GetStringToken(src,":",6))			// Unpack those too.
	endif
endfunction

//	Unpack an object structure (id:imageID:depth:x:y:width:height:rotation:properties)
//	@param object 	object struture
//	@param src 		image descriptor.

function _TILObjectUnpack(object ref as TILObject,src as string)
	object.id = Val(GetStringToken(src,":",1))									// Extract values.
	object.imageID = Val(GetStringToken(src,":",2))
	object.depth = Val(GetStringToken(src,":",3))
	object.x = ValFloat(GetStringToken(src,":",4))
	object.y = ValFloat(GetStringToken(src,":",5))
	object.width = ValFloat(GetStringToken(src,":",6))
	object.height = ValFloat(GetStringToken(src,":",7))
	object.rotation = ValFloat(GetStringToken(src,":",8))
	if GetStringToken(src,":",9) <> "" 											// If there are some properties
		_TILPropertyUnpack(object.properties,GetStringToken(src,":",9))			// Extract them.
	endif
endfunction

