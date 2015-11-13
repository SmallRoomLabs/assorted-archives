#
#	Font text file parser.
#	
import re

def toDecimal(s):
	s = (s + "..........")[:8].replace(".","0")
	s = re.sub("[A-Z]","1",s)
	return int(s,2)

lines = open("font.txt").readlines()
lines = [x.strip() for x in lines if x.strip() != ""]

charSet = [ None ] * 64 
for i in range(0,64):
	charSet[i] = { "small":[], "large":[] }

current = -1
for l in lines:
	if l[0] >= '0' and l[0] <= '9':
		current = int(l.split(" ")[0])
	if l[0] == '.' or l[0] == 'X':
		m = re.match("^([A-Z\.]+)\s*([A-Z\.]*)\s*$",l)
		assert m is not None
		charSet[current]["large"].append(toDecimal(m.group(1)))
		if m.group(2) != "":
			charSet[current]["small"].append((m.group(2)+"....")[:3])

fontLarge = []
fontSmall = []

for i in range(0,64):

	assert len(charSet[i]["large"]) == 9
	fontLarge = fontLarge + charSet[i]["large"]

	assert len(charSet[i]["small"]) == 5
	for col in range(0,3):
		rowData = 0
		for row in range(0,5):
			if charSet[i]["small"][row][col] != ".":
				rowData = rowData + (128 >> row)
		fontSmall.append(rowData)

src = "static BYTE8 _font8678[] = { "+",".join([str(x) for x in fontLarge])+ "};"
open("__largeFont.h","w").write(src)

src = "static BYTECONSTANT _font8678[] = { "+",".join([str(x) for x in fontSmall])+ "};"
open("__smallVerticalFont.h","w").write(src)
