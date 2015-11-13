# *****************************************************************************************************************************
# *****************************************************************************************************************************
#
#	Name: 		agkdoc.py
#	Purpose:	JavaDoc for AppGameKit2
#	Author:		Paul Robson (paulscottrobson@gmail.com)
#	Created:	11th December 2014
#
# *****************************************************************************************************************************
# *****************************************************************************************************************************

import re,os,time,sys 																			# stock imports.

# *****************************************************************************************************************************
#						The aggregator class collects both comments and things like @param and @return 
# *****************************************************************************************************************************

class Aggregator:
	def __init__(self):
		self.clear()																			# initially it is empty.

	def clear(self):																			
		self.aggregatorEmpty = True 															# initially there is no content.
		self.content = {}																		# content values.
		self.currentEntry = "base"																# initially all comment data goes here.
		self.content[self.currentEntry] = "" 													# which is initially empty.
		self.paramCount = 1 																	# next available parameter number

	def isEmpty(self):
		return self.aggregatorEmpty 

	def append(self,comment):
		if comment == "":																		# null comment.
			return
		m = re.match("@([a-zA-Z]+)(.*)$",comment)												# is it @something ?
		if m is not None:
			self.currentEntry = m.group(1).lower()												# this is the content group.
			if self.currentEntry == "param":													# if param, append param count
				self.currentEntry = self.currentEntry+str(self.paramCount)						# because there are several of them.
				self.paramCount = self.paramCount + 1 											
			if self.currentEntry not in self.content:											# does it need creating.
				self.content[self.currentEntry] = ""											# if so do it.
			comment = m.group(2).strip()														# comment is rest of line.

		comment = (self.content[self.currentEntry]+" "+comment).strip()							# append to current.
		while comment.find("  ") >= 0:															# replace multiple spaces.
			comment = comment.replace("  "," ")
		self.content[self.currentEntry] = comment 												# copy back.
		self.aggregatorEmpty = False 															# now something in the aggregator.

	def get(self,content):
		if content not in self.content:
			return ""
		return self.content[content]

	def find(self,paramName):
		paramName = paramName.lower()+" "														# make l/c and add space.
		for i in range(1,self.paramCount):
			paramText = self.content["param"+str(i)]											# get content
			if paramText.lower()[:len(paramName)] == paramName:									# does it match ?
				return paramText[len(paramName):].strip()										# return the rest.
		return ""

# *****************************************************************************************************************************
#													Encapsulation Superclass
# *****************************************************************************************************************************

class BaseDescriptor:
	def setSourceFile(self,fileName):
		self.sourceFile = fileName

	def render(self,target):
		target.write("<div id=\"title\">"+self.getName()+" : "+self.getObjectName()+"</div>\n")
		target.write("<div id=\"body\"><i> Defined in '"+self.sourceFile+"'</i></div>\n")
		target.write("<div id=\"body\">"+self.comment+"</div>\n")
		target.write("<table>\n")
		headers = self.getHeadings()
		if headers is not None:
			target.write('  <tr>\n')
			for h in headers:
				target.write('    <th>'+h+'</th>\n')
			target.write('  </tr>\n')
		count = 0
		for m in self.memberList:
			count = count + 1
			if count % 2 == 0:
				target.write('  <tr class="alt">\n')
			else:
				target.write("  <tr>\n")
			self.renderTableRow(target,m)
			target.write("  </tr>\n")
		target.write("</table>\n")

	def renderTableRow(self,target,m):
		target.write("    <td>"+m["name"]+"</td>\n")
		target.write("    <td>"+m["type"]+"</td>\n")
		target.write("    <td>"+m["comment"]+"</td>\n")

# *****************************************************************************************************************************
#											Encapsulates the description of a module
# *****************************************************************************************************************************

class ModuleDescriptor(BaseDescriptor):
	def __init__(self,aggregator):
		self.name = aggregator.get("name")
		if self.name == "":
			raise Exception("Module must have a name")
		self.comment = aggregator.get("base")
		self.memberList = []
		self.possibles = "name,author,email,license,prefix,version,created,updated,provides,requires".split(",")
		for p in self.possibles:
			v = aggregator.get(p)
			if v != "":
				m = { "item":p[0].upper()+p[1:].lower(),"value":v }
				self.memberList.append(m)

	def renderTableRow(self,target,m):
		target.write("    <td>"+m["item"]+"</td>\n")
		target.write("    <td>"+m["value"]+"</td>\n")

	def sortKey(self):
		return "0_"

	def getName(self):
		return "Module"

	def getHeadings(self):
		return None

	def getObjectName(self):
		return self.name

# *****************************************************************************************************************************
#											Encapsulates the description of a type
# *****************************************************************************************************************************

class TypeDescriptor(BaseDescriptor):
	def __init__(self,typeName,aggregator,code):
		self.name = typeName 																# save the type name.
		self.comment = aggregator.get("base")												# get the main comment
		self.memberList = []																# list of type members.
		for member in code:																	# work through the code.
			parts = [x.strip() for x in member.split("///")]								# divide into definition and comment
			parts.append("")																# at least two entries
			member = parts[0]																# get the two bits.
			comment = parts[1]
			match = re.match("^(.*)//",member)												# was there an ordinary comment ?
			if match is not None:															# if so, remove it.
				member = match.group(1).strip()	
			if member.find(" as ") < 0:														# is there no type specified ?
				newType = "integer"															# work out which type it is.
				if member[-1] == '#':													
					newType = "float"
				if member[-1] == "$":
					newType = "string"
				member = member + " as "+newType											# so num# becomes num# as float
			member = member.split(" as ")													# split into something as something.
			descriptor = { "name":member[0], "type":member[1], "comment":comment }			# build a data structure.
			self.memberList.append(descriptor)												# add it to the type Members list.

	def sortKey(self):
		return "1_"+self.name

	def getName(self):
		return "User Defined Type"

	def getHeadings(self):
		return ["Member","Type","Description"]

	def getObjectName(self):
		return self.name

# *****************************************************************************************************************************
#											Encapsulates the description of a function.
# *****************************************************************************************************************************

class FunctionDescriptor(BaseDescriptor):
	def __init__(self,functionInfo,returnValue,aggregator,code):
		self.returnValue = returnValue.strip() 												# return value, if any.		
		self.returnComment = aggregator.get("return")										# return comment, if any.
		self.comment = aggregator.get("base")												# get the main comment.

		match = re.match("^([0-9A-Za-z_]+)\\((.*)\\)$",functionInfo)						# extract the function information.
		self.name = match.group(1)															# first bit is the name, second parameters.
		self.memberList = []																# parameter information goes here.
		parameterList = match.group(2).strip()												# list of parameters for this function.
		if parameterList != "":																# are there any for it ?
			for param in parameterList.split(","):											# work through them.
				param = param.strip()														# remove spaces.
				if param.find(" ") < 0:														# is the parameter type specified ?
					if param[-1] == "$":													# no, pad it out in full.
						param = param+" as string"
					elif param[-1] == "#":
						param = param+" as float"
					else: 
						param = param+" as integer"
				match = re.match("^([0-9A-Za-z_]+[\\#\\$]?)\\s+(.*)$",param)				# split into name and type.
				descriptor = { "name":match.group(1),"type":match.group(2).strip() }		# make a descriptor.
				descriptor["type"] = descriptor["type"].replace("as ","")					# remove the x as y bit.
				descriptor["comment"] = aggregator.find(descriptor["name"])					# find a comment descriptor.
				self.memberList.append(descriptor)											# and add it in.
		returnDescriptor = { "name":"<i>returns</i>","comment":self.returnComment }			# add the return value
		returnDescriptor["type"] = self.identifyReturnType(self.returnValue,code)			# get the return type
		self.memberList.append(returnDescriptor)

	def identifyReturnType(self,returnValue,code):
		retType = "integer"																	# basic method using the postfix type.
		if returnValue == "":																# no return value
			retType = "none"																# type is none
			returnValue = " "																# stop [-1] crashing.
		if returnValue[-1] == '$':
			retType = "string"
		if returnValue[-1] == '#':
			retType = "float"
		for l in code:																		# look for it in the code being defined
			match = re.match(returnValue+"\\s+as\\s+([a-zA-z0-9\\[\\]]+)",l)				# e.g. xxxx  as [something]
			if match is not None:
				retType = match.group(1)													# if found update return typ
		return retType

	def sortKey(self):
		return "2_"+self.name

	def getName(self):
		return "Function"

	def getHeadings(self):
		return ["Parameter","Type","Description"]

	def getObjectName(self):
		return self.name+"("+",".join([x["name"] for x in self.memberList if x["name"] != "<i>returns</i>"])+")"

# *****************************************************************************************************************************
#										Source encapsulates and processes a source file.
# *****************************************************************************************************************************

class Source:
	def __init__(self,aggregator):
		self.aggregator = aggregator 															# save the aggregator file.

	def process(self,sourceName):
		self.aggregator.clear()																	# clear residual comment data.
		self.source = open(sourceName).readlines()												# read in the code.
		self.objectList = []																	# return list of types/functions
		self.source = [x.replace("\t"," ").strip() for x in self.source]						# preprocess it.
		self.source = [x for x in self.source if x != ""]										# remove blank lines.
		for i in range(0,len(self.source)):														# work through the source.
			line = self.source[i]
			if line[:3] == "///":																# is it a document comment ?
				self.aggregator.append(line[3:].strip())										# add it to the aggregate comments.
			if not self.aggregator.isEmpty():													# is there something in the aggregator ?

				if re.match("///\\s*@module",line) is not None:
					self.objectList.append(ModuleDescriptor(self.aggregator))					# create a module descriptor.
					self.aggregator.clear()														# comments have been used up.

				if line[:5] == "type ":															# is there a type definition ?
					p = i 																		# extract relevant bit of text.
					while self.source[p][:7] != "endtype":										# from type to endtype
						p = p + 1
					self.objectList.append(TypeDescriptor(line[4:].strip(),self.aggregator,self.source[i+1:p]))
					self.aggregator.clear()														# comments have been used up.

				if line[:9] == "function ":														# is there a function definition
					p = i 																		# find the end function
					while self.source[p][:11] != "endfunction":
						p = p + 1
					self.objectList.append(FunctionDescriptor(line[8:].strip(),self.source[p][11:].strip(),self.aggregator,self.source[i+1:p]))
					self.aggregator.clear()														# comments have been used up.

		self.aggregator.clear() 																# cannot pass on to another file.
		for obj in self.objectList:																# tell all objects where they are from.
			obj.setSourceFile(sourceName)
		self.source = None 																		# bin the source code.
		return self.objectList 																	# return list of known objects.

# *****************************************************************************************************************************
#		Default CSS Code
# *****************************************************************************************************************************

def getDefaultCSS():
	return """
		body {	 font-family:Arial,helvetica, sans-serif; }
		#title { font-size: 1.6em;margin-top:8px;padding-top:4px;padding-bottom:4px;background-color: #EAF2D3; 
		 		 border-top:1px solid black;border-bottom:1px solid black;}
		#body {  color:black;padding: 4px 4px 4px 4px; }
		table {	 border-collapse: collapse;   }
		th {	 text-align: left;padding: 3px 7px 2px 7px;background-color: #A7C942;color: #ffffff; }
		td {     border: 1px solid #98bf21;padding: 3px 7px 2px 7px; }
		tr.alt { color: #000000;background-color: #EAF2D3;}
"""

# *****************************************************************************************************************************
#														Source code collection
# *****************************************************************************************************************************

def getSortKey(object):
	return object.sortKey().lower()

class SourceCollection:
	def __init__(self,codeList):
		self.aggregator = Aggregator()															# create an aggregator for use.
		self.sourceProcessor = Source(self.aggregator)											# and a source code processor.
		self.objectList = []																	# list of known objects.
		for sourceFile in codeList:																# process all source files known.
			print("Examining ... "+sourceFile)
			newObjects = self.sourceProcessor.process(sourceFile)	
			for n in newObjects:																# add new objects to the list.
				self.objectList.append(n)
		self.objectList.sort(key = getSortKey)													# sort the objects.

	def render(self,target):
		target.write("<!DOCTYPE html>\n")														# create HTML header
		target.write("<html>\n")
		target.write("<head>\n")
		target.write('<link rel="stylesheet" href="agkdoc.css">\n')
		target.write("</head>\n")
		target.write("<body>\n")
		for obj in self.objectList:																# render all objects
			obj.render(target)
		target.write("<br /><div><i>Generated by agkdoc.py on "+time.strftime("%c")+"</i></div>\n")
		target.write("</body>\n")																# then render the footer
		target.write("</html>\n")

	def createCSSFile(self,target):
		if not os.path.isfile(target):															# if file does not exist
			open(target,"w").write(getDefaultCSS())												# create it.


rootDir = ["."] if len(sys.argv) == 1 else sys.argv[1:]											# default is a single directory.

for items in rootDir:																			# work through all the items.
	if items[-4:] == ".agc":																	# is it a specific file ?
		sc = SourceCollection([items])															# create that source collection.
		target = items[:-4]+".html"																# and render to this.
	else:
		fileList = []																			# list of files 
		for root,dirs,files in os.walk(items):													# walk the tree
			for f in files:																		# walk the directory
				if f[-4:] == ".agc":															# looking for agc files.
					fileList.append(root + os.sep + f)											# build a list of them.
		sc = SourceCollection(fileList)
		target = items+os.sep+"documentation.html"
		
	print("    Rendering documentation to "+target)												# now render all the documentation.
	n = target.find(os.sep)																		# figure out the CSS target
	cssTarget = "agkdoc.css"
	if n >= 0:
		cssTarget = target[:n]+os.sep+"agkdoc.css"
	print("    Rendering CSS to "+cssTarget)
	sc.render(open(target,"w"))	
	sc.createCSSFile(cssTarget)

