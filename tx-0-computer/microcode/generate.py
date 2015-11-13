#
#	Generate Microcode. This program takes the tx-0 microcode definition, analyses it, sorts it
#	by time and sequence in the file, and builds a .H file that can be included that implements it.
#
import re

def strip(s):
	if s.find("//") >= 0:
		s = s[:s.find("//")]
	s = s.strip()
	if s != "" and s[-1] == "\\":
		s = s[:-1].strip()+"\\"
	return s.replace("\t"," ")

gHeader = "/*** Generated code do not edit directly ***/\n\n"				# header for generated code.
source = open("microcode.def").readlines()									# read source
source = [strip(s) for s in source]											# remove comments, compress multilines.
source = "\n".join(source).replace("\\\n","").split("\n")					# join lines, remove slash followed by newline, resplit.
source = [s for s in source if s != ""]										# remove blank lines.


ccode = [s[1:] for s in source if s[0] == ':']								# get C-Code
open("__tx0_support.h","w").write(gHeader+"\n".join(ccode))					# and dump it.

source = [s for s in source if s[0] != ':']									# remove c-code from source

combos = [s[1:].replace(" ","") for s in source if s[0] == '@']				# extract the operation codes
source = [s for s in source if s[0] != '@']									# remove combos from the source.

operations = {}
sortedList = []
known = {}

index = 0

for l in source:															# work through each line.
	#print(l)
	m = re.match("^([0-9\,]*)/([10]*)\s*([0-9\.]+)\s*\"(.*?)\"\s*(.*)$",l)
	assert m is not None
	newOp = { "mnemonic":m.group(4).strip(),"code":m.group(5).strip() }		# build a new operation
	newOp["time"] = float(m.group(3))										# time in TX-0 Cycle.
	newOp["sortkey"] = int(newOp["time"] * 100000) + index 					# create a sorting key, time then file order.
	newOp["mask"] = 0
	newOp["test"] = 0

	if m.group(1) != "":													# if some mask (e.g. not mandatory)
		bitList = m.group(2)
		for bits in [int(x) for x in m.group(1).split(",")]:				# go through the bits
			newOp["mask"] |= (0o400000 >> bits)								# create mask.
			if bitList[0] == '1':											# see if in the test.
				newOp["test"] |= (0o400000 >> bits)
			bitList = bitList[1:]
	newOp["opcode"] = newOp["test"] | 0o600000
	index += 1
	operations[newOp["mnemonic"]] = newOp 									# put in dictionary.
	if newOp["mnemonic"] != "":
		known[newOp["mnemonic"].lower()] = newOp["opcode"]					# construct name => operand command dictionary.

	sortedList.append(newOp)												# add to list

sortedList.sort( key=lambda operation: operation["sortkey"])				# sort on sort key.

#
#	Generate code
#
handle = open("__tx0_operations.h","w")										# write operations file.
handle.write(gHeader)

for op in sortedList:														# work through all operations.
	m = op["mnemonic"] if op["mnemonic"] != "" else "(Mandatory)"			# get description then comment
	handle.write("/*** {0} at time {1} code = {2:06o} ***/\n\n".format(m,op["time"],op["opcode"]))
	if op["mask"] != 0:														# condition if there is one
		handle.write("if ((Instruction & 0{0:06o}) == 0{1:06o}) {{\n".format(op["mask"],op["test"]))

	c = "\n".join(["    "+s+";" for s in op["code"].split(";") if s != ""])	# output body
	handle.write(c+"\n")

	if op["mask"] != 0:														# closing curly if there is one.
		handle.write("}\n\n")

for c in combos:															# work through combinations
	#print(c)
	m = re.match("^([A-Z0-9]+)\=(.*)$",c)									# check syntax
	assert m is not None

	operation = 0o600000													# base operation
	for parts in m.group(2).split("+"):										# examine components
		assert parts.lower() in known
		operation |= known[parts.lower()]									# build up operation code.

	known[m.group(1).lower()] = operation 									# save in list

# known consists of mnemonic -> operation code for all stand alone and combo operations.

known["sto"] = 0o000000
known["add"] = 0o200000
known["trn"] = 0o400000
known["opr"] = 0o600000

pyList = ";".join(k.lower()+"="+str(known[k]) for k in known.keys())		# create list of python
open("python_insert.txt","w").write(pyList+'\n')

pyList = (pyList+";;;").replace(";","\\0")
open("__tx0_python_opr.h","w").write("#define OPERATOR_LIST \""+pyList+"\"\n")

#
#	Golux's known list.
#
goluxList = "sto,add,trn,opr,cll,clr,cla,clc,cal,lr0,com,lac,alr,lpd,lad,shr,cyr,cyl,tac,tbr,dis,ios,r1c,r3c,r1r,r1l,prt,pnt,pna,pnc,p6s,p6h,p7h,p6a,p7a"

golux = {}
for g in goluxList.split(","):
	golux[g] = known[g]
goList = ";".join(k.lower()+"="+str(known[k]) for k in golux.keys())
open("..\\golux\\golux_insert.txt","w").write(goList+"\n")
print(goList)