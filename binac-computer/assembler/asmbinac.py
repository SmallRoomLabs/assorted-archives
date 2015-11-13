# *******************************************************************************************************************************
# *******************************************************************************************************************************
#
#		Name:		asmbinac.py
#		Purpose:	BINAC Symbolic Assembler Program
#		Created:	17th October 2015
#		Author:		Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************************************************
# *******************************************************************************************************************************

import re,sys

# *******************************************************************************************************************************
#
#									  Exception specifically for assembler errors.
#
# *******************************************************************************************************************************

class AssemblerException(Exception):
	def __init__(self, message):
		self.message = message
	def __str__(self):
		return self.message

# *******************************************************************************************************************************
#
#													Memory Storage
#
# *******************************************************************************************************************************

class MemoryStore:
	def __init__(self,size = 512):
		self.size = size 																# size in double words
		self.memory = [ None ] * size 													# word memory allocated
		self.pointer = 0 																# write pointer
		self.highWord = True 															# write half 
	#
	#	Move the write pointer to a word boundary
	#
	def wordAlign(self):
		if not self.highWord:															# on second word
			self.write(025000)															# write SKIP
	#
	#	Write a whole word
	#
	def writeWord(self,word):															# write a whole word
		self.wordAlign() 																# must be on a whole word.
		self.memory[self.pointer] = word 												# save word
		print("        {0:04o}w {1:011o}".format(self.pointer,word))
		self.pointer = (self.pointer + 1) % self.size 									# bump pointer
	#
	#	Write a half word (e.g. single instruction)
	#
	def write(self,halfWord):															# write a half word
		print("        {0:04o}{1} {2:05o}".format(self.pointer,"h" if self.highWord else "l",halfWord))
		if self.highWord:																# upper half
			self.memory[self.pointer] = halfWord << 15 
			self.highWord = False
		else:																			# lower half
			self.memory[self.pointer] = self.memory[self.pointer] | halfWord
			self.pointer = (self.pointer + 1) % self.size 								# bump pointer
			self.highWord = True
	#
	#	Set the assembler pointer
	#
	def setPointer(self,pos):
		self.pointer = pos
		self.highWord = True
	#
	#	Get the assembler pointer, aligning it on a word.
	#
	def getAlignedPointer(self):														# get pointer for label, must be word aligned
		self.wordAlign()																# word align it
		return self.pointer 														

# *******************************************************************************************************************************
#
#												BINAC Assembler
#
# *******************************************************************************************************************************

class BinacAssembler:
	def __init__(self,addressSpace = 512):
		self.store = MemoryStore(addressSpace) 											# where the code goes
		self.memSize = addressSpace														# amount of memory
		self.memMask = addressSpace - 1													# mask for address values.
		self.dictionary = {} 															# known identifiers
		self.orders = {} 																# known orders
																						# list of orders
		orderList = """														
			stop/stop/1,f/addl/2,d/div/3,c/stc/4,a/add/5,
			m/mul/10,k/talc/11,l/tal/12,h/sta/13,t/jmc/14,s/sub/15,
			u/jmp/20,+/ashl/22,-/ashr/23,bp/brk/24,skip/nop/25
		""".lower().replace(" ","").replace("\t","").replace("\n","").split(",")
		for order in orderList:															# process them.
			o = order.split("/")
			self.orders[o[0]] = int(o[2],8)												# old style
			self.orders[o[1]] = int(o[2],8)												# new style
	#
	#	Assign an identifier a value ; if it exists, check it is unchanged
	#
	def assign(self,identifier,value):													# set dictionary value 
		identifier = identifier.lower()
		if identifier in self.dictionary:												# check not redefined differently
			if self.dictionary[identifier] != value:
				raise AssemblerException("Identifier "+identifier+" redefined.")
		self.dictionary[identifier] = value												# store value
	#
	#	Start a new pass
	#
	def startPass(self,passNumber):														# start a new pass
		self.asmPass = passNumber
		self.store.setPointer(0)
	#
	#	Assemble a single line of text.
	#
	def assembleLine(self,line):
		line = line if line.find("\\") < 0 else line[:line.find("\\")]					# remove comments
		line = line.rstrip().replace("\t"," ")											# remove trailing spaces, tabs to space
		line = line.lower()																# all lower case
		if line == "":																	# blank line.
			return			

		m = re.match("^([a-z][a-z0-9]+)\s*(.*)$",line)									# do we have a label here ?
		if m is not None:
			if m.group(2)[:1] == "=" or m.group(2)[:3] == "equ":						# is it an equate ?
				expr = m.group(2)[1:] if m.group(2)[0] == '=' else m.group(2)[3:]		# get expression after =/equ
				self.assign(m.group(1),self.evaluate(expr.strip(),True))				# evaluate expression.
				line = "" 																# nothing else
			else:																		# just a label in code.
				self.assign(m.group(1),self.store.getAlignedPointer())
				line = m.group(2).strip() 												# rest of line.

		line = line.strip() 															# remove surrounding spaces
		if line == "":																	# nothing left to do.
			return																		# label only or equate

		if line[:3] == "org":															# Origin ?
			self.store.setPointer(self.evaluate(line[3:],True) & self.memMask)
			return

		if line[:3] == "bss":															# Allocate storage
			n = self.evaluate(line[3:],True)											# how much
			if n >= 64:																	# do not allow > 64
				raise AssemblerException("BSS must be < 64 words")
			for i in range(0,n):														# write 'n' zeros
				self.store.writeWord(0)
			return

		m = re.match("^([a-z0-9\+\-]*)\s*(.*)",line)									# split into possible order and expr.
		if m.group(1) != "" and m.group(1) in self.orders:								# is it a known instruction ?
			opcode = self.orders[m.group(1)] << 9 										# order, shifted to correct place.
			if m.group(2).strip() != "":												# operand provided
				opcode += (self.evaluate(m.group(2).strip(),self.asmPass == 2) & self.memMask)	# add it in.
			self.store.write(opcode)													# write to assembler store
		else:																			# data value ?
			self.store.writeWord(self.evaluate(line,self.asmPass == 2))					# write as word
	#
	#	Evaluate a word composed of multiple additives
	#
	def evaluate(self,expr,requiredDefined):
		expr = expr.strip().lower() 													# remove first/last spaces,l/case
		total = 0
		for part in expr.split(" "):													# look through the bits
			if part != "":																# if something present.
				sign = 1																# default sign
				if part[0] == "-" or part[0] == "+":									# leading + or -
					sign = 1 if part[0] == "+" else -1									# work out sign
					part = part[1:]														# remove it
					if part == "":														# error if only sign.
						raise AssemblerException("Expression Syntax Error")
				total = total + sign * self.term(part,requiredDefined)					# Evaluate term, add sign and add
		if total < 0:																	# is the result < 0
			total = (abs(total) ^ 0o17777777777)+1 										# convert to 2's complement
		return total & 017777777777														# make result 31 bit.
	#
	#	Evaluate one term - identifier or constant.
	#
	def term(self,term,requiredDefined):
		if re.match("^[0-7]+$",term):													# octal constant
			return int(term,8)
		if re.match("^\\#[0-9]+$",term):												# decimal constant
			return int(term[1:],10)
		if re.match("^\\$[0-9a-f]+$",term):												# hex constant
			return int(term[1:],16)

		if re.match("^0.[0-9]+$",term):													# floating point
			return int(float(term) * pow(2,30))

		m = re.match("^([0-9]+\\.?[0-9]*)*e\\-([0-9]+)$",term)							# look for float e-nn
		if m is not None:
			exp = pow(10.0,-int(m.group(2)))
			f  = float(m.group(1))*exp
			if f >= 1.0:
				raise AssemblerException("Illegal floating point constant "+term)
			return int(f * pow(2,30))

		if re.match("^[a-z][a-z0-9]*$",term):											# identifier
			if term not in self.dictionary:												# unknown at present ?
				if requiredDefined:														# error if values required
					raise AssemblerException("Identifier "+term+" unknown")				# (pass 2 + some others)
				return 0 																# default 
			else:																		# identifier known
				return self.dictionary[term]											# return its value.

		raise AssemblerException("Unknown term "+term)
	#
	#	Assemble a file or group of files
	#
	def assemble(self,sourceFiles):
		if type(sourceFiles) is str:													# make a list if isn't.
			sourceFiles = [ sourceFiles ]
		print("BINAC Assembler (c) PSR 2015 : assembling "+",".join(sourceFiles))		# be informative.
		for asmPass in range(1,3):														# multiple passes
			print("    Pass {0}".format(asmPass))
			self.startPass(asmPass)														# start the pass.
			for f in sourceFiles:														# work through one or many files.
				src = open(f).readlines()												# read them in
				for n in range(0,len(src)):												# work through each line
					try:																# try to assemble it.
						self.assembleLine(src[n])
					except AssemblerException, ae:										# catch and rethrow with line #
						print("Error:{0} ({1}:{2})".format(ae.message,f,n+1))			# print an error message
						sys.exit(0)														# exit the program.
		print("Completed.")

# *******************************************************************************************************************************
#
#														Main Program
#
# *******************************************************************************************************************************

a = BinacAssembler()																	# create an assembler worker
arguments = sys.argv[1:]																# get the arguments.
a.assemble([x for x in arguments if x[0] != '-'])										# assemble anything not beginning with -

# TODO: Output
