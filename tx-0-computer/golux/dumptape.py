#
#	Dump golux.tape
#
src = open("golux.tape").readlines()
c = 0
for p in [x.strip() for x in src if x.strip() != ""]:

	if c == 0:
		ac = 0

	ac = ac >> 1
	for bit in range(0,6):
		if p[bit] == 'O':
			ac = ac | (0o400000 >> (bit * 3))

	c += 1

	if c == 3:
		c = 0
		print("{0:06o}".format(ac))
