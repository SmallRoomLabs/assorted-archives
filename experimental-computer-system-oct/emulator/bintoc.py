b = open("test.bin","rb").read(-1)
b = [ord(x) for x in b]
open("__binary.h","w").write(",".join([str(x) for x in b]))
open("__binary_size.h","w").write("#define BINARY_SIZE {0}\n".format(len(b)))
