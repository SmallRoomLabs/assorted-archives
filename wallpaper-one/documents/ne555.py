#
#		Astable circuit calculations.
#
# 	The frequency is linear between 0.9vcc and 0.1vcc, increasing as the voltage decreases.
#
#	So fOut = (1-fIn) * f ; if f = 0 then sound is off.
#
#	

#	Standard Astable.
#
#	R1 = 2k2
#	R2 = 56k
#	C1 = 0.01uf (10nf)
#
R1 = 2200
R2 = 56000
C1 = 0.01 / 1000000.0

f555 = 1.44/(R1+2*R2)/C1
print(f555)

RF0 = 1500
RF1 = 820
RF2 = 470

for f in range(1,7):
	totalTop = 0
	totalBottom = 0
	if (f & 1) != 0:
		totalTop = totalTop + 1.0 / RF0
	else:
		totalBottom = totalBottom + 1.0 / RF0
	if (f & 2) != 0:
		totalTop = totalTop + 1.0 / RF1
	else:
		totalBottom = totalBottom + 1.0 / RF1
	if (f & 4) != 0:
		totalTop = totalTop + 1.0 / RF2
	else:
		totalBottom = totalBottom + 1.0 / RF2

	rTop = 1/totalTop
	rBottom = 1/totalBottom
	mult = rTop / (rTop + rBottom)
	volt = 5 * mult
	frequency = int(f555 * (1 - mult))

	print("Flags:{0:x} Voltage Percent:{1} Frequency: {2} Hz".format(f,int(mult * 100),frequency))
