/// This module provides evaluation functionality to AGK2, the ability to evaluate an expression complete with 
/// variables etc. Unfortunately it is not possible to directly access the variables AGK2 uses, so each 
/// "state" has its own set, which can be reused when necessary<br>
/// The concept is fairly straightforward. This module contains a compiler which converts algebraic notation 
/// (a + b ) * 7 into an internal RPN code (actually AGK2 does this too !) and an interpreter which interprets
/// that code. Both the source and the 'object' rpn code are strings (the RPN code looks like a series of 
/// characters seperated by commas).<br>
/// The reason for the 'split' is the run time interpreter is much quicker than the compiler part.<br>
/// When running a script you provided it with a "state" - this is basically the variables that the script works on.
/// So a script could look like S = SIN(A):C = COS(A):N = S*S + C*C:SQRT(A) - this is three assignment statements,
/// and an expression. The Evaluator can be used to do either simple arithmetic or operate on sets of variables.

/// @name 	library-eval
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix Eval_
/// @provides evaluation 
/// @requires 
/// @version 0.1 
/// @created 05-Feb-15
/// @updated 06-Feb-15
/// @module

#constant Eval_Log 	Print 													// Command used to log for dumping

/// This type is a set of variables to be used in any evaluation, and also contains
/// the evaluation stack. Do not access the member variables directly as the implementation
/// may change in the future.

type Eval_State
	stackPointer as integer 												/// Stack pointer, points to current TOS value.
	stack as float[8]														/// Stack itself. All values are floats.
	variables as float[26]													/// 26 variables, A-Z (variables[1] == A)
endtype

/// This type contains the compiling information. You can compile using one of these, or there is a simpler 
///	shortcut.

type Eval_CompileInfo
	isError as integer 														/// has an error occurred ?
	errorDescription as string 												/// text description of error
	srcCode as string 														/// input infix code.
	rpnCode as string 														/// resulting RPN code.
endtype

global _Eval_WorkCompileInfo as Eval_CompileInfo							// Temporary compiler store.

global _Eval_OperatorLevels as String[3] = [ "", "><=#","+-","*/" ]			// Infix operator levels.
 
///	Run a RPN script on a new evaluation state. This and the function Eval_Continue which is the same save
/// it does not reset the stack at the start, can be used by developers if they wish to write in RPN directly.
/// However there is less error checking on this run time interpreter.

///	@param state the evaluation machine state
/// @return the top of stack value or 0 if there is no TOS value.

function Eval_Run(state ref as Eval_State,script as String)
	state.stackPointer = -1 												// There is no current TOS value as the stack is empty.
	retVal as float
	retVal = Eval_Continue(state,script)									// run it with the continuation function.
endfunction retVal

///	Run a RPN script on a new evaluation state, without resetting the stack. A script is a bit Forth-like
/// commands seperated by commas. Operators are +,-,*,/,%(modulus), <, = , >, # (not equal). Variable access
/// commands are !name and @name for write and read respectively. A floating point number or an integer
/// (must begin with a digit) can be pushed on the stack. &x is a single parameter function on TOS, e.g.
/// &sin,&cos,&tan,&sqrt.
/// Note that this does virtually no checking at all.

///	@param state the evaluation machine state
/// @param script an RPN script as a string.
/// @return the top of stack value or 0 if there is no TOS value.

function Eval_Continue(state ref as Eval_State,script as String)
	tokenCount as integer:i as integer:cmd as string:code as integer
	sp as integer 															// stack pointer short cut.
	script = Upper(script)													// capitalise script
	sp = state.stackPointer 												// get the stack pointer to a local (speed thing)
	tokenCount = CountStringTokens(script,",")								// Get the number of tokens
	for i = 1 to tokenCount													// For each token
		cmd = GetStringToken(script,",",i)									// Get it.
		code = asc(cmd)
		if code >= 48 and code < 58											// is it a number ?
			sp = sp + 1														// push it on the stack.
			state.stack[sp] = ValFloat(cmd)									// as a float.
		else 
			select code 													// Commands (int comp quicker than string comp)
				case 43														// + (add)
					state.stack[sp-1] = state.stack[sp-1] + state.stack[sp]
					sp = sp - 1
				endcase
				case 45														// - (subtract)
					state.stack[sp-1] = state.stack[sp-1] - state.stack[sp]
					sp = sp - 1
				endcase
				case 42														// * (multiply)
					state.stack[sp-1] = state.stack[sp-1] * state.stack[sp]
					sp = sp - 1
				endcase
				case 47														// / (divide)
					state.stack[sp-1] = state.stack[sp-1] / state.stack[sp]
					sp = sp - 1
				endcase
				case 37														// % (modulus)
					state.stack[sp-1] = mod(state.stack[sp-1],state.stack[sp])
					sp = sp - 1
				endcase
				case 60														// < (less than)
					state.stack[sp-1] = state.stack[sp-1] < state.stack[sp]
					sp = sp - 1
				endcase
				case 61														// = (equal)
					state.stack[sp-1] = state.stack[sp-1] = state.stack[sp]
					sp = sp - 1
				endcase
				case 62														// > (greater than)
					state.stack[sp-1] = state.stack[sp-1] > state.stack[sp]
					sp = sp - 1
				endcase
				case 35														// # (not equal)
					state.stack[sp-1] = state.stack[sp-1] <> state.stack[sp]
					sp = sp - 1
				endcase
				case 64														// @var (read variable)
					sp = sp+1
					state.stack[sp] = Eval_Read(state,mid(cmd,2,-1))
				endcase
				case 33														// !var (write variable)
					Eval_Write(state,mid(cmd,2,-1),state.stack[sp])
					sp = sp-1
				endcase
				case 38 													// &fn (function)
					state.stack[sp] = _Eval_Function(mid(cmd,2,-1),state.stack[sp])
				endcase
			endselect
		endif
	next i
	retVal as float = 0 													// The default return value.
	if sp >= 0 then retVal = state.stack[sp]								// If there is a value on the stack, return TOS.
	state.stackPointer = sp 												// Save SP back.
endfunction retVal

//	Evaluate a single argument function (currently SQRT,SIN,COS and TAN)
//	@param cmd 	name of function, upper case
//	@param n	parameter for function
// 	@return 	result of function 

function _Eval_Function(cmd as string,n as float)
	select Upper(cmd) 
		case "SQRT"
			n = sqrt(n)
		endcase
		case "SIN"
			n = Sin(n)
		endcase
		case "COS"
			n = Cos(n)
		endcase
		case "TAN"
			n = Tan(n)
		endcase
	endselect
endfunction n

/// Read a variable from an evaluation state.
/// @param name 	name of variable
/// @return value of variable.

function Eval_Read(state ref as Eval_State,name as String)
	retVal as float = 0
	retVal = state.variables[_Eval_IdentifierToIndex(name)]					// Read the variable required.
endfunction retVal

/// Write a variable to an evaluation state.
/// @param name 	name of variable
/// @param value 	value to write.

function Eval_Write(state ref as Eval_State,name as String,value as Float)
	state.variables[_Eval_IdentifierToIndex(name)] = value 					// Copy value
endfunction

//	Convert an identifier to an index in state.variables
//	@param name 	name of variable
//	@return number of variable in array.

function _Eval_IdentifierToIndex(name as String)
	index as integer 
	name = upper(name) 														// Capitalise identifier
	if len(name) <> 1 or name < "A" or name > "Z" 							// Throw error message if name is not A..Z
		Message("EVAL: Bad variable name "+name)
	endif
	index = asc(left(name,1)) - asc("A") + 1 								// convert it so A..Z is 1-26
endfunction index

//	Debugging Dump routine, uses Eval_Log constant so can dump to log file if reqd.
//	@param state state to dump

function _Eval_Dump(state ref as Eval_State)
	stack as String = "Stack : "											// dump the stack
	i as integer
	if state.stackPointer >= 0 												// stackPointer points to TOS so -1 is empty.
		for i = 0 to state.stackPointer
			stack = stack + " "+str(state.stack[i],1)
		next i
	endif
	Eval_Log(stack+" [top]")												// By changing this you can o/p to log 
	for i = 1 to 26															// For brevity only dump non-zero variables.
		if state.variables[i] <> 0 
			Eval_Log(chr(i+64)+"="+str(state.variables[i],1))
		endif
	next i
endfunction 

///	Compile RPN code. This is a sequence of expressions or assignments, seperated by a colon.
/// @param ec 	Compilation Information structure, initialised by this method.
/// @srcCode Source code to compile.
/// @return RPN object code, also stored in the compiler info structure.

function Eval_Compile(ec ref as Eval_CompileInfo,srcCode as String)
	ec.isError = 0															// Clear error
	ec.errorDescription = ""												// Clear error description.
	ec.srcCode = Upper(srcCode)												// Save source code
	ec.rpnCode = "" 														// No object code
	_Eval_PreProcess(ec)													// Remove leading spaces.
	
	while ec.srcCode <> "" and ec.isError = 0 								// While more to compile and no errors.
		srcOld as String:identifier as String
		srcOld = ec.srcCode 												// Save old source code while checking for ident = 
		identifier = _Eval_ExtractNI(ec,"I")								// Try to extract an identifier.
		_Eval_PreProcess(ec)												// Remove more spaces.
		if asc(identifier) >= asc("A") and asc(ec.srcCode) = asc("=")		// found <identifier> = [expression] 
			ec.srcCode = mid(ec.srcCode,2,-1)								// Remove the assignment equals.
			_Eval_Expression(ec)											// Evaluate the expression.
			ec.rpnCode = ec.rpnCode + "!" + identifier + ","				// add code to store to identifier.
		else 
			ec.srcCode = srcOld 											// Restore, as it wasn't an assignment.
			_Eval_Expression(ec)											// So it must be an expression.
		endif 
		while asc(ec.srcCode) = asc(":") or asc(ec.srcCode) = asc(" ") 		// Skip colons and spaces.
			ec.srcCode = mid(ec.srcCode,2,-1)
		endwhile
	endwhile 
												
endfunction ec.rpnCode

///	This is a shortcut version of Eval_Compile - it returns either the compiled code as a string, or 
///	?? followed by the error message, and uses an internal workspace.
/// @param srcCode 	Compiler source code
/// @return rpnCode or ?? followed by an error message if appropriate.

function Eval_QCompile(srcCode as String)
	retVal as String 
	EVAL_Compile(_Eval_WorkCompileInfo,srcCode)								// Compile it using module workspace.
	if _Eval_WorkCompileInfo.isError = 0 									// if okay
		retVal = _Eval_WorkCompileInfo.rpnCode 								// Return code 
	else 																	// if not
		retVal = "?? "+_Eval_WorkCompileInfo.errorDescription 				// Return ?? error
	endif 
endfunction retVal 

//	Compile code for an expression, supports + - * / %  ( ) < = > # and unary minus.
//	@param ec Current compilation structure which is modified appropriately.

function _Eval_Expression(ec ref as Eval_CompileInfo)
	_Eval_ExpressionAtLevel(ec,1)
endfunction

//	Compile code for an expression at the given level. Basic RDC
//	@param ec Current compilation
//	@param level operator level to work at.

function _Eval_ExpressionAtLevel(ec ref as Eval_CompileInfo,level as integer)
	_Eval_PreProcess(ec)													// Remove space, capitalise.
	if level > _Eval_OperatorLevels.length 									// Reached the term level.
		_Eval_Term(ec)														// So compile a term
	else 
		_Eval_ExpressionAtLevel(ec,level+1)									// Compile subexpression at next level.
		_Eval_PreProcess(ec)												// Remove space, capitalise.
		while ec.srcCode<>"" and _Eval_OpAtLevel(left(ec.srcCode,1),level)	// while there is an operator at this level
			operator as string
			operator = left(ec.srcCode,1)									// save the operation
			ec.srcCode = mid(ec.srcCode,2,-1)								// remove it from the source
			_Eval_ExpressionAtLevel(ec,level+1)								// do the RHS of this operation
			ec.rpnCode = ec.rpnCode + operator + ","						// add to the compiled code
		endwhile
	endif
endfunction

// 	Is the given operator an operator at the given descent level.
// 	@param operator 	operator to check.
//	@param level 		level to check it at
//  @return non zero if it is.

function _Eval_OpAtLevel(operator as string,level as integer)
	retVal as integer = 0:i as integer
	opList as string: opList = _Eval_OperatorLevels[level]					// get list of allowable.
	for i = 1 to len(opList)												// see if the operator is in it.
		if mid(opList,i,1) = operator then retVal = 1
	next i
endfunction retVal

//	Compile a term. Currently support ; decimal constants, unary minus, variable access, function calls
//	@param ec Current compilation

function _Eval_Term(ec ref as Eval_CompileInfo)
	first as integer
	_Eval_PreProcess(ec)													// Remove space, capitalise.
	first = asc(ec.srcCode)													// Ascii code for first item.
	if first >= asc("0") and first <= asc("9")								// found a number.
		number as string 													// convert it into a number.
		number = _Eval_ExtractNI(ec,"N")									// extract number/identifier.
		ec.rpnCode = ec.rpnCode+number+","									// add number to code string.
	elseif first = asc("-") 												// unary minus
		ec.rpnCode = ec.rpnCode+"0,"										// doing 0 (term) -
		ec.srcCode = mid(ec.srcCode,2,-1)
		_Eval_Term(ec)														// do the term
		ec.rpnCode = ec.rpnCode+"-,"										// and subtract from zero, e.g. -ve
	elseif first >= asc("A") and first <= asc("Z")							// variable name and function call begins with A-Z
		ident as String:ident = _Eval_ExtractNI(ec,"I")						// get an identifier / function name.
		if asc(ec.srcCode) = asc("(")										// function call ?
			_Eval_Expression(ec)											// Evaluate parameter to function.
			ec.rpnCode = ec.rpnCode + "&"+ident+","							// call the unary function
		else 																// otherwise a variable access
			ec.rpnCode = ec.rpnCode+"@"+ident+","							// add code to read variable
		endif
	elseif first = asc("(")													// is it parentheses ?
		ec.srcCode = mid(ec.srcCode,2,-1) 									// skip over open bracket
		_Eval_Expression(ec)												// evaluate expression in brackets.
		if asc(ec.srcCode) <> asc(")")										// closing bracket 
			_Eval_Error(ec,"Missing closing parenthesis")
		endif
		ec.srcCode = mid(ec.srcCode,2,-1) 									// skip over close bracket
	else
		_Eval_Error(ec,"Syntax Error "+ec.srcCode)							// else no idea.
	endif
endfunction

//	Extract a number or identifier out of the source, very simple and lazy but works well enough.
//	@param ec 	Compiler state
//	@param allow allow numbers or identifiers. (N or I)
//	@return 	Extracted identifier

function _Eval_ExtractNI(ec ref as Eval_CompileInfo,param as String)
	ni as string = ""														// number/identifier goes here.
	repeat
		isNi as integer = 0 												// This is set if it is 0-9,A-Z or .
		if asc(ec.srcCode) >= asc("0") and asc(ec.srcCode) <= asc("9") then isNi = 1
		if asc(ec.srcCode) >= asc("A") and asc(ec.srcCode) <= asc("Z") then isNi = (param = "I")
		if asc(ec.srcCode) = asc(".") then isNi = (param = "N")
		if isNi <> 0														// if one of these, add it.
			ni = ni + left(ec.srcCode,1)
			ec.srcCode = mid(ec.srcCode,2,-1)
		endif
	until isNi = 0															// until none found
endfunction ni

//	Handle an error by setting error flag and message
//	@param ec Current compilation
//	@param msg Message describing error.

function _Eval_Error(ec ref as Eval_CompileInfo,msg as string)
	if ec.isError = 0 														// if no error so far
		ec.isError = 1														// mark error
		ec.errorDescription = msg 											// save the mesage
	endif
endfunction

// Preprocess compilation - capitalises and removes leading spaces.
//	@param ec Current compilation

function _Eval_PreProcess(ec ref as Eval_CompileInfo)
	ec.srcCode = Upper(ec.srcCode)											// Capitalise
	while asc(ec.srcCode) = asc(" ")										// Remove spaces
		ec.srcCode = mid(ec.srcCode,2,-1)
	endwhile
endfunction
