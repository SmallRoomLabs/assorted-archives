def convert(inFile,outFile):
	src = open(inFile,"rb").read(-1)
	src = [str(ord(x)) for x in src]
	open(outFile,"w").write(",".join(src)+"\n")

convert("chip8.rom","chip8_rom.h")
convert("monitor.rom","monitor_rom.h")

