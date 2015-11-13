#
#	Convert test.bin to something includable.
#

data = open("test.bin","rb").read(-1)
data = [str(ord(x)) for x in data]

open("__binary.h","w").write(",".join(data))
