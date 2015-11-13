#
#	Process CDC160 File.
#	
import re

#//		$EAC 	Effective address calculation to Z based on 2 x LSB of opcode. Initially replaces with $EACF,$EACI,$EACD,$EACB
#//				Note 'I' increments cycles by 1 if cycle value is -ve. The $EACx are replaced by the code.

def process(s,op):
	n = op % 4
	s = s.replace("$D",["%C","(%C)","%F","%B"][n])
	s = s.replace("$M",["D","I","F","B"][n])
	s = s.replace("$J",["ZJ","NZ","PJ","NJ"][n])
	s = s.replace("$K",["Z","NZ","P","M"][n])
	s = s.replace("$TEST",["A == 0","A != 0","(A & 04000) == 0","(A & 04000) != 0"][n])
	s = s.replace("$EAC",["$EA_D","$EA_I","$EA_F","$EA_B"][n])

	s = s.replace("$EA_D","S = Z & 077")
	s = s.replace("$EA_I","S = Z & 077;READ();S = Z")
	s = s.replace("$EA_F","S = ((P + (Z & 077)) & 07777)")
	s = s.replace("$EA_B","S = ((P - (Z & 077)) & 07777)")
	s = s.replace("$INC","P = (P + 1) & 07777")

	while s.find(";;") >= 0:
		s = s.replace(";;",";")
	return s

lines = open("cdc160.def").readlines()
lines = [x if x.find("//") < 0 else x[:x.find("//")] for x in lines]
lines = [x.replace("\t"," ").strip() for x in lines]
lines = [x for x in lines if x != ""]

mnemonicOld = [None] * 64
mnemonicNew = [None] * 64
code = [None] * 64

for l in lines:
	print(l)
	m = re.match("^([0-7\-]+)\s+([0-9\-]+)\s+\"(.*?)\"\s+\"(.*?)\"\s+(.*)$",l)

	oStart = oEnd = int(m.group(1)[:2],8)
	if len(m.group(1)) == 5:
		oEnd = int(m.group(1)[-2:],8)

	for opcode in range(oStart,oEnd+1):
		cycles = int(m.group(2))
		if cycles < 0 and opcode % 4 == 1:
			cycles -= 1
		cycles = abs(cycles)

		assert mnemonicNew[opcode] is None
		mnemonicOld[opcode] = process(m.group(3),opcode).lower()
		mnemonicNew[opcode] = process(m.group(4),opcode).lower()

		code[opcode] = process(m.group(5)+";CYCLES({0});break;".format(abs(cycles)),opcode)

open("__mnemonics_new.h","w").write("static char *_mnemonics[] = {"+",".join('"'+x+'"' for x in mnemonicNew)+"};")
open("__mnemonics_old.h","w").write("static char *_mnemonics[] = {"+",".join('"'+x+'"' for x in mnemonicOld)+"};")

handle = open("__opcodes.h","w")
for n in range(0,64):
	handle.write("case 0{0:02o}: /* {1} */\n".format(n,mnemonicOld[n][:3]))
	handle.write("\t{0}\n".format(code[n].strip()))