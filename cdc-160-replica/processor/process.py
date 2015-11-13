#
#	CDC160 Processor
#

import re

def process(s,op):
	s = s.replace("$M",["D","I","F","B"][op % 4])
	s = s.replace("$EAC",["EACD()","EACI()","EACF()","EACB()"][op % 4])
	s = s.replace("$A",["%D","@%D","%F","%B"][op % 4])
	s = s.replace("$J",["ZJ","NZ","PJ","NJ"][op % 4])
	s = s.replace("$C",["Z","NZ","PL","MI"][op % 4])
	s = s.replace("$T",["A == 0","A != 0","(A & 04000) == 0","(A & 04000) != 0"][op % 4])
	s = s.replace("$N","P = (P + 1) & 07777")
	return s

header = "/*** Generated Code, do not edit ***/\n\n"

lines = open("cdc160.def").readlines()
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]
lines = [x.replace("\t"," ").strip() for x in lines]
lines = [x for x in lines if x != ""]

open("__CDC160Support.h","w").write(header+"\n".join([x[1:] for x in lines if x[0] == ':']))

lines = [x for x in lines if x[0] != ':']
mnemonics = [ None ] * 64
code = [ None ] * 64

for l in lines:
	print(l)
	m = re.match("^([0-7\-]+)\s+([0-9]+)\s+\"(.*?)\"\s+(.*)$",l)
	oStart = oEnd = int(m.group(1)[:2],8)
	if len(m.group(1)) > 2:
		oEnd = int(m.group(1)[3:5],8)
	for opcode in range(oStart,oEnd+1):
		assert mnemonics[opcode] is None
		mnemonics[opcode] = process(m.group(3),opcode).lower().strip()
		cycles = int(m.group(2))
		if opcode >= 0O10 and opcode < 0O60 and opcode % 4 == 1:
			cycles += 1;
		code[opcode] = process(m.group(4),opcode)+";cycles += {0};break;".format(cycles)

handle = open("__CDC160Mnemonics.h","w")
handle.write(header)
handle.write("#ifdef NEWMNEMONICS\n")
handle.write("static const char *_mnemonics[] = { " + ",".join(['"'+x.split("/")[1]+'"' for x in mnemonics])+"};\n")
handle.write("#else\n")
handle.write("static const char *_mnemonics[] = { " + ",".join(['"'+x.split("/")[0]+'"' for x in mnemonics])+"};\n")
handle.write("#endif\n")

handle = open("__CDC160Opcodes.h","w")
handle.write(header)
for n in range(0,64):
	if code[n] is not None:
		handle.write("case 0{0:02o}: // {1}\n".format(n,mnemonics[n]))
		handle.write("    {0}\n".format(code[n]))