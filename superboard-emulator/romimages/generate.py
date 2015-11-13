#
#
#


def generate(romImage,cFile,name):
	print("Processing "+romImage)
	romImage = open(romImage,"rb").read(-1)
	romImage = [ord(x) for x in romImage]
	print("Size "+str(len(romImage)))
	romImage = ",".join([str(x) for x in romImage])

	open(cFile,"w").write("CONSTTYPE "+name+ "[] = { "+romImage + "}; ")
	print("Generated "+cFile)

generate("CHARGEN.ROM","__rom_char.h","romCharacters")
generate("OSIBASIC.ROM","__rom_basic.h","romBasic")
generate("SYN600.ROM","__rom_monitor.h","romMonitor")