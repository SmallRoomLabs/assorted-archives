##########################################################################################################################
#
#													BINAC Arithmetic
#													================
#
#											BINAC is 31 bits 2's complement.
#
#	The adding and subtracting is normal. The multiplication and division are slightly different, but the main
#	difference is that both set the least significant bit to 1 whatever. This has issues with using multiply 
#	and divide for multiple shifts ; the value will always be out by 1.
#
##########################################################################################################################

import random

class BinacNumber:
	#
	#	Initialise the number with a default FP value
	#
	def __init__(self,defaultValue = 0.0):
		BinacNumber.signBit = (1 << 30)										# sign bit
		BinacNumber.maskBits = 0o17777777777								# mask and inverse bit
		self.setFloat(defaultValue)											# set the default value
		assert len("{0:b}".format(self.maskBits)) == 31						# check the mask is 31 bits.
		random.seed(242)													# initialise the randomised value.
	#
	#	Binary Add (Simple add with mask)
	#
	def add(self,number):
		self.binary = (self.binary + number.toInt()) & self.maskBits		# add normally and mask
	#
	#	Binary Subtract (Simple add with 2's complemented value and mask)
	#
	def subtract(self,number):
		subtractor = self.twosComplement(number.toInt()) 					# 2's complement of sub value
		self.binary = (self.binary + subtractor) & self.maskBits			# add to this.
	#
	#	Binary multiply (see Auerbach section 2)
	#
	def multiply(self,number):
		partialProduct = 0 													# result
		multiplicand = number.toInt() 										# multiplicand
		multiplier = self.toInt() 											# multiplier
		sign = 1															# positive
		if (multiplicand & self.signBit) != 0:								# remove signs (multiplicand)
			sign = -sign 													# while tracking the new sign.
			multiplicand = self.twosComplement(multiplicand)
		if (multiplier & self.signBit) != 0:								# remove signs (multiplier)
			sign = -sign
			multiplier = self.twosComplement(multiplier)

		for bit in range(0,30):												# do 30 bits
			if (multiplier & 1) != 0:										# if LSB is set
																			# add multiplicand into partial product
				partialProduct = (partialProduct + multiplicand) & self.maskBits	
			multiplier = multiplier >> 1 									# shift multiplier right (e.g. next bit)
			partialProduct = partialProduct >> 1 							# shift partial product right.

		if sign < 0:														# put sign back if -ve
			partialProduct = self.twosComplement(partialProduct)

		self.binary = partialProduct | 1									# save result, add round off bit.
	#
	#	Binary divide (see Auerbach section 3, Shaw)
	#
	def divide(self,number):
		result = 0															# result
		p = self.signBit 													# value added in.
		n1 = self.toInt()													# get the two numbers
		n2 = number.toInt() 												# divide doesn't need signing

		for i in range(0,30):												# do 30 bits 
			if (n1 & self.signBit) != (n2 & self.signBit):					# are the signs different.
				n1 = (n1 + n1 + n2) & self.maskBits							# n1 = 2n1 + n2
			else:
				n1 = (n1 + n1 + self.twosComplement(n2)) & self.maskBits	# n1 = 2n1 - n2
				result = result | p 										# set bit as subtracted
			p = p >> 1
		result = (result + 0x40000001) & self.maskBits						# fix up and add rounding up
		self.binary = result
	#
	#	Put a random value in the number
	#	
	def randomWord(self):
		n = (random.randint(0,self.maskBits+1) << 8) ^ random.randint(0,self.maskBits+1)
		self.binary = n & self.maskBits
	#
	#	Set to the binary equivalent of the provided float
	#
	def setFloat(self,value):
		self.binary = int(abs(value) * pow(2.0,30))							# convert floating to binary
		if value < 0:														# handle negative numbers.
			self.binary = self.twosComplement(self.binary)
	#
	#	31 bit Two's complement calculation
	#
	def twosComplement(self,n):
		return ((n ^ self.maskBits) + 1) & self.maskBits
	#
	#	Return as integer
	#
	def toInt(self):
		return self.binary
	#
	#	Return as float
	#
	def toFloat(self):
		sign = 1.0
		binary = self.binary
		if (binary & BinacNumber.signBit) != 0:								# if -ve
			sign = -1														# result is negative
			binary = self.twosComplement(binary) 							# 2's complement value

		total = 0.0															# bit total
		addValue = 0.5														# bit value
		for i in range(0,30):												# for each bit, 0 to 29 inclusive.
			if (binary & (1 << (29-i))) != 0:								# if it is set (working MSB down)
				total += addValue											# add the bit value in
			addValue /= 2.0													# halve the bitvalue
		total = total * sign 												# put the sign in.
		return total

##########################################################################################################################

testCount = 1000 * 1000 * 10
delta = 2.0e-9
																			# These come from the Arithmetic paper.
checkList = [ [ "40000 00000",0.5 ], [ "00001 00000", pow(2,-15)],["00000 00001",pow(2,-30)] ]


convertToFloat = {}															# dictionary octal -> float
convertToOctal = {}															# dictionary float -> octal

for c in checkList:															# build the dictionaries
	octal = int(c[0].replace(" ",""),8)										# value.
	octal2C = (octal ^ 0o17777777777)+1 									# 2's complement of it
	convertToFloat[octal] = c[1]
	convertToOctal[c[1]] = octal
	convertToFloat[octal2C] = -c[1]
	convertToOctal[-c[1]] = octal2C

work = BinacNumber() 														# we muck around with this.

print("Checking binary to float:")
for oc in convertToFloat.keys():
	work.binary = oc 														# set the binary
	print("{0:011o} {1} {2}".format(oc,work.toFloat(),convertToFloat[oc]-work.toFloat()))
	assert convertToFloat[oc] == work.toFloat()

print("Checking float to binary:")
for oc in convertToOctal.keys():											# set via float
	work.setFloat(oc)														
	print("{0:011o} {1} {2}".format(work.binary,work.toFloat(),work.toFloat()-oc))
	assert work.toFloat() == oc

print("Checking conversions")
for n in range(1,testCount):
	work.randomWord()														# create a random word
	init = work.binary														# remember it
	f = work.toFloat()														# convert to float
	work.setFloat(f)														# set as float
	assert(work.binary == init)												# check converted okay.

n1 = BinacNumber()
n2 = BinacNumber()

print("Checking addition")
for n in range(1,testCount):
	ok = False																# pick two numbers so their sum is < 1
	while not ok:
		n1.randomWord()
		n2.randomWord()
		f1 = n1.toFloat()
		f2 = n2.toFloat()
		ok = abs(f1 + f2) < 1
	n1.add(n2)																# do the addition
	assert n1.toFloat() == f1+f2

print("Checking subtraction")
for n in range(1,testCount):
	ok = False																# pick two numbers so their difference is < 1
	while not ok:
		n1.randomWord()
		n2.randomWord()
		f1 = n1.toFloat()
		f2 = n2.toFloat()
		ok = abs(f1 - f2) < 1
	n1.subtract(n2)															# do the subtraction
	assert n1.toFloat() == f1-f2

print("Checking multiplication")
worstError = 0
for n in range(1,testCount):
	n1.randomWord() 														# create the sum (all are legal)
	n2.randomWord()
	f1 = n1.toFloat()
	f2 = n2.toFloat()
	n1.multiply(n2)															# do the multiply
	diff = abs(n1.toFloat()- f1 * f2) 										# check difference is reasonable
	assert diff < delta 													# Binac 32 bit, Python 64 bit ?
	worstError = max(worstError,diff)
print("  Worst error ",worstError)

print("Checking division")
worstError = 0
for n in range(1,testCount):
	ok = False																# pick two numbers so their division result is < 1
	while not ok:
		n1.randomWord()
		n2.randomWord()
		f1 = n1.toFloat()
		f2 = n2.toFloat()
		ok = f2 != 0.0 and abs(f1/f2) < 1.0
	n1.divide(n2)															# do the division
	diff = abs(n1.toFloat()-f1/f2) 											# difference is reasonable ?
	assert diff < delta
	worstError = max(worstError,diff)
print("  Worst error ",worstError)
