#
#	Definition File Processor
#
import re

mnemonics = [ None ] * 256
code = []

lines = open("rca1802.def").readlines()														# read definition file.
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]						# remove comments.
lines = [x.replace("\t"," ").strip() for x in lines]										# strip and remove spaces
lines = [x for x in lines if x != ""]														# remove empty lines.

open("__1802support.h","w").write("\n".join(x[1:] for x in lines if x[0] == ':'))			# output C
lines = [x for x in lines if x[0] != ":"]													# then remove it.

for l in lines:
	print(l)
	match = re.match("^([0-9A-F\-]+)\s+(\d+)\s+\"(.*?)\"\s+(.*)$",l)						# split up.
	assert(match is not None)
	oStart = oEnd = int(match.group(1)[:2],16)												# range of opcodes
	if len(match.group(1)) == 5:
		oEnd = int(match.group(1)[-2:],16)
	for opcode in range(oStart,oEnd+1):
		assert mnemonics[opcode] is None
		mnemonics[opcode] = match.group(3).replace("$L","{0:X}".format(opcode % 16)).replace("$P",str(opcode % 8)).replace("$F",str(opcode % 4+1))
	codeEntry = { "start":oStart, "end":oEnd, "code":match.group(4).replace("$REG","*pReg"),"cycles":int(match.group(2)) }
	code.append(codeEntry)

for i in range(0,256):
	if mnemonics[i] is None:
		print("{0:02x}".format(i))								
																							# Dump mnemonics
open("__1802mnemonics.h","w").write("static const char *_mnemonics[] = { "+",".join([('"'+x+'"').lower() for x in mnemonics])+" };\n")

code = sorted(code, key=lambda ce:ce["start"])

handle = open("__1802opcodes.h","w")
for ce in code:
	handle.write("// **************** {0} ***************\n".format(mnemonics[ce["start"]]))
	for opc in range(ce["start"],ce["end"]+1):
		handle.write("case 0x{0:02x}:\n".format(opc))
	handle.write("    {0};\n".format(ce["code"]))
	handle.write("    cycles += {0};\n".format(ce["cycles"]))
	handle.write("    break;\n")

handle = open("__1802ports.h","w")
for i in range(1,8):
	handle.write("#ifndef INPORT{0}\n".format(i))
	handle.write("#define INPORT{0}() (DEFAULT_BUS_VALUE)\n".format(i))
	handle.write("#endif\n")
	handle.write("#ifndef OUTPORT{0}\n".format(i))
	handle.write("#define OUTPORT{0}(n) {{}}\n".format(i))
	handle.write("#endif\n")
handle.write("#ifndef SETQON\n")
handle.write("#define SETQON() {}\n")
handle.write("#endif\n")
handle.write("#ifndef SETQOFF\n")
handle.write("#define SETQOFF() {}\n")
handle.write("#endif\n")
for i in range(1,5):
	handle.write("#ifndef EFLAG{0}\n".format(i))
	handle.write("#define EFLAG{0}() (0)\n".format(i))
	handle.write("#endif\n")
