# README #

This is the README for agkdoc.

### What is this repository for? ###

AGKDoc is a javadoc like program for AppGameKit2 source code (it may well work for AGK1 code but I haven't tried it)

### How do I get set up? ###

AGKDoc is a Python program, which requires Python installed. It was developed using Python 3, but should work under Python 2
as it uses no new features.

The only required part of the repository is the python script agkdoc.py, everything else is test or examples. For an example
of what input and output looks like, look at the documentation.html file, and the three .agc files in the DemoApp directory.

AGKDoc allows commenting of user defined functions and types, inline to the code. It is not a requirement, any uncommented 
functions will be ignored.

AGKDoc comments use three slashes /// so they will be viewed by the Compiler as any other comment.  If you have already commented
your code by putting // comments before it, you can convert it to AGKDoc just by adding another / to each, though this will not 
comment parameters, members or return values.

A module definition is done as follows. All the parameters are optional apart from @name (it must have a name) and the terminating
/// @module - this is not an inline documentation comment but a marker so that the program knows where the module documentation
ends.

Note that requires and provides are for my library building system, so you can use these for whatever you like.

/// This is a very simple module whose sole purpose in life is to count Objects 
/// and it doesn't even do that. However it does mean I can have some nice module
/// header code. All these below are optional (except @name).

/// @name 	DodgyModule
/// @author Paul Robson 
/// @email  paulscottrobson@gmail.com 
/// @license tokill
/// @prefix dgm_
/// @version 0.1 
/// @created 16-Feb-2014
/// @updated 22-Jul-2014
/// @requires somelib,someotherlib
/// @provides dodgy
/// @module

A typical function, note you can use inline HTML for effect if you wish. Parameters and Return inline comments are not  required - the table will be generated in the output HTML but it won't have any descriptions (this code is in anothertest.agc and its output is in documentation.html)

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

### Running it ###

To generate document for agc files, run python agkdoc.py in the directory where the files are. 

Parameters can be individual source files or directories. 

If it is an individual file (e.g. fred.agc) then it generates a file fred.html in the same directory for documentation.

If it is a directory (e.g. mysource) then it generates a file documentation.html in that directory which covers all .agc 
files in that directory and all its subdirectories.

Documentation files are linked to an agkdoc.css file - this is created by the program if it does not already exist. If an agkdoc.css 
file is already present it is not overwritten (in case you have modified the CSS)

### Who do I talk to? ###

All bugs, suggestions should go to paulscottrobson@gmail.com
