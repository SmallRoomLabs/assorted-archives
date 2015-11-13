###############################################################################################################################################
###############################################################################################################################################
#
#		Project:	Arduboy Basic Compiler
#		File: 		term.py
#		Purpose:	Evaluate a term
#		Created:	1st August 2015
#		Modified:	1st August 2015
#
###############################################################################################################################################
###############################################################################################################################################

from utility import CodeLine

###############################################################################################################################################
#										This class is a worker function which compiles a term
###############################################################################################################################################

class TermEvaluator:
	def __init__(self,expressionEvaluator,identifierEvaluator):
		self.expressionEvaluator = expressionEvaluator
		self.identifierEvaluator = identifierEvaluator
	#
	#	Evaluate a single term. This can be : constant, quoted character, identifier (represents variable, constant, function)
	#	unary minus, 1 bit complement, byte read, word read (32 bit) or parenthesised expression.
	#
	def evaluate(self,codeLine):
		if codeLine.getConstant(False) is not None:										# handle a constant.
			const = codeLine.getConstant()												# extract constant.
			codeLine.compile([ const ])													# compile it.

		elif codeLine.getString(False) is not None:										# is it a string ?
			string = codeLine.getString()												# extract it.
			if string == "":															# This changes an empty string to zero.
				string = chr(0)
			if len(string) != 1:
				codeLine.error("Characters in numeric expressions must be length 0 or 1.")
			codeLine.compile([str(ord(string[0]))])										# compile the ASCII code.

		elif codeLine.getIdentifier(False) is not None:									# Found a constant, variable, or function call.
			ident = codeLine.getIdentifier()											# get the identifier
			info = self.identifierEvaluator.evaluate(ident)								# Find information about it.
			if info is None:															# Cannot identify the identifierself.
				codeLine.error("Cannot find term "+ident)
			if info["type"] == "Variable":												# Variable, read the value in.
				codeLine.compile([ str(info["address"]), "@"])
			if info["type"] == "Constant":												# Constant is just a constant.
				codeLine.compile([str (info["value"])])
			if info["type"] == "Function":												# Function , get a parameter list.
				self.getParameterList(codeLine)
				codeLine.compile([ "$$"+info["name"].upper()])							# then compile the call.

		else:																			# Must be ! ? - ~ or (
			operator = codeLine.getOperator()											# otherwise try to get an operator
			print(operator)
			if operator is not None:
				if operator == "-":														# -(term) unary negation.
					self.evaluate(codeLine)												# get the term.
					codeLine.compile(["NEG"])											# compile the NEGation.
				elif operator == "~":													# ~(term) bit inversion.
					self.evaluate(codeLine)												# get the term.
					codeLine.compile(["NOT"])											# compile the NEGation.
				elif operator == "?":													# ?(term) byte read
					self.evaluate(codeLine)												# get the term.
					codeLine.compile(["C@"])											# compile the byte read
				elif operator == "!":													# !(term) word read
					self.evaluate(codeLine)												# get the term.
					codeLine.compile(["@"])												# compile the byte read
				elif operator == "(":													# parenthesised expression.
					self.expressionEvaluator.evaluate(codeLine)							# evaluate the expression.
					nextToken = codeLine.getOperator()									# check it ends with a close bracket.
					if nextToken is None or nextToken != ")":
						self.error("Missing closing bracket on parenthesised expression")
				else:
					codeLine.error("Unknown term operator "+operator)					# not a known operator.
			else:
				codeLine.error("Unknown term")

	#
	#	Rip out a parameter list. This is stored on the stack with the first parameter lowest, finishing with the 
	#	number of parameters, so (66,77,12) would compile to 66 77 12 3 with the 3 being TOS.
	#
	def getParameterList(self,codeLine):
		if codeLine.getOperator() != "(":												# check it begins with an open bracket.			
			codeLine.error("Missing open parenthesis in function call")
		parameterCount = 0
		while codeLine.getOperator(False) != ")":										# Look for closing bracket.
			self.expressionEvaluator.evaluate(codeLine)									# get an expression which is a parameter
			parameterCount = parameterCount + 1											# bump parameter count
			if codeLine.getOperator(False) == ",":										# remove seperating comma.
				codeLine.getOperator()

		codeLine.getOperator()															# remove closing bracket.
		codeLine.compile([ str(parameterCount) ])										# push parameter count on stack.
		return [31]

###############################################################################################################################################
#														Class purely for testing
###############################################################################################################################################
			
class __DummyIdentifierObject:
	def evaluate(self,identifier):
		if identifier == "A":
			return { "type":"Variable", "address":42 }
		if identifier == "C":
			return { "type":"Constant", "value":69 }
		if identifier == "LEN":
			return { "type":"Function", "name":"len" }

###############################################################################################################################################
#
#		Date		Version			Notes
#		====		=======			=====
#		01-Aug-15	0.1				Initial version
#
###############################################################################################################################################

t = TermEvaluator(None,__DummyIdentifierObject())
t.expressionEvaluator = t

if __name__ == "__main__":
	examples = [ "4702+","a-","c+","len()","len(32)","len(a,c)",'"a"',"'P'","''","-A","~LEN(14)","?12","!-A","(~C)" ,"'fred'"]
	for c in examples:
		print("Input:",c)
		line = CodeLine("code.bas",1,c)
		print("Before:",line.sourceCode)
		t.evaluate(line)
		print("After:",line.sourceCode)
		print("Code:",line.objectCode)

