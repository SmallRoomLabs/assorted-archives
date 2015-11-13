#
#	Font compiler
#

fonts = [ None ] * 128

for l in open("font.txt").readlines():
	#print(l.strip())
	if l.strip() != "" and l[0] != '#':
		if l[0] == ':':
			current = ord(l[1])
			if l[1:3] == "CH":
				current = int(l[3:])
			fonts[current] = []
		else:
			l = l.strip().replace(".","0").replace("X","1")
			assert len(l) == 3
			fonts[current].append(int(l,2))

for i in range(0,128):
	if fonts[i] is not None:
		#print(i,fonts[i])
		assert(len(fonts[i]) == 5)
		n = 0
		b = 1
		for y in range(0,5):
			for x in range(0,3):
				if (fonts[i][4-y] & (0x04 >> x)) != 0:
					n |= b
				b = b * 2
		fonts[i] = n | 0x8000
	else:
		print("Missing "+chr(i)+" "+str(i))

ascii6 = [ None ] * 64

for i in range(32,96):
	ascii6[i & 0x3F] = { "pattern":fonts[i], "char":chr(i) }

handle = open("smallfont.asm","w")
handle.write(";\n; 6 bit ASCII 3 x 5 font, bottom to top, left to right\n;\n")
for i in range(0,64):
	handle.write("    dw {0:5}\n".format(ascii6[i]["pattern"],ascii6[i]["char"]))