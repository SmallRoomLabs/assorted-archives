#
#	Convert test.bin to __image.h
#
src = open("test.bin","rb").read(-1)
src = [str(ord(x)) for x in src]
src = ",".join(src)
open("__image.h","w").write(src)