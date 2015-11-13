import re

modes = {}

modes["im"] = { "eac":"", "cycles":-2, "desc":"#@1" }
modes["dr"] = { "eac":"eac = fetchByte()","cycles":-1,"desc":"@1" }
modes["ex"] = { "eac":"fetchWordTemp16();eac = temp16;","cycles":0,"desc":"@1@2" }
modes["in"] = { "eac":"eac = fetchByte()+ix;","cycles":0,"desc":"@1,X" }

mnemonics = [ None ] * 256
codeList =  [ None ] * 256

def defineOpcode(opcode,mnemonic,code,cycles):
	assert mnemonics[opcode] is None
	mnemonics[opcode] = mnemonic.lower() 
	codeList[opcode] = "CYCLES({0});{1}".format(cycles,code)
	#print("{1:02x} {0} {2}".format(mnemonics[opcode],opcode,codeList[opcode]))

src = open("6800.def").readlines()
src = [x if x.find("//") < 0 else x[:x.find("//")] for x in src]
src = [x.strip().replace("\t"," ") for x in src if x.strip() != ""]

open("__6800support.h","w").write("\n".join([x[1:] for x in src if x[0] == ':']))

src = [x for x in src if x[0] != ':']

for l in src:
	print(l)
	m = re.match("^\"([\w\s\%@\#]+)\"\s+(\d+)\s+([\(\)\w\:\,]+)\s+(.+)",l)
	assert m is not None

	if len(m.group(3)) == 2:
		defineOpcode(int(m.group(3),16),m.group(1),m.group(4),int(m.group(2)))
	else:
		opcodes = re.match("^\((.*)\)$",m.group(3))
		for mode in opcodes.group(1).split(","):
			m2 = re.match("^(\w+)\:([0-9A-F]+)$",mode)
			assert(m2 is not None)
			mode = m2.group(1).lower()
			body = m.group(4)
			assert mode in modes
			if mode == "dr":
				body = body.replace("readByte(","readBytePage0(").replace("writeByte(","writeBytePage0(")
			if mode == "im":
				body = body.replace("readByte(eac)","fetchByte()")
			mnemonic = m.group(1).replace("@O",modes[mode]["desc"])
			body = body.replace("@EAC",modes[mode]["eac"],int(m.group(2)))
			defineOpcode(int(m2.group(2),16),mnemonic,body,int(m.group(2))+modes[mode]["cycles"])

for i in range(0,256):
	if mnemonics[i] is None:
		mnemonics[i] = "byte {0:02x}".format(i)

m = ['"'+m+'"' for m in mnemonics]
open("__6800mnemonics.h","w").write("static char *_mnemonics[] = { "+",".join(m) + "};")

handle = open("__6800opcodes.h","w")
for i in range(0,256):
	if codeList[i] is not None:
		handle.write("case 0x{0:02x}: /* ${0:02x} {1} */\n".format(i,mnemonics[i]))
		handle.write("\t{0};break;\n".format(codeList[i]).replace(";;",";"))

