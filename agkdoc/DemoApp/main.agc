
SetWindowTitle( "DemoApp" )
SetWindowSize( 1024, 768, 0 )

SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
    
repeat
    Print( ScreenFPS() )
    Sync()
until GetRawKeyState(27)

/// This is a function which does something to someone somewhere. Probably
/// of no real interest to anyone.
/// @param intp1 	an integer value but you've got to work it out
/// @param intp2    array which has some point I've forgotten.
/// @param someFloat# 	it's a float Jim, but not as we know it. I will go on and 
/// 					on and on and on and on and on and on and on and on and
/// 					on and on and on and on and on and on and on and on and
/// 					on and on and on and on and on and on and on and on and
// going to let it figure out defType
/// @return a float which will involve some work digging it out.

function MyMadeUpFunction(intp1,intp2 as integer[],someFloat#,defType ref as MyMadeUpType[])
	returnValue as float[12]
	// Code for my madeup function
endfunction returnValue

/// This type defines something or other somehow or other in some way or
/// other. This is <b>HTML</b> so this bolding should work.
/// But this should also be on the same line this is a <i>heading</i>

type MyMadeUpType
	int1									/// Int1 Member.
	int2 as integer							/// Int2 is an integer
	int3 as integer[12]						/// Int3 is an integer array
	float1# 
	float2 as float 						// a-ha - not a document comment.
	float3 as float[12]						/// Float3 is a float array
	string1$								/// This is a string on its own.
	string2 as string
	string3 as string[32]
	udtype1 as altType						// a user defined subtype.
	udtype2 as altType[24]
endtype

	
type altType
	n
endtype
