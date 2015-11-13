#
#	Convert test.bin to a ROM image.
#
print("Converting ROM.")
try:
	h = open("test.bin","rb")
	rom = h.read(8192)
	rom = [ord(x) for x in rom]
except:
	rom = [0xFF] * 8192


open("__romimage.h","w").write("static ROMMEMORY romMemory[] = { "+",".join([str(x) for x in rom])+"};\n")