#
#		Code generator, phased CPU.
#
import re

def process(txt,op):
	tests = ["Carry == 0","PSZValue != 0","(PSZValue & 0x80) == 0","_CPUParityEven(PSZValue) == 0","Carry != 0","PSZValue == 0","(PSZValue & 0x80) != 0","_CPUParityEven(PSZValue) != 0"]
	txt = txt.replace("$R","ABCDEHLM"[op & 7])
	txt = txt.replace("$H","{0:02x}".format((op & 7) * 8))
	txt = txt.replace("$P","{0:02x}".format(op & 0x1F))
	txt = txt.replace("$C",["nc","nz","p","po","c","z","m","pe"][op & 7])
	txt = txt.replace("$T",tests[op & 7])
	return txt

opcode = [ None ] * 256															# nothing initially
mnemonics = [ None ] * 256
current = None 																	# current opcode structure

src = open("8008.def").readlines()												# read file in.
src = [x if x.find("//") < 0 else x[:x.find("//")] for x in src]				# remove comment lines.
src = [x.replace("\t"," ").rstrip() for x in src]								# tabs to spaces, and right strip
src = [x for x in src if x != ""]												# remove empty lines.

for l in src:																	# scan file.
	m = re.match("^([0-9A-F\-\,]+)\s+(\d+)\s+\"(.*)\"$",l)						# look for matching instruction def.
	if m is not None:
		current = { "mnemonic":m.group(3),"cycles":int(m.group(2)), "phases": [] }
		r = m.group(1)
		if len(r) == 2:
			r = r + "-" + r
		if len(r) == 5:
			r = r + ",1"
		opcodes = range(int(r[:2],16),int(r[3:5],16)+1,int(r[-1]))
		current["step"] = int(r[-1])
		for op in opcodes:
			assert opcode[op] is None
			opcode[op] = current

	else:	
		m = re.match("^\s+([0\*])\s+([0\*])\s+(.*)\[(.*)\:(.*)\]$",l)
		assert m is not None
		phase = { "code":m.group(3).strip()+";".replace(";;",";") }
		phase["leftstatus"] = 0 if m.group(1) == "0" else 1
		phase["rightstatus"] = 0 if m.group(2) == "0" else 1
		phase["status"] = phase["leftstatus"] * 2 + phase["rightstatus"]
		phase["addressLamps"] = m.group(4)
		phase["dataLamps"] = m.group(5)
		current["phases"].append(phase)
		current["phaseCount"] = len(current["phases"])

handle = open("__8008opcodes.h","w")
for i in range(0,256):
	mnemonics[i] = "byte {0:02x}".format(i)
	if opcode[i] is not None:
		current = opcode[i]
		n = i / current["step"]
		mnemonics[i] = process(current["mnemonic"],n).lower()
		handle.write("case 0x{0:02x}: /* {0:02x} {1} */\n".format(i,mnemonics[i]))
		for p in range(0,len(current["phases"])):
			phase = len(current["phases"]) - p - 1
			dat = current["phases"][phase]
			code = process(dat["code"],n)
			code = code + "STATUS({0});DISPLAY({1},{2});".format(dat["status"],dat["addressLamps"],dat["dataLamps"])
			if phase == len(current["phases"])-1:
				code = code + "CYCLES({0});cpuPhase = (0);".format(opcode[i]["cycles"])
			else:
				code = code + "cpuPhase = {0};".format(phase+2)

			code = "if (cpuPhase == {0}) {{ {1} }}".format(phase+1,code)
			handle.write("    {0}\n".format(code))
		handle.write("    break;\n\n")
handle.close()

m = ",".join('"'+x+'"' for x in mnemonics)
open("__8008mnemonics.h","w").write("static const char *__mnemonics[] = { "+m+"};\n")