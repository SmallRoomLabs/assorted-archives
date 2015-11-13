#
#	Font compiler
#
fonts = [ None ] * 64
current = -1

for l in open("font.txt").readlines():
	print(l)
	if l.strip() != "":
		if l[0] == ':':
			current = ord(l[1]) & 0x3F
			fonts[current] = []
		else:
			l = l.strip().replace(".","0").replace("X","1")
			assert len(l) == 3
			fonts[current].append(int(l,2))

for i in range(0,64):
	print(i,fonts[i])
	assert(len(fonts[i]) == 5)
	n = 0
	for g in fonts[i]:
		n = n * 8 + g
	n = n * 2 + 1
	fonts[i] = n

open("fonttable.asm","w").write("\n".join(["  dw "+str(x) for x in fonts]))