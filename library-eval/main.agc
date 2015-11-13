//
//
//							This is the demo program for the evaluation library
//
//
SetWindowTitle( "Evaluation Library Test" )
SetWindowSize( 1024, 768, 0 )
SetVirtualResolution( 1024, 768 )
#option_explicit

#include "library-eval.agc"													// Include the Evaluation Library

srcCode as String:resultCode as String

varSet1 as Eval_State														// This is 'state 1' - a virtual machine.
Eval_Write(varSet1,"A",22.7)												// Set A to 22.7
Eval_Write(varSet1,"B",4)													// Set B to 4

varSet2 as Eval_State														// This is 'state 2' - a virtual machine.
Eval_Write(varSet2,"A",1)													// Set A to 1
Eval_Write(varSet2,"B",2)													// Set B to 2

srcCode = "C = (A + 3) * B:D = C * 2 :C-1" 									// Do C = (A+3) * B then D = C * 2 and return C-1

compInfo1 as Eval_CompileInfo												// Compile it using this structure
resultCode = Eval_Compile(compInfo1,srcCode)								// Compile the code above.
																			// resultCode now contains the 'binary code'
																			// e.g. the rpn coded string
																			// it is also in compInfo1.rpnCode
							
rtn1 as float:rtn2 as float 						

rtn1 = Eval_Run(varSet1,resultCode)											// Now we run this script on each 'virtual machine'
rtn2 = Eval_Run(varSet2,compInfo1.rpnCode)									// Note resultCode == compInfo1.rpnCode
																			// Returned value is the top of the stack.
repeat
    print(rtn1)																// Result of running code on varset1
    print(rtn2)																// Same, varset 2
    print("Err:"+str(compInfo1.isError)+" "+compInfo1.errorDescription)		// Any error from the compiler
    print("Src:"+srcCode)													// Source code.
    print("Rpn:"+compInfo1.rpnCode)											// Print the 'object' rpn code

	print("")
	
	_Eval_Dump(varSet1)														// Dump VM states.
	_Eval_Dump(varSet2)
    Sync()
until GetRawKeyState(27) > 0

//
//	Eval_QCompile is pretty much the same except it compiles the string, and returns either the compiled code
//	or an error message prefixed with ?? if there was an error. 
//	So you could do 
//		
//		resultCode = Eval_QCompile(srcCode)										
//
//	Rather than explicitly declaring compInfo1 and using that. The error testing is different though.
//
