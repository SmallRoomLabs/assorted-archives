#
#	Definition File Processor
#
import re

mnemonics = [ None ] * 256
code = [ None ] * 256

lines = open("scmp.def").readlines()														# read definition file.
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]						# remove comments.
lines = [x.replace("\t"," ").strip() for x in lines]										# strip and remove spaces
lines = [x for x in lines if x != ""]														# remove empty lines.

for l in lines:
	print(l)
	match = re.match("^([0-9A-F\-]+)\s+(\d+)\s+\"([A-Z\$\(\)\#\$\s\@0-3]+)\"\s+(.*)$",l)	# split up.
	assert(match is not None)
	oStart = oEnd = int(match.group(1)[:2],16)												# range of opcodes
	if len(match.group(1)) == 5:
		oEnd = int(match.group(1)[-2:],16)
	for opcode in range(oStart,oEnd+1):
		assert(mnemonics[opcode] is None)
		mnemonics[opcode] = match.group(3).replace("$P","P"+str(opcode % 4)).lower()
		mnemonics[opcode] = mnemonics[opcode].replace("(p0)","(pc)")
		code[opcode] = match.group(4).replace("$P","P"+str(opcode % 4))+";CYCLES({0});break;".format(match.group(2))
		code[opcode] = code[opcode].replace(";;",";")

for i in range(0,256):																		# replace None with ""
	mnemonics[i] = mnemonics[i] if mnemonics[i] is not None else ""
																							# Dump mnemonics
open("__mnemonics.h","w").write("static const char *_mnemonics[] = { "+",".join(['"'+x+'"' for x in mnemonics])+" };\n")

handle = open("__opcodes.h","w")
for i in range(0,256):
	if code[i] is not None:
		handle.write("case 0x{0:02x}: // **** {1} ****\n".format(i,mnemonics[i]))
		handle.write("    {0}\n".format(code[i]))