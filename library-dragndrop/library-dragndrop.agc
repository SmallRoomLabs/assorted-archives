/// <p>This library provides basic drag and drop functionality. It can handle both sprites, text and the whole screen.
/// Each object you want to involve can be categorised as a clickable, targettable and draggable (except the whole
/// screen cannot be dragged). Clickable objects can be clicked on. Draggable objects can be dragged around the screen.
/// Targettable objects are recipients for draggables (this can include the whole display). Objects can have all three
///	behaviours - they can be clicked on, clicked and dragged, and have other things dropped on them. Successful clicks
/// or drops are recorded by a value being returned from the main function (DNDHandlePointer). A structure (DVDEventInfo)
/// is passed to the DNDHandlePointer() function so the caller knows what has been clicked, dragged, dropped etc.</p><p>
///	Note that this function deliberately leaves everything in the same position when it has finished. If you drag and drop
/// something from one place to another it will record this but it will end up in the same state it started in. <i>If you
/// want to physically move it you must do it yourself by responding to the callback</i> - the example code shows 
///	how to do this. </p><p>
/// Note that objects that are clickable and draggable respond to click on button release - because when the button goes
/// down they don't know which it is.</p>

/// @name 	library-dragndrop
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix DND
/// @provides dragndrop
/// @requires
/// @version 0.9 
/// @created 18-Feb-15
/// @updated 27-Feb-15
/// @module

#constant DND_DISPLAY 			1												// Internal ID for 'whole screen'.

#constant DND_SPRITE 			2 												// Sprite type ID
#constant DND_TEXT 				3 												// Text type ID

#constant DND_NOTHING			10 												// No event
#constant DND_CLICK 			11 												// Something clicked
#constant DND_DROPPED 			12 												// Successful drop occurred

#constant _DND_STATE_WAITING 	101												// Waiting for button press.
#constant _DND_STATE_PRESS 		102 											// Pressed but not yet dragging.
#constant _DND_STATE_DRAGGING	103 											// Dragging object around screen.
#constant _DND_STATE_RETURNING 	104												// Object returning to original point.

#constant _DND_DRAG_DELAY 		500 											// ms hold down for drag to start.
#constant _DND_ELASTIC_DELAY 	200 											// ms time for ping back on failed drop.

global _DND_Error_Fired as integer = 0 											// Stops multiple errors.

///	This is a utility type representing a point in 2D space.

type DNDPoint																	
	x as float																	// X coordinate
	y as float																	// Y Coordinate
endtype

///	This structure is passed when a callback event occurs - this means at present that a drop
///	has successfully happened.

type DNDEventInfo 													
	event as integer 															/// What happened (DND_NOTHING/CLICK/DROPPED)
	clickedObjectType as integer 												/// What was clicked (type)
	clickedObjectID as integer 													/// What was clicked (id)
	draggedObjectType as integer 												/// What was dragged (type)
	draggedObjectID as integer 													/// What was dragged (id)
	droppedObjectID as integer 													/// Upon what object was it dropped
	offset as DNDPoint 															/// Where on object was it dropped ?
endtype

///	This structure defines a single drag and drop group. Its internal structure should not be assumed.

type DNDControl
	state as integer 															/// State.
	pointerState as integer 													/// Button state.
	lastPointerState as integer 												/// Previous button state
	dragObjectID as integer 													/// Object identified as being dragged
	dragPhysicalID as integer 													/// Object that is actually being physically moved.
	dragWidth as float 															/// Width of drag object
	dragHeight as float 														/// Height of drag object.
	pointer as DNDPoint 														/// Pointer position.
	originPointer as DNDPoint 													/// Pointer position on first click.
	offset as DNDPoint 															/// Offset from click point to object centre.
	origin as DNDPoint 															/// Where we started from (position of sprite/text)
	endPoint as DNDPoint 														/// Where the drag ended (position of sprite/text)
	isClickable as integer 														/// 1 if identified object is clickable
	isDraggable as integer 														/// 1 if identified object is draggable
	msClickTime as integer 														/// Time in ms when object clicked
	msDropTime as integer 														/// Time in ms when dropped.
	draggable as integer[0]														/// IDs of objects that can be dragged
	clickable as integer[0]														/// IDs of object that can be clicked.
	targettable as integer[0]													/// IDs of object that can be dropped on.
endtype

///	Initialise a Drag and Drop control object, ready to use.
///	@param ref 	DND Control object

function DNDInitialise(dnd ref as DNDControl)
	dnd.state = _DND_STATE_WAITING												// In waiting state.
	dnd.draggable.length=0:dnd.clickable.length=0:dnd.targettable.length=0		// Clear arrays of involved sprite/text objects.
endfunction

///	Destroy a drag and drop control object
///	@param dnd 	DND Control object

function DNDDestroy(dnd ref as DNDControl)
	dnd.state = -1																// An illegal state, causing the main method to fail.
endfunction

///	Get Button Pressed with DND Interface - this handles all of the dragging and the dropping. This function returns non-zero
///	if either a drag/drop or click event has occurred, and in this case the description of that event will be in the 
/// DNDEventInfo structure passed in to the call.
///	@param dnd 	DND Control object
/// @param dei Return info regarding what happened, if anything.
///	@return non zero if click or drop occurred.

function DNDHandlePointer(dnd ref as DNDControl,dei ref as DNDEventInfo)
	
	n as integer	
	
	dei.event = DND_NOTHING:dei.clickedObjectID = 0:dei.draggedObjectID = 0 	// Erase the return value.
	dei.droppedObjectID = 0:dei.offset.x = 0:dei.offset.y = 0
	dei.clickedObjectType = DND_SPRITE:dei.draggedObjectType = DND_SPRITE
	
	if dnd.state <= 0 then _DNDError("Not initialised") 						// Not initialised, or destroyed.
	
	dnd.lastPointerState = dnd.pointerState 									// Update pointer state.
	dnd.pointerState = GetPointerState()
	dnd.pointer.x = GetPointerX():dnd.pointer.y = GetPointerY()
	
	select dnd.state															// Tiny state machine.
		case _DND_STATE_WAITING													// ** Waiting for a button click **
			if dnd.pointerState <> 0 and dnd.lastPointerState = 0				// Was a button pressed ?
				n = _DNDStartClickCode(dnd)										// Start click code.
				if n <> 0 then dei.event = DND_CLICK:dei.clickedObjectID = n 	// Was something clicked ?
			endif
		endcase
		
		case _DND_STATE_PRESS													// ** Waiting for Drag to Start **
			if dnd.pointerState = 0												// Has the pointer been released while waiting for drag?
				if dnd.isClickable <> 0 and dnd.isDraggable <> 0 				// If it is clickable and draggable.
					dei.event = DND_CLICK 										// Then this time it has been clicked.
					dei.clickedObjectID = dnd.dragObjectID
				endif
				dnd.state = _DND_STATE_WAITING									// and it is in waiting state.
			else 
																				// Is it time to drag now, or have we already dragged ?
				if GetMilliseconds() > dnd.msClickTime + _DND_DRAG_DELAY or _DNDCheckDragging(dnd)		
					dnd.state = _DND_STATE_DRAGGING								// Switch to drag state
				endif
			endif
		endcase
		
		case _DND_STATE_DRAGGING												// ** Physical dragging taking place **
			dnd.endPoint.x = dnd.pointer.x - dnd.dragWidth/2 - dnd.offset.x
			dnd.endPoint.y = dnd.pointer.y - dnd.dragHeight/2 - dnd.offset.y
			_DNDMovePhysicalDragObject(dnd,dnd.endPoint.x,dnd.endPoint.y)		// Track the cursor.
			if dnd.pointerState = 0 											// Has the button been released ?
																				// Attempt to drop using the centre of drag object as tgt point
				if _DNDAttemptDrop(dnd,dei,dnd.endPoint.x+dnd.dragWidth/2,dnd.endPoint.y+dnd.dragHeight/2) <> 0 		
					dnd.state = _DND_STATE_WAITING								// If so, do it again from the start
				else
					dnd.state = _DND_STATE_RETURNING							// Pingback				
					dnd.msDropTime = GetMilliseconds()							// Remember when.
				endif
			endif
		endcase
		
		case _DND_STATE_RETURNING												// ** Dragged object returning to origin **
			prop as float
			prop = (GetMilliseconds()-dnd.msDropTime)*1.0 / _DND_ELASTIC_DELAY	// Work out proportion through returning.
			if prop > 1.0 
				dnd.state = _DND_STATE_WAITING									// If so, do it again from the start
				_DNDMovePhysicalDragObject(dnd,dnd.origin.x,dnd.origin.y)		// Reposition it where it should be.
			else
				x as float:y as float											// Work out where it is in the flyback
				x = dnd.endPoint.x + (dnd.origin.x - dnd.endPoint.x) * prop * prop
				y = dnd.endPoint.y + (dnd.origin.y - dnd.endPoint.y) * prop * prop
				_DNDMovePhysicalDragObject(dnd,x,y)								// and reposition it.
			endif
		endcase
	endselect
	
	if dei.clickedObjectID < 0 													// handle -ve object IDs (e.g. text)
		dei.clickedObjectID = -dei.clickedObjectID:dei.clickedObjectType = DND_TEXT
	endif
	if dei.draggedObjectID < 0
		dei.draggedObjectID = -dei.draggedObjectID:dei.draggedObjectType = DND_TEXT
	endif
	
endfunction dei.event <> DND_NOTHING 											// Return non zero if something happened.

//	Handle start of click code. Identify what has been clicked, check if it is draggable, if so
//	start the draggable wait. Different code for clickable only and clickable/draggable which is detected
//	on release.
//	@param dnd 	DND Control object
//	@return object (+ or -). Returns 0 if not clicked on anything, so DNDGetButtonPressed.

function _DNDStartClickCode(dnd ref as DNDControl)
	retVal as integer = 0
	_DNDLocateObject(dnd,dnd.pointer.x,dnd.pointer.y)							// Find out the object that was clicked on.
	dnd.originPointer = dnd.pointer
	if dnd.dragObjectID <> 0 													// Was something found.
		dnd.dragPhysicalID = dnd.dragObjectID									// Object being physically dragged is the clicked on.
																				// (e.g. no cloning)
		if dnd.dragPhysicalID < 0 												// Figure out the size of the physical ID object
			dnd.dragWidth = GetTextTotalWidth(-dnd.dragPhysicalID)
			dnd.dragHeight = GetTextTotalHeight(-dnd.dragPhysicalID)
		else
			dnd.dragWidth = GetSpriteWidth(dnd.dragPhysicalID)
			dnd.dragHeight = GetSpriteHeight(dnd.dragPhysicalID)
		endif
																
		dnd.isClickable = _DNDIsInArray(dnd.clickable,dnd.dragObjectID)	> 0		// Set is clickable and is draggable.
		dnd.isDraggable = _DNDIsInArray(dnd.draggable,dnd.dragObjectID) > 0		// check to see if object is in arrays.

		if dnd.isClickable <> 0 and dnd.isDraggable = 0 						// if it is just clickable and not draggable.
			retVal = dnd.dragObjectID 											// Then return this object ID, it's been clicked.
		else
			dnd.msClickTime = GetMilliseconds()									// Set click time.
			dnd.state = _DND_STATE_PRESS 										// Switch to 'press' state.
			if dnd.dragObjectID < 0 											// Save the start point.
				dnd.origin.x = GetTextX(-dnd.dragObjectID)
				dnd.origin.y = GetTextY(-dnd.dragObjectID)
			else
				dnd.origin.x = GetSpriteX(dnd.dragObjectID)
				dnd.origin.y = GetSpriteY(dnd.dragObjectID)
			endif
			retVal = 0															// Return zero.
		endif
	endif
endfunction retVal

//	Attempt to drop at x,y
//	@param dnd 	DND Control object
//	@param info If drop was successful, details of the drop are stored here.
//	@param x 	x position of click
//	@param y 	y position of click
//	@return non-zero if okay.

function _DNDAttemptDrop(dnd ref as DNDControl,info ref as DNDEventInfo,x as float,y as float)
	i as integer:target as integer = 0
	depth as integer:bestDepth as integer = 99999
	eventInfo as DNDEventInfo
	
	for i = 1 to dnd.targettable.length 										// Check all possible drop places.
		if _DNDGetObjectCollision(dnd.targettable[i],x,y,eventInfo.offset) <> 0	// Has it landed ?
			depth = _DNDGetObjectDepth(dnd.targettable[i])						// Get depth 
			if depth < bestDepth 												// Above any other targettable objects
				bestDepth = depth:target = dnd.targettable[i]					// Record as best so far
			endif
		endif
	next i
	if target <> 0 																// Did we find something ?
		info.event = DND_DROPPED
		info.draggedObjectID = dnd.dragObjectID 								// What we dropped
		info.droppedObjectID = target 											// Where we dropped it.
		if target = DND_DISPLAY													// Correct for display
			info.offset = dnd.endPoint 											// Dropping on desktop.
		elseif target < 0
			info.offset.x = x - GetTextX(-target) - dnd.dragWidth/2				// Dropping on text
			info.offset.y = y - GetTextY(-target) - dnd.dragHeight/2
		else
			info.offset.x = x - GetSpriteX(target) - dnd.dragWidth/2			// Dropping on sprite
			info.offset.y = y - GetSpriteY(target) - dnd.dragHeight/2		
		endif
		_DNDMovePhysicalDragObject(dnd,dnd.origin.x,dnd.origin.y)				// Put the dragging object back to the start.
	endif
endfunction target <> 0

//	A click has occurred at (x,y), identify which object it is, then update the dnd.offset and dnd.dragObjectID values
//	@param dnd 	DND Control object
//	@param x 	x position of click
//	@param y 	y position of click

function _DNDLocateObject(dnd ref as DNDControl,x as float,y as float)
	bestDepth as integer = 99999 												// Best depth found so far.
	i as integer:depth as integer
	offset as DNDPoint
	dnd.dragObjectID = 0
	for i = 1 to dnd.clickable.length 											// Check clickables.
		if _DNDGetObjectCollision(dnd.clickable[i],x,y,offset) <> 0
			depth = _DNDGetObjectDepth(dnd.clickable[i])
			if depth < bestDepth then dnd.dragObjectID = dnd.clickable[i]
		endif
	next i
	for i = 1 to dnd.draggable.length 											// Check draggables.
		if _DNDGetObjectCollision(dnd.draggable[i],x,y,offset) <> 0
			depth = _DNDGetObjectDepth(dnd.draggable[i])
			if depth < bestDepth then dnd.dragObjectID = dnd.draggable[i]
		endif
	next i
	if dnd.dragObjectID <> 0 													// Copy the offset if something found
		_DNDGetObjectCollision(dnd.dragObjectID,x,y,dnd.offset)
	endif
endfunction

//	Make the current drag object move to a particular place, adjusted for the object.
//	@param dnd 	DND Control object
//	@param x 	x position of click
//	@param y 	y position of click

function _DNDMovePhysicalDragObject(dnd ref as DNDControl,x as float,y as float)
	if dnd.dragPhysicalID > 0 																	// Sprite object
		SetSpritePosition(dnd.dragPhysicalID,x,y)
	else 																		// Text object
		SetTextPosition(-dnd.dragPhysicalID,x,y)
	endif 
endfunction

//	Check if dragged from original point. Dragging started either by time, or by movement.
//	@param dnd 	DND Control object
//	@return non-zero if dragged

function _DNDCheckDragging(dnd ref as DNDControl)
	x as float:x = abs(dnd.pointer.x - dnd.originPointer.x)						// X Distance from start
	y as float:y = abs(dnd.pointer.y - dnd.originPointer.y)						// Y Distance from start
	retVal as integer
	retVal = x > GetVirtualWidth()/100 and y > GetVirtualHeight()/100				// Start dragging if moved far enough.
endfunction retVal

//	Check to see if a value is in an array.
//	@param arr	 array to search.
//	@param value value to search for.
//	@return index of value in array or -1 if not found.

function _DNDIsInArray(arr ref as integer[],value as integer)
	retval as integer = -1:i as integer
	for i = 1 to arr.length														// Scan through looking for value
		if arr[i] = value 
			retVal = i
			exit
		endif
	next i
endfunction retval

/// 	Add an index to one of the DCT arrays.
///		@param ref 	DND Control object
///		@param storage which to go in combinations of D,C,T
///		@param otype type of object to add..
///		@param index object to add

function DNDAddObject(dnd ref as DNDControl,storage as string,otype as integer,index as integer)
	
	if otype = DND_DISPLAY														// If Display, then add is index display sprite
		otype = DND_SPRITE:index = DND_DISPLAY
	endif
	
	if otype <> DND_SPRITE and otype <> DND_TEXT then _DNDError("Bad object type")
	
	if oType = DND_TEXT then index = -index 									// internally we refer to types using -ve references.
	
	i as integer
	for i = 1 to len(storage)													// Work through DCT stuff.
		select upper(mid(storage,i,1))
			case "D"
				if _DNDIsInArray(dnd.draggable,index) >= 0 then _DNDError("Duplicate object ID "+str(index))
				dnd.draggable.length = dnd.draggable.length + 1 				// Make space.
				dnd.draggable[dnd.draggable.length] = index 					// Store it.
				if index = DND_DISPLAY then _DNDError("Full display cannot be dragged")
			endcase
			case "C"
				if _DNDIsInArray(dnd.clickable,index) >= 0 then _DNDError("Duplicate object ID "+str(index))
				dnd.clickable.length = dnd.clickable.length + 1 				// Make space.
				dnd.clickable[dnd.clickable.length] = index 					// Store it.
			endcase
			case "T"
				if _DNDIsInArray(dnd.targettable,index) >= 0 then _DNDError("Duplicate object ID "+str(index))
				dnd.targettable.length = dnd.targettable.length + 1 			// Make space.
				dnd.targettable[dnd.targettable.length] = index 				// Store it.
			endcase
			case default
				_DNDError("Bad storage "+storage)
			endcase
		endselect
	next i
endfunction

//	Report DND Error
//	@param msg error message

function _DNDError(msg as string)
	if _DND_Error_Fired = 0 then Message("[DND Library] "+msg)
	_DND_Error_Fired = 1
endfunction

//	Get Object Depth
//	@param 	objectID 		DND Object ID
//	@return depth 

function _DNDGetObjectDepth(objectID as integer)
	retVal as integer
	if objectID = DND_DISPLAY														// whole display ?
		retVal = 10001																// the back is 10000, so display behind everything.
	elseif objectID < 0 															// -ve = text object
		retVal = GetTextDepth(-objectID)
	else
		retVal = GetSpriteDepth(objectID)											// +ve = sprite object
	endif
endfunction retVal

//	Object Collision check, is point x,y in the collision space of the given object.  On success
//	the offset parameter is set to the offset of the click point to the centre.
//	@param 	objectID 		DND Object ID
//	@param 	x 				X Position to test
//	@param 	y 				Y Position to test
//	@param 	offset 			Point structure to return offset from centre.
//	@return non zero if there is a collision.

function _DNDGetObjectCollision(objectID as integer,x as float,y as float,offset ref as DNDPoint)
	retVal as integer = 0
	if objectID = DND_DISPLAY 												// Do it for the whole display.
		retVal = (x >= 0 and y >= 0 and x <= GetVirtualWidth() and y <= GetVirtualHeight())
		offset.x = x-GetVirtualWidth()/2
		offset.y = y-GetVirtualHeight()/2
	elseif objectID < 0 														// Collision for text object
		xc as float:yc as float 
		xc = GetTextX(-objectID)+GetTextTotalWidth(-objectID)/2					// work out the centre
		yc = GetTextY(-objectID)+GetTextTotalHeight(-objectID)/2 
																				// Clicked within that text area ?
		retVal = abs(x-xc) < GetTextTotalWidth(-objectID)/2 and abs(y-yc) < GetTextTotalHeight(-objectID)/2
		offset.x = x-xc:offset.y = y-yc 										// Work out offset.
	else																		// Collision for sprite object
		retVal = GetSpriteHitTest(objectID,x,y)									// Check for collision.
		offset.x = x-(GetSpriteX(objectID)+GetSpriteWidth(objectID)/2)			// Calculate offset.			
		offset.y = y-(GetSpriteY(objectID)+GetSpriteHeight(objectID)/2)			
	endif
endfunction retVal
