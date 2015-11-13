########################################################################################################
#
#												GOLUX Assembler
#												===============
#
#	Should be code compatible with GOLUX as described in the memo, except for:
#
#	(1) TAB does not function as RETURN, it is a space, to allow for use with editors that tab.
#	(2) It is less fussy ; equates can be anywhere, definitions can be as long as you like and so on.
#	(3) It takes a standard ASCII text file as input, not paper tape (paper tape output though ....)
#
########################################################################################################

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
#	Golux Assembler Worker
#
class AssemblerWorker:
	def __init__(self):
		self.words = {} 														# clear word list
		self.standardVocabulary()												# load standard word list
																				# TX-0 Opcodes.
		self.opcodes = { "sto":0o000000, "add":0o200000, "trn": 0o400000, "opr":0o600000}
		self.tapeOut = open("golux.tape","w")									# Golux paper tape output.
	#
	#	Load in standard vocabulary words
	#
	def standardVocabulary(self):
																				# from microcode ... in text file.
		wordList = """															
		r1r=254848;p7a=208401;r3c=255488;lr0=196736;r1c=254464;tac=245764;r1l=254489;alr=196737;p7h=208768;cll=229376;cla=245760;clc=245792;add=65536;clr=212992;cyr=196992;p6s=257024;opr=196608;lad=196634;tbr=245779;ios=253952;pnt=207232;lac=245778;p6a=207889;pna=206865;p6h=208256;pnc=206897;cyl=196633;prt=206848;shr=196864;trn=131072;sto=0;cal=245888;lpd=196626;com=196640;dis=205824
		"""
		for w in wordList.strip().split(";"):									# split up golux word list
			wp = w.split("=")													# split a=b into two bits
			self.define(wp[0],int(wp[1]))										# define it
	#
	#	Define a new vocabulary word
	#
	def define(self,name,value):
		name = name.lower()														# case independent
		if name in self.words:													# if exists
			if self.words[name] != value:										# check unchanged
				raise AssemblerException(name+" redefined.")
		self.words[name] = value												# assign value
	#
	#	Set the current pass
	#
	def setPass(self,passNumber):
		self.currentPass = passNumber 											# set the current pass
		self.pointer = -1 														# pointer has not been specified.
		self.titleSkipped = False 												# have we skipped the title.
		self.lineNumber = 0 													# clear line number
	#
	#	Assemble a single line.
	#
	def assembleLine(self,line):
		self.lineNumber += 1 													# bump line number
		if not self.titleSkipped:												# skip first line as it is always title
			self.titleSkipped = True
			return
		line = line.strip().lower().replace("\t"," ")							# remove stuff, make L/C

		m = re.match("^([0-7]+)\|\s*(.*)$",line)								# is it setting origin nnnn| ?
		if m is not None:
			self.pointer = int(m.group(1),8)									# set pointer from that.
			if self.pointer > 0xFFFF:
				raise AssemblerException("Illegal assembly address")			# must be in 64k range
			line = m.group(2).strip()

		m = re.match("^([a-z][a-z0-9]*)\s*\=\s*(.*)$",line)						# label = something ?
		if m is not None:
			if m.group(2).strip() == "":										# check something exists.
				raise AssemblerException("Equate has no right hand side")
			n = self.assembleInstruction(m.group(2),True)						# assemble the equate bit, must be defined
			self.define(m.group(1),n)											# do the actual assignment.
			return

		if line.find("|") >= 0:													# remove slash comments
			line = line[:line.find("|")].strip()

		if line == "" or line == ".":											# if empty or . then return
			return
		if self.pointer < 0:													# if we get this far we must have
			raise AssemblerException("No assembler address specified")			# specified an address.

		m = re.match("^([a-z][a-z0-9]*)\,\s*(.*)$",line)						# label definition.
		if m is not None:
			self.define(m.group(1),self.pointer) 								# define the label
			line = m.group(2).strip() 											# remove it from the line.

		if line[:5] == "start":													# is it start ?
			if self.currentPass == 2:											# ignore on pass # 1
				instr = 0o200000 												# instr would be add
				line = line[5:].strip() 										# remove start
				if line[:3] == "trn":											# start trn ?
					line = line[3:].strip()										# remove trn
					instr = 0o400000 											# instr is now trn
				n = self.assembleInstruction(line,True)							# find out where to start
				self.writeWord(instr | (n & 0177777))							# write add x or trn x 
				return

		if line != "" and line != ".":											# if what remains is not . or empty
			n = self.assembleInstruction(line,self.currentPass == 2)			# convert to a value.
			if self.currentPass == 2:											# show assembled code.
				print("{0:06o}:{1:06o} {2}".format(self.pointer,n,line))
			self.write(n)														# write it out as code (pass 2 only)
	#
	#	Write out an instruction to the current pointer which is advanced
	#
	def write(self,word):
		if self.currentPass == 2:												# output code on pass 2
			#print("---> {0:06o} = {1:06o})".format(self.pointer,word))
			self.writeWord(self.pointer)										# write the address (sto address)
			self.writeWord(word)												# write the word (word behind)
																				# *see page 6/7 on 6M-4789-1
		self.pointer += 1
	#
	#	Write out a word in R3C format to the paper tape.
	#
	def writeWord(self,word):
		for tapeWord in range(0,3):												# 3 consecutive words
			tapeOut = ""
			for tapeBit in range(1,7):											# bits 1-6 checked.
				wBit = (2 - tapeWord)  											# first bit C,B,A
				wBit += (tapeBit - 1) * 3										# adjust for C,F,I,L,O,R etc.
				bit = word & (0o400000 >> wBit)									# extract bit from word.
				tapeOut = tapeOut+("O" if bit != 0 else ".")
			tapeOut = tapeOut + "O"												# final hole.
			self.tapeOut.write(tapeOut+"\n")									# output paper tape.
	#
	#	Assemble an instruction in the format [<opcode>] <term>|<+->|<term> .... Optionally does not
	#	require all identifiers to be defined for forward referencing.
	#
	def assembleInstruction(self,instr,mustEvaluate):
		orgInstr = instr
		opcode = 0
		if len(instr) > 3 and instr[3] == ' ':									# is it [opcode] something
			if instr[:3] not in self.opcodes:									# known opcode ?
				raise AssemblerException("Opcode "+instr[:3]+" unknown")
			opcode = self.opcodes[instr[:3]]									# reset base opcode
			instr = instr[3:].strip() 											# remove base opcode
			if instr == "":														# check there is an operand.
				raise AssemblerException("Missing operand in instruction")

		instr = instr.replace("-","+-")											# make all - +- so + is seperator.
		for subx in instr.split("+"):											# divide into bitsself.
			if subx != "":														# if a part is there.
				if re.match("^(\-?[0-7]+)$",subx) is not None:						# is it a number
					opcode = opcode + int(subx,8)								# add that
				else:
					if subx in self.words:										# known word
						opcode = opcode + self.words[subx]						# add to running total
					else:
						if mustEvaluate:										# if must evaluate
							raise AssemblerException("Unknown identifier "+subx)
		opcode = opcode & 0777777 												# make it 18 bit.
		#print"{1:06o} : {0}".format(orgInstr,opcode)
		return opcode
	#
	#	Assemble files.
	#
	def assemble(self,fileList):
		if type(fileList) == type(""):											# convert single file to list.
			fileList = [ fileList ]
		for p in range(1,3):													# 2 passes.
			print("Pass "+str(p))
			self.setPass(p)														# set pass
			for f in fileList:													# for each file
				print("    Assembling ..."+f)
				src = open(f).readlines()										# read it in.
				for l in src:													# for each line
					self.assembleLine(l)										# assemble it.

w = AssemblerWorker()
w.assemble("test.glx")

# TODO: Wrapper for AssemblerWorker (complete CLI)
# TODO: Assembler error trapping.
