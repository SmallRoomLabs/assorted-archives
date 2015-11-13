###############################################################################################################################################
###############################################################################################################################################
#
#		Project:	Arduboy Basic Compiler
#		File: 		expression.py
#		Purpose:	Evaluate an Expression
#		Created:	1st August 2015
#		Modified:	1st August 2015
#
###############################################################################################################################################
###############################################################################################################################################

from utility import CodeLine
from term import TermEvaluator

###############################################################################################################################################
#													Classic Recursive Descent Compiler
###############################################################################################################################################

class ExpressionEvaluator:
	#
	#	Constructor makes a term evaluator and sets up the priority list
	#
	def __init__(self,identifierEvaluator):
		self.termEvaluator = TermEvaluator(self,identifierEvaluator)
		self.operatorLevels = [ [ "&", "|","^"],["<",">","==",">=","<=","<>"],["+","-"] ,["*","/","%"],["?","!"] ]
		self.code = { "?":"+ C@","!":"+ @", "*":"*", "/":"/","%":"%","+":"+","-":"-","<":"<",">":">","==":"==","<=":"> NOT",">=":"< NOT","<>":"== NOT","&":"BITAND","|":"BITOR","^":"BITXOR" }
	#
	#	Evaluate an expression"
	#
	def evaluate(self,codeLine):
		return self.evaluateAtLevel(codeLine,0)

	#
	#	Called recursively for each level.
	#
	def evaluateAtLevel(self,codeLine,level):
		if level == len(self.operatorLevels):											# out of levels, return a term.
			return self.termEvaluator.evaluate(codeLine)
		else:																			# otherwise
			self.evaluateAtLevel(codeLine,level+1)										# evaluate one expression at this level
			nextOperator = codeLine.getOperator(False)									# look at next operator
			while nextOperator is not None and nextOperator in self.operatorLevels[level]: # if operator at this level
				nextOperator = codeLine.getOperator()									# get it
				self.evaluateAtLevel(codeLine,level+1)									# get the RHS
				codeLine.compile(self.code[nextOperator].split(" "))					# get code from dictionary and apply it
				nextOperator = codeLine.getOperator(False)								# look at next operator

###############################################################################################################################################
#															Helper class for testing
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

t = ExpressionEvaluator(__DummyIdentifierObject())
t.expressionEvaluator = t

if __name__ == "__main__":
	examples = [ "4*3+a?1 < 17" ,"2 > 1","2 >= 1","2 == 1","2 <> 1","a ^ c,"]
	for c in examples:
		print("Input:",c)
		line = CodeLine("code.bas",1,c)
		print("Before:",line.sourceCode)
		t.evaluate(line)
		print("After:",line.sourceCode)
		print("Code:",line.objectCode)

