#
#	This is only for the console typewriter.
#
#	Maps cents (Shift+6) onto 0x1C
#		 circle (Shift+Plus) onto 0x1D
#		 dot with hat (Shift+=) onto 0x1E

typewriter = """
aA:30|bB:23|cC:16|dD:22|eE:20|fF:26|gG:13|hH:05|iI:14|jJ:32|kK:36|lL:11|mM:07|nN:06|oO:03|
pP:15|qQ:35|rR:12|sS:24|tT:01|uU:34|vV:17|wW:31|xX:27|yY:25|zZ:21|0):56|1*:74|2@:70|3#:64|
4$:62|5%:66|6!:72|7&:60|8_:33|9(:37|-_:52|/?:44|,+:54|+!:46|..:42|;::50|=!:02|!!:04
""".replace("\t","").replace(" ","").replace("\n","").split("|")

twCodeToCharMap = {}
twCharToCodeMap = {}

for t in typewriter:
	mapping = { "lower":t[0], "upper":t[1], "code":int(t[3:],8) }
	if mapping["code"] == 0o4:
		mapping["lower"] = mapping["upper"] = " "
	if mapping["upper"] == "!":
		if mapping["code"] == 58:
			mapping["upper"] = chr(0x1C)
		if mapping["code"] == 38:
			mapping["upper"] = chr(0x1D)
		if mapping["code"] == 2:
			mapping["upper"] = chr(0x1E)

	assert mapping["code"] not in twCodeToCharMap

	twCodeToCharMap[mapping["code"]] = mapping

	twCharToCodeMap[ord(mapping["lower"])] = { "code":mapping["code"],"upper":False }
	twCharToCodeMap[ord(mapping["upper"])] = { "code":mapping["code"],"upper":True }

print(twCodeToCharMap)

codeToChar = [0] * 128
for i in range(0,64):
	if i in twCodeToCharMap:
		codeToChar[i*2] = ord(twCodeToCharMap[i]["upper"])
		codeToChar[i*2+1] = ord(twCodeToCharMap[i]["lower"])

codeToChar = ",".join([str(x) for x in codeToChar])
open("codeToChar.h","w").write(codeToChar)

print(codeToChar)
print(twCodeToCharMap[24])