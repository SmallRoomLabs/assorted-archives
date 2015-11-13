///	This function adds one to the product of its parameters, and returns this value.
/// This is not the most exciting function I have ever seen.
/// @param num1 	the first parameter
/// @param num2 	the other parameter. You can extend this over multiple lines if you
///					want.
///	@return 		the product of the two parameters with one added.

function AddOneToMyProduct(num1 as integer,num2)
	result = num1 * num2 + 1
endfunction result

A typical type looks like this. Note the comments are after the members for types (again, this is not required)

///	This type represents a single point in 3D Space

type My3DType
	x#													/// the x coordinate
	y as float											/// the y coordinate
	z# as float											/// the z coordinate.
	someWork#											// this is picked up but not commented as it's a normal comment.
endtype
