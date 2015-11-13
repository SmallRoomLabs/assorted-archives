#
#	Generate old mnemonics
#
#	3 byte 	40-7F
#	2 byte  04-3C,8 / 06-3E,8
#
#	Mapping characters 0-25, Numbers 0-7 (26-33)
#	This is 34 characters ; to pack into 32, any value >= "V" (22) has two 
# 	subtracted as there are no V or W in the opcodes.
#
mnemonics = [ None ] * 256
reg = "ABCDEHLM"
alu = [ "ad","ac","su","sb","nd","xr","or","cp" ]
cond = "czsp"
letters = [ None ] * 26

def setm(mnemonic,opcode):
	mnemonic = mnemonic.upper()
	assert mnemonics[opcode] is None
	mnemonics[opcode] = mnemonic
	for l in mnemonic:
		if l >= "A" and l <= "Z":
			letters[ord(l)-ord('A')] = mnemonic

for r1 in range(0,8):
	for r2 in range(0,8):
		if r1 != 7 or r2 != 7:
			setm("l"+reg[r2]+reg[r1],0xC0+r2*8+r1)

for r in range(0,7):
	setm("l"+reg[r]+"i",0x06+r*8)
for r in range(1,6):	
	setm("in"+reg[r],0x00+r*8)
	setm("dc"+reg[r],0x01+r*8)

for op in range(0,8):
	setm(alu[op]+"i",0x04+op*8)
	for r in range(0,8):
		setm(alu[op]+reg[r],0x80+op*8+r)

setm("rlc",0x02)
setm("rrc",0x0A)
setm("ral",0x12)
setm("rar",0x1A)

for c in range(0,8):
	setm("jmp",0x44+c*8)
	setm("call",0x46+c*8)
	setm("ret",0x07+c*8)

for c in range(0,4):
	setm("jf"+cond[c],0x40+c*8)
	setm("jt"+cond[c],0x60+c*8)
	setm("cf"+cond[c],0x42+c*8)
	setm("ct"+cond[c],0x62+c*8)
	setm("rf"+cond[c],0x03+c*8)
	setm("rt"+cond[c],0x23+c*8)

for c in range(0,8):
	setm("rs"+str(c),0x05+c*8)

for c in range(0,32):
	m = "{0}{1:02o}".format("i" if c < 8 else "o",c)
	setm(m,0x41+c*2)
setm("hlt",0x00)
setm("hlt",0x01)
setm("hlt",0xFF)

for i in range(0,256):
	if mnemonics[i] is None:
		mnemonics[i] = "{0:03o}".format(i).upper()
print(mnemonics)

for i in range(0,26):
	if letters[i] is None:
		print(chr(i+65)+" not used")
	else:
		print(chr(i+65)+" " +	letters[i])