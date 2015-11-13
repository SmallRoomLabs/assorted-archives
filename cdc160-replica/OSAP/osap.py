###################################################################################################################
###################################################################################################################
#
#									   OSAP (One Sixty Assembler Program) in Python
#
#							Based on the CDC160 version by R. Hyer, September 1960 not in Python
#
#										  Written by Paul Robson September 2015
#
#	
#	Notes:
#
#		1) 	Lower case is acceptable
#		2) 	We use spaces and tabs as seperators.
#		3) 	Double slash comments are supported. Commenting by position is not supported (rem still is).  
#		4) 	Any number of operand sums on a line is permitted.
#		5) 	Assembles "new style" more readable mnemonics which use @ (indirect) # (immediate) and auto
#			deduct D,F or B.
#		6) 	Can change default number base.
#		
###################################################################################################################
###################################################################################################################

import re,sys

###################################################################################################################
#											UTILITY FUNCTIONS/CLASSES
###################################################################################################################

#
#	Exception specifically for assembler
#
class AssemblerException(Exception):
	def __init__(self, message):
		self.message = message
	def __str__(self):
		return self.message

###################################################################################################################
#											     CHARSET MAPPERS
###################################################################################################################

class Mapper:
	def __init__(self,mapperDescr):
		mapperDescr = re.sub("\\s+","",mapperDescr)													# Remove any spaces	
		mappings = mapperDescr.split("|")															# Split around the seperator
		assert len(mappings) % 2 == 0																# check it is even.
		self.map = {}
		for i in range(0,len(mappings),2):															# do alternate charactes
			ch = mappings[i].strip()																# get character
			ch = " " if ch == "_" else ch 															# convert _ to space
			self.map[ch] = int(mappings[i+1],8)														# store octal equivalent.													

	def convert(self,istr):
		istr = istr.upper()			 																# we are using upper case
		shifted = False
		codes = []																					# result.
		for c in istr:																				# work through string
			code = self.map[c]
			if (code > 0o77) != shifted:															# has shift state changed.
				shifted = (code > 0o77)																# change the state
				codes.append(self.getShift() if shifted else self.getUnshift())						# add code to change state
			codes.append(code & 0o77)																# add code
		if shifted:																					# unshift TTY if required.
			codes.append(self.getUnshift())
		return codes

class FlexowriterMapper(Mapper):																	# Flexowriter
	def __init__(self):
		mapper = """
		A|30|B|23|C|16|D|22|E|20|F|26|G|13|H|05|I|14|J|32|K|36|L|11|M|07|N|06|O|03|
		P|15|Q|35|R|12|S|24|T|01|U|34|V|17|W|31|X|27|Y|25|Z|21|0|56|1|74|2|70|3|64|
		4|62|5|66|6|72|7|60|8|33|9|37|-|52|/|44|'|54|+|46|.|42|;|50|,|40|+|02|_|04
		"""
		Mapper.__init__(self,mapper)

class AnalexBCDMapper(Mapper):																		# Analex Printer / BCD
	def __init__(self):
		mapper = """
        _|20|0|12|1|01|2|02|3|03|4|04|5|05|6|06|7|07|8|10|9|11|A|61|B|62|C|63|D|64|
        E|65|F|66|G|67|H|70|I|71|J|41|K|42|L|43|M|44|N|45|O|46|P|47|Q|50|R|51|S|22|
        T|23|U|24|V|25|W|26|X|27|Y|30|Z|31|.|73|-|40|+|60|=|13|(|34|)|74|/|21|*|54|,|33
		"""
		Mapper.__init__(self,mapper)

class TeletypeMapper(Mapper):																		# 5 bit teletype (note use of figure shift)
	def __init__(self):
		mapper = """
		A|30|B|23|C|16|D|22|E|20|F|26|G|13|H|05|I|14|J|32|K|36|L|11|M|07|N|06|O|03|
		P|15|Q|35|R|12|S|24|T|01|U|34|V|17|W|31|X|27|Y|25|Z|21|
		-|130|?|163|:|116|$|122|3|120|!|126|&|113|#|105|8|114|'|132|(|136|)|111|.|107|,|106|
		9|103|o|115|1|135|4|112|5|101|7|134|;|117|2|131|/|127|6|125|"|121|_|04
		"""
		Mapper.__init__(self,mapper)
	def getShift(self):
		return 0o33
	def getUnshift(self):
		return 0o37

###################################################################################################################
#												OLD CDC160 MNEMONICS
###################################################################################################################

#
#	Simple old style opcode - uses value 00-77 as operand.
#
class OldSimpleOpcode:
	def __init__(self,mnemonic,opcode):
		self.mnemonic = mnemonic 																	# save mnemonic and opcode
		self.opcode = opcode

	def assemble(self,assembler,mnemonic):
		if assembler.getPass() == 2:																# only do it on pass 2.
			operand = assembler.evaluate()															# evaluate, no indirection allowed.
			operand = self.process(mnemonic,operand,assembler.currentAddress())						# process it for F B R etc.
			if operand > 0o77:																		# range error
				raise AssemblerException("Opcode must have operand in range 00-77")
			assembler.word(self.opcode + operand)													# assemble into memory
		else:
			assembler.word(None)																	# pass 1, assemble "None"

	def process(self,mnemonic,operand,pctr):	
		return operand 																				# simple ops do nothing.
#
#	Old style opcode with mode D,I,F,B or R, also INP and OUT are F's as well :)
#
class OldComplexOpcode(OldSimpleOpcode):
	def __init__(self,mnemonic,opcode):
		self.mnemonic = mnemonic
		self.opcode = opcode

	def process(self,mnemonic,operand,pctr):
		mode = mnemonic[-1]																			# mode is D I F B R
		if mnemonic == "jfi" or mnemonic == "inp" or mnemonic == "out":								# three special cases, these are all
			mode = "f"																				# forward referenced.
		if mode == "r":																				# convert R into F/B
			mode = "f" if operand > pctr else "b"
		if mode == "f":																				# process F
			operand = (operand - pctr) & 0o7777
		if mode == "b":																				# process B
			operand = (pctr - operand) & 0o7777
		return operand

###################################################################################################################
#												NEW CDC160 MNEMONICS
###################################################################################################################

#
#	Assembles immediate mode (#) like LPN, indirect @ like LPI, Direct Forward and Backward as appropriate.
#
class NewComplexOpcode(OldSimpleOpcode):
	def __init__(self,immediate,difbBase):
		self.immediateOpcode = immediate
		self.baseOpcode = difbBase

	def assemble(self,assembler,mnemonic):
		if assembler.getPass() == 2:
			operand = assembler.operands
			if operand[0] == '#' or operand[0] == '@':												# immediate or indirect ?
				opcode = self.immediateOpcode														# get opcode & operand
				if operand[0] == '@':																# process indirection.
					opcode = self.baseOpcode + 1
				if operand[0] == '#' and self.immediateOpcode < 0:									# check the immediate mode exists
					raise AssemblerException(mnemonic+" does not have an immediate mode.")
				assembler.operands = operand[1:]													# strip the operand and write back.
				operand = assembler.evaluate() 														# and work it out.
			else:																					# DFB
				opcode = self.baseOpcode 															# figure out opcode/operand
				operand = assembler.evaluate()
				if operand >= 0o100:																# not in page zero ? 
					if operand > assembler.currentAddress():										# forward ?
						operand = operand - assembler.currentAddress()
						opcode += 2
					else:																			# backward ?
						operand = assembler.currentAddress() - operand
						opcode += 3
			if operand < 0 or operand > 0o77:														# bad operand ?
				raise AssemblerException("Operand out of range")
			assembler.word((opcode << 6) | operand)													# assemble it.

		else:
			assembler.word(None)
#
#	Assembles conditional jumps jz,jnz,jpl,jmi, new assembler syntax
#
class NewJumpConditional:
	def __init__(self,forwardOpcode):
		self.forwardOpcode = forwardOpcode

	def assemble(self,assembler,mnemonic):
		if assembler.getPass() == 2:
			operand = assembler.evaluate()															# where we are going
			opcode = self.forwardOpcode
			addr = assembler.currentAddress()														# from where
			if operand >= addr:																		# calculate fwd/bwd jump
				operand = operand - addr 
			else:
				operand = addr - operand
				opcode += 4 																		# if backward adjust opcode
			if operand > 0o77:
				raise AssemblerException("Jump out of range")
			assembler.word((opcode << 6) | operand)													# assemble it.
		else:
			assembler.word(None)

#
#	Assembles jmp @ - can be forward indirect or normal indirect.
#
class NewJumpIndirect:
	def __init__(self):
		pass

	def assemble(self,assembler,mnemonic):
		if assembler.operands[0] != '@':
			raise AssemblerException("Syntax error in indirect jump")
		if assembler.getPass()  == 2:
			opcode = 0o70 																			# Set up for 070 JPI
			assembler.operands = assembler.operands[1:]
			operand = assembler.evaluate() 															# work out target address
			if operand > assembler.currentAddress() and assembler.currentAddress() >= 0o100:		# is it forward ?
				opcode = 0o71 																		# It is now 071 JFI
				operand = operand - assembler.currentAddress() 
			if operand > 0o77:																		# Range error
				raise AssemblerException("Jump indirect out of range")
			assembler.word((opcode << 6) | operand)													# assemble it.
		else:
			assembler.word(None)

###################################################################################################################
#												PSEUDO OP WORKER
###################################################################################################################
#
#	Pseudo-operation worker.
#
class PseudoOperationWorker:
	def __init__(self,mnemonic):
		self.mnemonic = mnemonic
	def assemble(self,assembler,mnemonic):
		if mnemonic == "org":																		# ORG xxxxx
			assembler.counterSel = "prg"
			assembler.counters[assembler.counterSel] = assembler.evaluate() 	
		if mnemonic == "con" or mnemonic == "prg":													# CON/PRG xxxx
			assembler.counterSel = mnemonic															# switch to CON or PRG
			if assembler.operands != "":															# xxxx is optional.
				assembler.counters[assembler.counterSel] = assembler.evaluate() 
		if mnemonic == "blr":																		# BLR xxxx block reserve
			assembler.counters[assembler.counterSel] += assembler.evaluate()
		if mnemonic == "end":																		# END is end of file.
			assembler.passHasEnded = True
		if mnemonic == "equ":																		# EQU set equivalence
			assembler.assignSymbol(assembler.label,assembler.evaluate())
		if mnemonic == "dec":																		# DEC put onstant as decimal
			if re.match("^-?[0-9]+",assembler.operands) is None:									# validate it.
				raise AssemblerException("Bad decimal value "+assembler.operands)
			assembler.word(int(assembler.operands) & 0o7777)										# assemble the word.
		if mnemonic in ["bcd","flx","tty","bcr","flr","ttr"]:										# Text listings.
			isPacked = mnemonic[-1] != "r"															# xxr is single word per character.
			mapper = assembler.ttyMapper 															# pick the mapper
			if mnemonic[:2] == "bc":																# bcd/bcr is Analex printer / BCD
				mapper = assembler.bcdMapper
			if mnemonic[:2] == "fl":																# flx/flr is Flexowriter
				mapper = assembler.flexoMapper
			string = assembler.operands 															# this is what we want to convert
			if string.find("./") >= 0:																# use ./ as an OPTIONAL end marker.
				string = string[:string.find("./")]
			codes = mapper.convert(string)															# convert to codes.
			if isPacked and len(codes) % 2 == 1:													# do we have a need for a padding space.
				space = mapper.convert(" ")															# add a space
				codes = codes + space 								
				assert len(codes) % 2 == 0
			if isPacked:																			# output packed
				for i in range(0,len(codes),2):
					assembler.word((codes[i] << 6) | codes[i+1])
			else:																					# or unpacked
				for c in codes:
					assembler.word(c)
		if mnemonic == "base":																		# BASE set number base
			assembler.numberBase = assembler.evaluate()												# figure out what base
			if assembler.numberBase < 2 or assembler.numberBase > 16:								# outside range
				raise AssemblerException("Assembler default number base must be 2..16")
					
###################################################################################################################
#													ASSEMBLER
###################################################################################################################
#
#	Assembler class
#
class CDC160Assembler:
	#
	#	Set up opcode/pseudo opcode tables, clear memory, create default symbols TEM0-7
	#
	def __init__(self):
		pseudoOpcodes = "org,con,prg,blr,wai,end,equ,dec,bcd,flx,tty,bcr,flr,ttr"					# known pseudo ops 
		opcodes = "err,sha,lpn,lsn,ldn,lcn,adn,sbn,lpd,lpi,lpf,lpb,lsd,lsi,lsf,lsb,ldd,ldi,ldf,"	# old style mnemonics
		opcodes += "ldb,lcd,lci,lcf,lcb,add,adi,adf,adb,sbd,sbi,sbf,sbb,std,sti,stf,stb,srd,sri,"
		opcodes += "srf,srb,rad,rai,raf,rab,aod,aoi,aof,aob,zjf,nzf,pjf,njf,zjb,nzb,pjb,njb,jpi,"
		opcodes += "jfi,inp,out,otn,exf,ina,hlt"

		self.keywords = {} 																			# dictionary of keywords
		for pop in pseudoOpcodes.split(","):														# create the pseudo operations.
			self.keywords[pop] = PseudoOperationWorker(pop)
		n = 0
		for op in opcodes.split(","):																# work through opcodes.
			if op[-1] in ["d","i","f","b"] or op == "inp" or op == "out":							# is it complex (D,I,F,B), or INP or OUT
				self.keywords[op] = OldComplexOpcode(op,n << 6)										# create a complex opcode worker
			else:
				self.keywords[op] = OldSimpleOpcode(op,n << 6)										# create a simple opcode worker.
			if op[-1] == "b":																		# anything with a B should have an R as well
				self.keywords[op] = OldComplexOpcode(op[:-1]+"r",n << 6)
			n += 1
		self.symbols = {}																			# known symbols
		self.memory = [ None ] * 4096 																# memory all set to unused.
		for i in range(0,8):																		# set up tem0 to tem7
			self.assignSymbol("tem{0}".format(i),0O70+i)			
		self.ttyMapper = TeletypeMapper()															# ASCII to character code mappers.
		self.flexoMapper  = FlexowriterMapper()
		self.bcdMapper = AnalexBCDMapper()
		self.extendNewMnemonics()																	# allow new mnemonics

	#
	#	Add the new mnemonics in.
	#
	def extendNewMnemonics(self):

		self.keywords["rol"] = OldSimpleOpcode("rol",0o102) 										# SHA variants
		self.keywords["rol3"] = OldSimpleOpcode("rol3",0o110)
		self.keywords["mul10"] = OldSimpleOpcode("mul10",0o112)

		self.keywords["rol2"] = OldSimpleOpcode("rol2",0o103)										# Assemble the S/N 37 and up opcodes even though
		self.keywords["rol6"] = OldSimpleOpcode("rol6",0o111)										# we aren't using them.
		self.keywords["mul100"] = OldSimpleOpcode("mul100",0o113)
		self.keywords["tpa"] = OldSimpleOpcode("tpa",0o101)


		self.keywords["and"] = NewComplexOpcode(0o02,0o10)											# New style mnemonics
		self.keywords["xor"] = NewComplexOpcode(0o03,0o14)											# these support immediate and DIFB 
		self.keywords["lda"] = NewComplexOpcode(0o04,0o20)
		self.keywords["lca"] = NewComplexOpcode(0o05,0o24)
		self.keywords["add"] = NewComplexOpcode(0o06,0o30)
		self.keywords["sub"] = NewComplexOpcode(0o07,0o34)
		self.keywords["sta"] = NewComplexOpcode(-1,0o40)											# these support just DIFB									
		self.keywords["rml"] = NewComplexOpcode(-1,0o44)									
		self.keywords["adm"] = NewComplexOpcode(-1,0o50)									
		self.keywords["ild"] = NewComplexOpcode(-1,0o54)									
		self.keywords["otc"] = NewComplexOpcode(0o74,-1) 											# otc is one on its own, immediate only.
		self.keywords["jz"] = NewJumpConditional(0o60)												# conditional jumps
		self.keywords["jnz"] = NewJumpConditional(0o61)											
		self.keywords["jpl"] = NewJumpConditional(0o62)											
		self.keywords["jmi"] = NewJumpConditional(0o63)											
		self.keywords["jmp"] = NewJumpIndirect() 													# JPI and JFI both are JMP @xxxx

		self.keywords["base"] = PseudoOperationWorker("base")										# add the base pseudo operation

	#
	#	Assign a symbol
	#
	def assignSymbol(self,name,value):
		name = name.lower()																			# case independent
		if name in self.symbols:																	# if it already exists
			if value != self.symbols[name]:															# throw exception if value has changed
				raise AssemblerException("Symbol "+name+" has changed value.")
		self.symbols[name] = value 																	# assign the value.
	#
	#	Called when starting a pass. Initialise CON and PRG.
	#
	def startPass(self,passNumber):
		self.passNumber = passNumber 																# set pass number
		self.counters = { "prg": 0O100, "con": 0O020 };												# set prg and con counters.
		self.counterSel = "prg"																		# start assembling to PRG.
		self.passHasEnded = False 																	# END can stop early.
		self.numberBase = 8 																		# Current working base.
	#
	#	Get current pass
	#	
	def getPass(self):
		return self.passNumber
	#
	#	Assemble a single line of code.
	#
	def assembleLine(self,line):
		if self.passHasEnded or line.strip() == "":													# ignore after END or empty lines.
			return
		line = line if line.find("//") < 0 else line[:line.find("//")].rstrip()						# remove comments.
		line = line.replace("\t"," ").rstrip()														# replace tabs with spaces.
		self.label = ""
		match = re.match("^(\w+)\s*(.*)$",line)														# look for a label
		if match is not None:																		# do we have a label
			self.label = match.group(1).lower() 													# keep the label
			line = match.group(2)
			if line[:1] == "=":																		# convert = xxxx to equ xxxx
				line = "equ "+line[1:]
			if line[:3] != "equ":																	# if it isn't an equate, then set the label
				self.symbols[self.label] = self.currentAddress()									# to the current counter.

		line = line.strip()																			# remove any spaces.
		if line != "":																				# is the line empty
			match = re.match("^(\w+)\s*(.*)$",line)													# remove what is probably the opcode.
			if match is None:																		# if we don't recognise it, trouble.
				raise AssemblerException("Syntax Error")
			self.mnemonic = match.group(1).lower()													# get the mnemonic
			self.operands = match.group(2).strip()													# and the rest of the line.
			if self.mnemonic != "rem":																# if not a comment
				if self.mnemonic in self.keywords:													# do we recognise it ?
					self.keywords[self.mnemonic].assemble(self,self.mnemonic)						# so assemble it.
				else:
					self.operands = line.strip() 													# no mnemonic and operands is the whole lot.
					if self.passNumber == 1:
						self.word(None)
					else:
						self.word(self.evaluate())													# evaluate the operands and assemble as a data word
	#
	#	get current address
	#
	def currentAddress(self):
		return self.counters[self.counterSel]
	#
	#	Assemble a single word.
	#
	def word(self,word):
		if self.currentAddress() > 0o7777:															# general overflow
			raise AssemblerException("Counter must be < 7777")
		if self.counterSel == "con" and self.currentAddress() > 0o77:								# con out of first page
			raise AssemblerException("Counter for CON must be < 77")
		if word is not None:																		# testing
			print("\t{0:04o}:{1:04o}".format(self.currentAddress(),word))
		self.memory[self.currentAddress()] = word 													# output the word.
		self.counters[self.counterSel] += 1															# bump the counter
	#
	#	Evaluate the operands.
	#
	def evaluate(self):
		result = 0 																					# final result.
		self.isIndirect = False
		for e in self.operands.split(" "):															# work through operands
			e = e.strip().lower()																	# remove spaces
			if e != "":																				# if not null.
				sign = 1																			# extract the sign.
				if e[0] == "-":
					sign = -1
					e = e[1:]
				allowed = "0123456789abcdef"[:self.numberBase]										# permitted characters in number in current base
				if re.match("^["+allowed+"]+$",e):													# is it a number ?
					result = result + sign*int(e,self.numberBase)									# convert it.
				else:
					if e not in self.symbols:														# unknown symbol ?
						raise AssemblerException(e+" is undefined.")
					result = result + sign*self.symbols[e]											# use it
		return result & 0o7777
	#
	#	Assemble a source code collection
	#
	def assemble(self,sourceFiles):
		if type(sourceFiles) is str:																# make a list if isn't.
			sourceFiles = [ sourceFiles ]
		print("OSAP/Python (c) PSR 2015 : assembling "+",".join(sourceFiles))						# be informative.
		for asmPass in range(1,3):																	# multiple passes
			print("    Pass {0}".format(asmPass))
			self.startPass(asmPass)																	# start the pass.
			for f in sourceFiles:																	# work through one or many files.
				src = open(f).readlines()															# read them in
				for n in range(0,len(src)):															# work through each line
					try:																			# try to assemble it.
						self.assembleLine(src[n])
					except AssemblerException, ae:													# catch and rethrow with line #
						print("Error:{0} ({1}:{2})".format(ae.message,f,n+1))						# print an error message
						sys.exit(0)																	# exit the program.
		print("    Assembled {0} (decimal) words.".format(4096-self.memory.count(None)))			# Finished.
	#
	#	Symbol table dump
	#
	def dumpSymbolTable(self):	
		print("    Dumped symbol table to symbol.lst")												# output symbol table.
		keyList = self.symbols.keys()																# get symbols.
		keyList.sort(key = lambda s:self.symbols[s])												# sort it on the address/value
		symbols = ["{1:04o} : {0}".format(k,self.symbols[k]) for k in keyList]						# create it
		open("symbols.lst","w").write("\n".join(symbols))											# dump it.
	#
	#	Convert binary to paper tape.
	#
	def tapeEncode(self,number):
		return ("{0:07b}\n".format(number)).replace("0",".").replace("1","O")
	#	
	#	Output in bi-octal format (alternate half words, first has 7th hole punched, ends when first doesn't have 7th punched)
	#
	def dumpBiOctal(self):
		low = 0o7777																				# Find memory that has been
		high = 0o0000																				# assembled to after page 0.
		for address in range(0o0100,0o10000):
			if self.memory[address] is not None:													# something there ?
				low = min(low,address)																# update the low/high range.
				high = max(low,address)
		paperTape = open("code.bioctal","w")														# start writing it out.
		for address in range(low,high+1):													
			data = self.memory[address] 															# get the assembled word
			data = data if data is not None else 0													# pad with zeros.
			paperTape.write(self.tapeEncode((data >> 6) | 0o100))									# upper half with seventh hole
			paperTape.write(self.tapeEncode(data & 0o77))											# lower half without seventh hole.

		paperTape.write(self.tapeEncode(0o77))														# write a word without seventh hole
																									# terminates the read.
		paperTape.close()
		print("    Bi-Octal tape code.bioctal produced for {0:04o} to {1:04o}".format(low,high))
#
#	Build the assembler.
#
a = CDC160Assembler()																				# create an assembler
arguments = sys.argv[1:]																			# get the arguments.
a.assemble([x for x in arguments if x[0] != '-'])													# assemble anything not beginning with -
a.dumpSymbolTable()
a.dumpBiOctal()