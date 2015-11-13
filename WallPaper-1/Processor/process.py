#
#		SC/MP Processor.
#
import re

lines = open("scmp.def").readlines()												# read file
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]				# remove comments
lines = [x.strip().replace("/t"," ") for x in lines if x.strip() != ""]				# remove spaces, blanks, tabs.
																					# dump out C
open("_cpusupport.h","w").write("\n".join([x[1:].strip() for x in lines if x[0] == ":"]))

mnemonics = [ None ] * 256															# mnemonics/code
code = [ None ] * 256

for l in [x for x in lines if x[0] != ':']:											# work through opcodes
	print(l)
	m = re.match("^([A-Z0-9\-]+)\s+\"(.*?)\"\s+([0-9]+)\s+(.*)$",l)					# Split into parts
	rStart = rEnd = int(m.group(1)[:2],16)											# Work out opcode range.
	if len(m.group(1)) == 5:
		rEnd = int(m.group(1)[-2:],16)

	for opcode in range(rStart,rEnd+1):												# work through opcodes
		assert(mnemonics[opcode] is None)											# check duplicates
																					# compose mnemonics
		mnemonics[opcode] = m.group(2).replace("$P","P"+str(opcode % 4)).replace("$N(P0)","$O")
		code[opcode] = m.group(4).replace("$P","P"+str(opcode % 4))+";"				# build code
		code[opcode] = code[opcode] + "CYCLES = CYCLES + {0};break;".format(m.group(3))
		code[opcode] = code[opcode].replace("@CHECKEX","if (OPERAND == 0x80) OPERAND = EX")
		code[opcode] = code[opcode].replace("@EAC","EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;EA = (EA + P{0}) & 0xFFFF".format(opcode % 4))
		code[opcode] = code[opcode].replace("@PREINDEX","if ((OPERAND & 0x80) != 0) P{0} = (P{0} - 256 + OPERAND) & 0xFFFF".format(opcode % 4))
		code[opcode] = code[opcode].replace("@POSTINDEX","if ((OPERAND & 0x80) == 0) P{0} = P{0} + OPERAND) & 0xFFFF".format(opcode % 4))

handle = open("_opcodes.h","w")
for opcode in range(0,256):
	if mnemonics[opcode] != None:
		handle.write("case 0x{0:02x}: /*** {1} ***/\n".format(opcode,mnemonics[opcode]))
		for l in code[opcode].split(";"):
			if l != "":
				handle.write("\t{0};\n".format(l))