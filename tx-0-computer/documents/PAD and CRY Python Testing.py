#
#	TX-0 
#
#	AMB + PAD + CRY = Cycle left once.

import random

class TX0:
	def __init__(self,initA):
		self.ac = initA
		self.mbr = 0

	def amb(self):
		self.mbr = self.ac 
		return self

	def pad(self):
		self.ac = self.ac ^ self.mbr
		return self 

	def cry(self):
		self.ac = self.ac ^ self.mbr							# unpick the pad
		self.ac = self.add18(self.ac,self.mbr)					# and add :)
		return self

	def add18(self,n1,n2):
		n1 = n1 + n2											# 18 bit 1's complement addition.
		if n1 > 0o777777:
			n1 = (n1 & 0o777777) + 1
		return n1

	def show(self):
		print("AC:{0:018b} MB:{1:018b}".format(self.ac,self.mbr))

	def toDecimal(self,n):
		return n if n <= 0o377777 else -(n ^ 0o777777)

tries = 1000*1000*10

random.seed(42)
tx0 = TX0(0)
for i in range(1,tries):
	if i % 50000 == 0:
		print("Loop "+str(i))
	start = tx0.ac = random.randint(0,0o777777)					# check AMB/PAD/CRY is CYL
	tx0.amb().pad().cry()
	result = ((start << 1) | (start >> 17)) & 0o777777
	assert result == tx0.ac

	fail = True
	while fail:													# create two numbers that don't overflow.
		tx0.ac = random.randint(0,0o777777)
		tx0.mbr = random.randint(0,0o777777)
		fail = False

		n1 = tx0.toDecimal(tx0.ac)
		n2 = tx0.toDecimal(tx0.mbr)
		fail = abs(n1 + n2) > 0o377777

	n3 = tx0.toDecimal(tx0.add18(tx0.ac,tx0.mbr))				# check add18
	assert n1 + n2 == n3

	tx0.pad().cry()												# check pad/cry
	n3 = tx0.toDecimal(tx0.ac)
	assert n1 + n2 == n3
