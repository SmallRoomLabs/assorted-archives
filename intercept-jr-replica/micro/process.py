#
#	Process opr.def
#
import re

source = open("opr.def").readlines()
source = [x if x.find("//") < 0 else x[:x.find("//")] for x in source]
source = [x.strip().replace("\t"," ") for x in source if x.strip() != ""]

ops = {} 																	# opcode -> record
mnemonicCode = {} 															# mnemonic -> record

for l in source:
	#print(l)
	m = re.match("^([0-7]+)\s*([A-Z\s\@]+)\s*([0-9]+)\s*(.*)$",l)
	assert m is not None

	opcode = int(m.group(1),8)												# opcode
	cycles = int(m.group(3),10)												# CPU cycles
	code = m.group(4).strip()+";"											# code
	mnemonic = m.group(2).strip().lower()									# mnemonic

	while code.find("@") >= 0:												# while more substitutions.
		p = code.find("@")													# find the @
		replace = code[p+1:p+4].lower()										# this is what we are replacing.
		assert replace in mnemonicCode										# check it is known.
		repRec = mnemonicCode[replace]										# this is the replacer
		code = code[:p]+repRec["code"]+code[p+4:]							# rebuild line.
		if mnemonic[0] == '@':												# if required construct full mnemonic
			mnemonic = mnemonic + " " + repRec["mnemonic"]

	while code.find(";;") >= 0:												# remove double semicolons
		code = code.replace(";;",";")

	if mnemonic[0] == '@':													# remove the builder marker
		mnemonic = mnemonic[1:].strip()

	rec = {"opcode":opcode,"cycles":cycles,"code":code,"mnemonic":mnemonic}	# build and store record
	ops[opcode] = rec
	mnemonicCode[mnemonic] = rec

	#print(rec)

hMnemonics = open("__im6100_mnemonics.h","w")
hOpcodes = open("__im6100_opcodes.h","w")

opcodes = ops.keys()
opcodes.sort()

for op in opcodes:
	case = "case 0{0:04o}: /****** {1} ******/\n".format(op,ops[op]["mnemonic"])
	hMnemonics.write(case)
	hMnemonics.write("    mnemonic = \"{0}\";break;\n".format(ops[op]["mnemonic"]))
	hOpcodes.write(case)
	code = "{0}CYCLES({1});break;".format(ops[op]["code"],ops[op]["cycles"])
	code = code.replace(";",";#").split("#")
	code = "\n".join(["    "+x.strip() for x in code if x.strip() != "" and x.strip() != ";"])
	hOpcodes.write(code+"\n\n")
