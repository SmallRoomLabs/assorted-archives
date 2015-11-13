#
#	8008 Processor
#

import re

def process(s,op):
	s = s.replace("$SRC",["A","B","C","D","E","H","L","M"][op % 8])
	s = s.replace("$DST",["A","B","C","D","E","H","L","M"][int(op/8) % 8])
	s = s.replace("$SCODE",["A","B","C","D","E","H","L","_ReadHL()"][op % 8])
	s = s.replace("$TNAME",["NC","NZ","P","PO","C","Z","M","PE"][int(op/8) % 8])
	s = s.replace("$TTEST",["Carry == 0","PSZValue != 0","(PSZValue & 0x80) == 0","_Parity(PSZValue) != 0","Carry != 0","PSZValue == 0","(PSZValue & 0x80) != 0","_Parity(PSZValue) == 0"][int(op/8) % 8])
	s = s.replace("$RST","{0:02X}".format(op & 0x38))
	s = s.replace("$IOPORT","{0:02X}".format((op >> 1) & 0x1F))
	return s

lines = open("8008.def").readlines()
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]
lines = [x.replace("\t"," ").strip() for x in lines]
lines = [x for x in lines if x != ""]

open("__8008support.h","w").write("\n".join([x[1:] for x in lines if x[0] == ':']))

lines = [x for x in lines if x[0] != ':']
mnemonics = [ "" ] * 256
code = [ None ] * 256

for l in lines:
	print(l)
	m = re.match("^([0-9A-F\-\,]+)\s+([0-9]+)\s+\"(.*?)\"\s+(.*)$",l)
	oStart = oEnd = int(m.group(1)[:2],16)
	oStep = 1
	if len(m.group(1)) > 2:
		oEnd = int(m.group(1)[3:5],16)
	if len(m.group(1)) > 5:
		oStep = int(m.group(1)[-1])

	opcode = oStart
	while opcode <= oEnd:
		assert code[opcode] is None
		mnemonics[opcode] = process(m.group(3),opcode).lower()
		cycles = int(m.group(2))
		if opcode >= 0x80 and opcode < 0xFF and (opcode & 7) == 7:
			cycles += 3
		code[opcode] = process(m.group(4),opcode)+";CYCLES({0});break;".format(cycles)
		opcode += oStep


for i in range(0,256):
	if mnemonics[i] == "":
		mnemonics[i] = "db {0:02x}".format(i)

open("__8008mnemonics.h","w").write("static const char *_mnemonics[] = { " + ",".join(['"'+x.lower()+'"' for x in mnemonics])+"};\n")

handle = open("__8008opcodes.h","w")
for n in range(0,256):
	if code[n] is not None:
		handle.write("case 0x{0:02x}: // {1}\n".format(n,mnemonics[n]))
		handle.write("    {0}\n".format(code[n]))

handle = open("__8008ports.h","w")
for n in range(0,32):
	name = "READPORT" if n < 8 else "WRITEPORT"
	name = "{0}{1:02X}".format(name,n)
	handle.write("#ifndef {0}\n#define {0}() {{}}\n#endif\n".format(name))

