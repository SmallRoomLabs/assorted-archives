###################################################################################################################
#
#									   OSAP (One Sixty Assembler Program) in Python
#
#							Based on the CDC160 version by R. Hyer, September 1960 not in Python
#
#										  Written by Paul Robson September 2015
#
#	Changes:
#		(1) lower case assembly code can be input
#		(2) labels of more than six characters can be used. Theoretical problem with faulty code but worth benefit.
#
#	TODO:
#		If no op present, try it as data.
#		Pseudo op stuff
#		Output dumping.
###################################################################################################################

import re

#
#	Exception specifically for assembler
#
class AssemblerException(Exception):
	def __init__(self, message):
		self.message = message
	def __str__(self):
		return self.message
#
#	Assembler worker.
#
class Assembler:
	#
	#	Set up opcode/pseudo opcode tables, clear memory, create default symbols TEM0-7
	#
	def __init__(self):
		pseudoOpcodes = "ORG,CON,PRG,BLR,WAI,END,EQU,DEC,BCD,FLX,TTY,BCR,FLR,TTR"					# known pseudo ops and 64 opcodes
		opcodes = "ERR,SHA,LPN,LSN,LDN,LCN,ADN,SBN,LPD,LPI,LPF,LPB,LSD,LSI,LSF,LSB,LDD,LDI,LDF,LDB,LCD,LCI,LCF,LCB,ADD,ADI,ADF,ADB,SBD,SBI,SBF,SBB,STD,STI,STF,STB,SRD,SRI,SRF,SRB,RAD,RAI,RAF,RAB,AOD,AOI,AOF,AOB,ZJF,NZF,PJF,NJF,ZJB,NZB,PJB,NJB,JPI,JFI,INP,OUT,OTN,EXF,INA,HLT"

		opcodes = opcodes.split(",")
		self.opcodeHash = {}																		# hash of opcode -> function code
		for i in range(0,64):
			self.opcodeHash[opcodes[i]] = i;														# anything with a B also exists as R
			if opcodes[i][-1] == "B":																# which the assembler does F/B for us.
				self.opcodeHash[opcodes[i][:-1]+"R"] = i 											# so add that in too.

		for pop in pseudoOpcodes.split(","):														# add the pseudo-operations.
			self.opcodeHash[pop] = -1 																# they have a code -1.

		self.memory = [None] * 4096 																# 4k of unwritten memory.
		self.symbols = {} 																			# Symbol table.
		for i in range(0,8):																		# initialise TEM0 to TEM7
			self.symbols["TEM"+str(i)] = 0O070+i 													# to the values 0070-0077

	#
	#	Called when starting a pass. Initialise CON and PRG.
	#
	def startPass(self,passNumber):
		self.passNumber = passNumber 																# set pass number
		self.counters = { "PRG": 0O100, "CON": 0O020 };												# set prg and con counters.
		self.counterSel = "PRG"																		# start assembling to PRG.
		self.passHasEnded = False 																	# END can stop early.

	#
	#	Assemble a single line of code.
	#
	def assemble(self,line):
		if self.passHasEnded:																		# have we had an END if so stop.
			return
		line = line.upper().rstrip().replace("\t","|")												# capitalise, right strip, tab to bar
		line = "".join(line.split(" "))																# remove all spaces.

		self.label = ""																				# initially no label.
		if line[0] != "|":																			# is there a label, e.g. non empty first line.
			self.label = line.split("|")[0]															# extract the label.
			line = line[len(self.label):]															# remove the label

		while line != "" and line[0] == "|":														# remove leading bars.
			line = line[1:]

		if line == "" and self.label != "":															# label on its own
			self.setLabel(self.label,self.counters[self.counterSel])								# is allowed.

		if line[:3] != "REM" and line != "":														# we ignore all comments/labels onlyself.

			match = re.match("^([A-Z]+)(\|*)([A-Z0-9]*)(\|*)(\-?[A-Z0-9]*)",line)					# try and figure it out.
			if match is None:																		# couldn't, syntax error.
				raise AssemblerException("Syntax Error")

			self.opcode = match.group(1)															# look at the operation code

			if self.opcode not in self.opcodeHash:													# unknown operation code.
				# TODO: Try it as a constant ????
				raise AssemblerException("Unknown operation code "+self.opcode)

			self.functionCode = self.opcodeHash[self.opcode]										# look up the opcodes numeric value.

			self.operands = [ None, None ]															# we have no operands, yet.
			if match.group(2) != "" and len(match.group(2)) < 3:									# space/tab at most 2. 
				self.operands[0] = match.group(3)
				if match.group(4) != "" and len(match.group(4)) < 3 :								# repeat for the second value.
					self.operands[1] = match.group(5)

			if self.functionCode < 0:																# is it a pseudo operation ?
				self.pseudoOperation()
			else:
				self.location = self.allocateMemory(1) 												# allocate 1 word of code.
				if self.passNumber == 1:															# on pass 1
					if self.label != "":															# if there was a label
						self.setLabel(self.label,self.location)										# complete the assignment.
				else:																				# pass 2 assemble it.
					self.memory[self.location] = self.assembleInstruction()							# and write to memory.
	#
	#	Assemble the actual instruction.
	#
	def assembleInstruction(self):
		operand = self.evaluateOperand()															# evaluate operand
		print("Assemble",self.location,self.label,self.opcode,self.operands,operand)
		mode = self.opcode[-1]																		# this is the opcode : D I F B R N

		if self.opcode == "JFI":																	# JFI is forward not indirect :)
			mode = "F"
		if mode == "R":																				# is the mode R ?
			mode = "B" if operand < self.location else "F"											# convert mode to "B" or "F"

		if mode == "F":																				# forward
			operand = operand - self.location 														# convert to offset
		if mode == "B":																				# backward
			operand = self.location - operand 														# convert to offset

		if operand is None:																			# No operand at all
			if "HLT,INA,ERR".find(self.opcode) >= 0:												# HLT ERR INA do not require one
				operand = 0
			else:																					# Everything else does.
				raise AssemblerException("No operand provided")

		if operand < 0 or operand > 64:																# legal operand ?
			raise AssemblerException("Operand is out of range")

		return (self.functionCode << 6) | operand 													# return the resulting code.


	#
	#	Handle Pseudo-Operations
	#
	def pseudoOperation(self):
		if "BLR,DEC,BCD,FLX,TTY,BCR,FLR,TTR".find(self.opcode) >= 0 and label != "":				# data pseudo ops have normal alloc.
									self.setLabel(self.label,self.counters[self.counterSel])		# complete the assignment.

		print("Pseudo op",self.label,self.opcode,self.operands)
	#
	#	Give a label a value
	#
	def setLabel(self,label,value):
		if label in self.symbols:																	# if already in symbol table
			if self.symbols[label] != self.location:												# check it hasn't moved.
					raise AssemblerException("Redefined label "+label)	
		self.symbols[label] = self.location 														# put it in the symbol table.
	#
	#	Allocate an amount of memory in storage, checking if CON and PRG are legitimate afterwards.
	#
	def allocateMemory(self,amount):
		n = self.counters[self.counterSel]															# get the current counter
		self.counters[self.counterSel] += amount 													# bump the counter
		if self.counterSel == "PRG":																# check PRG
			if self.counters["PRG"] > 0O7776:
				raise AssemblerException("PRG Counter has acceeded 7776")
		if self.counterSel == "CON":																# check CON
			if self.counters["CON"] > 0O0077:
				raise AssemblerException("CON Counter has exceeded 0077")
		return n
	#
	#	Evaluate the operand.
	#
	def evaluateOperand(self):
		result = None																				# none provided
		for op in self.operands:
			if op is not None:																		# present ?
				result = result if result is not None else 0 										# make it 0 if was None.
				sign = 1																			# default to positive
				if op[0] == '-':																	# is it negative ?
					sign = -1																		# change sign
					op = op[1:]																		# remove -
				if op[0] >= '0' and op[0] <= '7':													# numeric octal constant
					result = result + sign * int(op,8)												# add in with sign
				else:
					if op not in self.symbols:														# unknown
						raise AssemblerException("Label "+op+" unknown")
					result = result + sign * self.symbols[op]										# add symbol value in with sign.
				result = result & 0O7777 															# force into range.
		return result

a = Assembler()
for apass in range(1,3):
	print("Pass ",apass)
	a.startPass(apass)
	a.assemble("		rem hello world !")
	a.assemble("		LDN		42		 			i like chips in gravy")
	a.assemble(" 		LSN		1 	2")
	a.assemble("		JFI		loop2")
	a.assemble(" 		LSN		3 	-2")
	a.assemble("loop	NZF 	loop2")
	a.assemble("loop2	LDI 	14")
	a.assemble("	hlt")

print("Dump:")
for i in range(0,4096):
	if a.memory[i] is not None:
		print("{0:04o} {1:04o}".format(i,a.memory[i]))
print()
print(a.symbols)
