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

for i in range(32,96):
	if fonts[i] is not None:
		#print(i,fonts[i])
		assert(len(fonts[i]) == 5)
		n = 0
		b = 1
		for y in range(0,5):
			for x in range(0,3):
				if (fonts[i][y] & (0x01 << x)) != 0:
					n |= b
				b = b * 2
		fonts[i] = n
	else:
		print("Missing "+chr(i)+" "+str(i))

data = ",".join([str(x) for x in fonts[32:96]])
open("__smallfont.h","w").write(data+"\n")