###############################################################################################################################################
###############################################################################################################################################
#
#		Project:	Arduboy Basic Compiler
#		File: 		utility.py
#		Purpose:	Utility functions and classes for the Compiler
#		Created:	30th July 2015
#		Modified:	30th July 2015
#
###############################################################################################################################################
###############################################################################################################################################

import re 																				# We need regular expressions.

###############################################################################################################################################
#											This class encapsulates a line of code.
###############################################################################################################################################

class CodeLine:
	#
	#	Constructor saves the information, and preprocesses the line.
	#
	def __init__(self,sourceFile,lineNumber,sourceCode):
		self.sourceFile = sourceFile													# save constructor parameters
		self.lineNumber = lineNumber
		self.sourceCode = sourceCode
		self.objectCode = []															# no object code yet, haven't compiled anything.
		self.quoteDelimiter =chr(127)													# Quote delimited, replaces " and '.
		self.operatorList = [ "==","<>",">=","<=" ]
		self.operatorList.extend([ "+","-","*","/","%","&","|","^","(",")","!","?","=",">","<","~","," ])
		self.preProcess()																# preprocess the source code.
	#
	#	Pre process the source code. Remove comments, convert quotes to use delimiters, capitalise and remove multiple spaces from the 
	#	code bits.
	#
	def preProcess(self):
		code = [x.strip() for x in self.sourceCode.split(":") if x.strip() != ""] 		# split into instructions, remove empty ones.
		code = [x for x in code if not self.isCommentLine(x)]							# Remove REM or ' comments
		code = [self.processQuotes(x) for x in code]									# Process '' and "" pairs, convert to chr(128),chr(129)
		code = ":".join(code)															# Make the full string again.
		code = code.split(self.quoteDelimiter)											# Makes it alternate strings and code. Every odd one is a string.
		for i in range(0,len(code)):													# look through 
			if i % 2 == 0:																# if code found.
				code[i] = code[i].strip().upper().replace("\t"," ")						# make code upper case, replace tabs with spaces.
				while code[i].find("  ") >= 0:											# remove multiple spaces.
					code[i] = code[i].replace("  "," ")						
		self.sourceCode = self.quoteDelimiter.join(code)								# Put it back together.
	#
	#	Helper function that identifies comments.
	#
	def isCommentLine(self,line):
		if line != "":																	# Line is not empty ?
			if line[:3].upper() == "REM":												# begins with REM
				return True 															# then it is a comment
		return False
	#
	#	Helper function that looks for quoted strings and replaces the quotes by a control character. Additionally,
	#	this function checks the quotes balance correctly
	#
	def processQuotes(self,line):
		currentQuote = None 															# Not in quotes at the start
		for i in range(0,len(line)):													# Examine each character.
			if currentQuote is None and (line[i] == '"' or line[i] == "'"):				# opening a quote ?
				currentQuote = line[i]													# remember opening quote
				line = line[:i]+self.quoteDelimiter+line[i+1:] 							# replace it with a delimiter.
			elif currentQuote == line[i]:												# have we found the closing quote ?
				currentQuote = None 													# now out of quotes
				line = line[:i]+self.quoteDelimiter+line[i+1:] 							# replace it with a delimiter.

		if currentQuote is not None:													# At the end , have we balanced out ???
			self.error("Quotes do not balance or match")	
		return line
	#
	#	Error has occurred.
	#
	def error(self,message):
		raise Exception("{0} ({1}:{2})".format(message,self.sourceFile,self.lineNumber))
	#
	#	Extract an identifier from the text. Returns None if no match.
	#
	def getIdentifier(self,remove = True):
		return self.extractUsingRegEx("([A-Z_][A-Z0-9_]*)",remove)						# Match this Regex
	#
	#	Extract a numeric constant from the text, Returns None if no match.
	#
	def getConstant(self,remove = True):
		return self.extractUsingRegEx("(\-?[0-9]+)",remove)								# Match this Regex
	#
	#	Extract an operator from the text, Returns None if no match.
	#
	def getOperator(self,remove = True):	
		self.sourceCode = self.sourceCode.strip()										# Remove spaces
		for op in self.operatorList:													# Work through operators known
			if self.sourceCode[:len(op)] == op:											# If match found
				if remove:																# Remove if removing
					self.sourceCode = self.sourceCode[len(op):].strip()
				return op 																# Return the operator
		return None
	#
	#	Get a quoted string, returns None if none present
	#
	def getString(self,remove = True):
		self.sourceCode = self.sourceCode.strip()										# Remove spaces
		if self.sourceCode != "" and self.sourceCode[0] == self.quoteDelimiter:			# does it begin with quote delimiter ?
			pos = self.sourceCode[1:].find(self.quoteDelimiter)+1						# find the next one.
			string = self.sourceCode[1:pos]												# get the actual string
			if remove:																	# Remove if removing
				self.sourceCode = self.sourceCode[pos+1:]
			return string 																# and return it.
		return None
	#
	#	Helper function, return something that matches a regular expression.
	#
	def extractUsingRegEx(self,regEx,remove = True):
		self.sourceCode = self.sourceCode.strip()										# Remove space
		match = re.match(regEx,self.sourceCode)											# Run the Regex
		if match is not None:															# Match found ?
			if remove:																	# Remove it if required
				self.sourceCode = self.sourceCode[len(match.group(1)):].strip()
			return match.group(1)														# Return the matching part.
		else:
			return None
	#
	#	Compile code in.
	#
	def compile(self,code):
		self.objectCode.extend(code)


###############################################################################################################################################
#
#		Date		Version			Notes
#		====		=======			=====
#		30-Jul-15	0.1				Initial version
#
###############################################################################################################################################

if __name__ == "__main__":
	c = CodeLine("code.bas",1,'"\'" "  " "hi":rem comment: print  a      ,    "hello world":rem a comment:x = y + 2:: :print "goodbye"')
	print(c.sourceCode)
	ident = c.getString()
	while ident is not None:
		print("["+ident+"]")
		ident = c.getString()
	print(c.sourceCode)
	print(c.operatorList)