#
#	Font compiler
#
def bitReverse(n):
	r = 0
	for i in range(0,16):
		r = r << 1
		r = r + (n & 1)
		n = n >> 1
	return r 

def bitCheck(ch,bits):
	shCount = 0
	for i in range(0,5):
		if ((bits >> 8) ^ (bits & 0xFF)) == 1:
			print(ch,shCount)
		bits = bits >> 3
		shCount += 1

def writeFont(fontFile,characters):
	handle = open(fontFile,"w")
	handle.write(";\n")
	handle.write("; {0} characters \"{1}\"\n".format(fontFile,characters))
	handle.write(";\n")
	for c in characters:
		handle.write("    dw {0}\n".format(fonts[ord(c)]))
	handle.close()
	print("Created "+fontFile)
fonts = [ None ] * 128
current = -1

for l in open("font.txt").readlines():
#	print(l)
	if l.strip() != "":
		if l[0] == ':':
			current = ord(l[1].lower())
			fonts[current] = []
		else:
			l = l.strip().replace(".","0").replace("X","1")
			assert len(l) == 3
			fonts[current].append(int(l,2))

for i in range(1,27):
	fonts[64+i] = fonts[96+i]

for i in range(32,127):
	if fonts[i] is not None:
		#print(i,fonts[i])
		assert(len(fonts[i]) == 5)
		n = 0
		for g in fonts[i]:
			n = n * 8 + g
		n = n * 2 + 1
		fonts[i] = bitReverse(n)
		bitCheck(chr(i),fonts[i])
	else:
		print("Missing "+chr(i)+" "+str(i))


writeFont("octal_mon_font.asm","01234567:")

print("{0:x} {0}".format(fonts[48]))