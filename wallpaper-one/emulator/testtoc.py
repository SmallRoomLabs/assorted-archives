#
#	Convert test.asm to a C include.
#
code = open("test.bin","rb").read(2048)
code = [str(ord(x)) for x in code]
open("__testcode.h","w").write("static ROMTYPE romMemory[] = { "+",".join(code)+"};\n")
