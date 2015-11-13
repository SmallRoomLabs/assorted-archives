#  ****************************************************************************************************************
#  ****************************************************************************************************************
# 
# 		File:		ttf2png.py 
# 		Purpose:	Convert a TTF to a bitmap font and associated subimages.txt file.
# 		Author:		Paul Robson
# 		Created:	30-Dec-14
# 
#  ****************************************************************************************************************
#  ****************************************************************************************************************

from PIL import Image,ImageFont, ImageDraw,ImageOps
import math,re,sys

#  ****************************************************************************************************************
#
#	Class representing a font ; can get information on and render characters onto a Pillow Draw Surface
# 	Also provides mapping facility so chr(1)-chr(31) can be used for accented or other characters.
#	
#  ****************************************************************************************************************

class FontSource:
	def __init__(self,font,mapping = None,workingSize=64):
		self.font = None
		self.fontName = font 														# the font being used
		self.extendedMapping = mapping or {}										# mapping non ASCII -> other fonts.
		self.workingSize = workingSize 												# base size being generated (scaled obviously)
		if font[-4:] != ".ttf":														# add .ttf if required.
			font = font + ".ttf"
		self.font = ImageFont.truetype(font,workingSize)							# create an instance of the font.
		self.setFontColour() 														# white
		self.setBorder()															# black thin border
		self.dropShadow = 0															# no drop shadow

	def __del__(self):
		if self.font != None:														# if font is not None, delete it
			del self.font

	def map(self,character):														# map character to other characters.
		return self.extendedMapping[character] if character in self.extendedMapping else character

	def getMaxHeight(self,startChar,endChar):
		maxHeight = 0 																# figure out the tallest character
		for i in range(startChar,endChar+1): 										# for AGK all characters will be the same
			maxHeight = max(maxHeight,self.size(self.map(chr(i)))[1])				# height in rendering
		return maxHeight

	def setFontColour(self,colour = (255,255,255,255)):								# set font solid colour.
		self.fontColour = colour
		self.interpolation = None 													# means it is solid.
		return self

	def setBorder(self,colour = (0,0,0,255),size = 4):								# set font border
		self.border = size*self.workingSize / 100									# border size is a percentage
		self.borderColour = colour
		return self

	def setDropShadow(self,colour = (128,128,128,128),size = 10):					# set drop shadow
		self.dropShadow = max(1,size * self.workingSize / 100) 						# drop shadow offset.
		self.dropColour = colour
		return self

	def setHorizontalInterpolation(self,colour1,colour2):							# shortcuts.
		self.setNonSolidColour("H",colour1,colour2)
	def setVerticalInterpolation(self,colour1,colour2):
		self.setNonSolidColour("H",colour1,colour2)
	def setRadialInterpolation(self,colour1,colour2):
		self.setNonSolidColour("H",colour1,colour2)

	def setNonSolidColour(self,interpolation,colour1,colour2):						
		self.interpolation = interpolation.upper()									# save interpolation (R,H,V)
		self.colour1 = colour1 	
		self.colour2 = colour2 

	def size(self,character):	
		size = self.font.getsize(self.map(character))								# get the character size
		width = size[0] + self.border * 2 											# allow for border
		height = size[1] + self.border * 2
		if character != 32:															# for non-space character
			width = width + self.dropShadow 										# allow for drop shadow.
			height = height + self.dropShadow
		return (int(width+0.7),int(height+0.7))

	def render(self,image,draw,position,character):
		size = self.size(character)													# get the character size
		character = self.map(character)												# map character to actual character
		if self.dropShadow > 0:														# Draw drop shadow
			draw.text((position[0]+self.border*2+self.dropShadow,position[1]+self.border*2+self.dropShadow), character,font = self.font, fill = self.dropColour)
		if self.border > 0:															# Cheat to draw border
			for i in range(0,46):
				angle = math.radians(i*360/45)
				draw.text((position[0]+self.border+self.border*math.cos(angle),position[1]+self.border+self.border*math.sin(angle)),character,font = self.font, fill = self.borderColour)

		fillColour = self.fontColour 												# what we are going to fill it in.				
		if self.interpolation is not None:											# if interpolating, create a dummy colour.
			fillColour = (255,128,0,128)											# which can't be there.		

																					# Draw the character itself
		draw.text((position[0]+self.border,position[1]+self.border),character,font = self.font, fill = fillColour)

		if self.interpolation is not None:											# interpolate ?
			self.doInterpolation(image.load(),position[0],position[1],size[0],size[1])

		if False:																	# Debugging rectangle (optional obviously!)
			draw.rectangle((position[0],position[1],position[0]+size[0]-1,position[1]+size[1]-1),outline = (255,0,0,255))

	def doInterpolation(self,pixels,xStart,yStart,width,height):
		for x in range(0,width):													# scan this.
			for y in range(0,height):
				pixColour = pixels[x+xStart,y+yStart]								# read the pixel.
				if pixColour[3] >= 96 and pixColour[3] <= 160:						# this is required because of antialiasing ?
					pixels[x+xStart,y+yStart] = self.calculateInterpolation(x,y,width,height)

	def calculateInterpolation(self,x,y,width,height):
		interp = 0.0																# the actual interpolation value.

		if self.interpolation[0] == "H":											# Horizontal, left to right.
			interp = x / width
		if self.interpolation[0] == "V":											# Vertical, top to bottom.
			interp = y / height
		if self.interpolation[0] == "R":											# Radial, middle out.
			dx = 2*(x - width / 2)/width											# get dx,dy in range -1..0..1
			dy = 2*(y - height / 2)/height
			interp = min(1.0,math.sqrt(dx*dx+dy*dy)*0.9)							# calculate interpolation

		retVal = [0,0,0,0]															# return value, default
		for i in range(0,4):														# calculate them, linear
			retVal[i] = int((self.colour2[i]-self.colour1[i]) * interp + self.colour1[i]+0.5)
		return tuple(retVal)														# convert to tuple and return.

#  ****************************************************************************************************************
#	
#		Class represents an image suitable for export to AGK2
#
#  ****************************************************************************************************************

class FontImage:
	def __init__(self,fontSource,width = 512): 
		self.LOW_CHAR = 32															# font range supported
		self.HIGH_CHAR = 127
		self.fontSource = fontSource 												# save font source
		self.fontMaxHeight = fontSource.getMaxHeight(self.LOW_CHAR,self.HIGH_CHAR+1)# save max pixel height of font
		self.width = width 															# save width
		self.positions = self.calculatePositions()									# figure out where the characters go
		self.height = self.calculateHeight() 										# work out the image height needed
		self.image = Image.new("RGBA",(self.width,self.height),(255,255,255,0))		# create blank image for characters
		self.draw = ImageDraw.Draw(self.image)										# enable drawing to them
		self.render()																# render the characters

	def __del__(self):
		del self.draw 																# remove draw object
		del self.image 																# remove image object

	def calculatePositions(self):
		positions = {}																# mapping of char code => (x,y)
		x = 0																		# start top left
		y = 0
		for i in range(self.LOW_CHAR,self.HIGH_CHAR+1):								# work through font characters
			size = self.fontSource.size(chr(i))										# get the size of the char that goes there.
			if x + size[0] >= self.width - 2:										# if it doesn't fit on this line.
				x = 0																# go to the next line.
				y = y + self.fontMaxHeight
			positions[i] = (x,y,size[0],size[1])									# save position
			x = x + size[0]+2 														# allocate space for it.
		return positions 															# return positions hash

	def calculateHeight(self):
		maxY = 0																	# lowest character (probably 127 !)
		for i in range(self.LOW_CHAR,self.HIGH_CHAR+1):								# scan through finding it
			maxY = max(maxY,self.positions[i][1])
		return maxY+self.fontMaxHeight+2 											# add space for the actual character

	def render(self):	
		for i in range(self.LOW_CHAR,self.HIGH_CHAR):								# work through font
			self.fontSource.render(self.image,self.draw,self.positions[i],chr(i))	# rendering each character

	def write(self,pngFile):
		txtFile = pngFile[:-4]+" subimages.txt"										# file name for subimages.txt
		self.image.save(pngFile)													# save the png file.
		hText = open(txtFile,"w")													# open the text file
		for i in range(self.LOW_CHAR,self.HIGH_CHAR+1):								# write out each piece of position data
			hText.write(str(i)+":"+(":".join(str(n) for n in self.positions[i]))+"\n")

#  ****************************************************************************************************************
#
#	Create image from hash. The hash entries are
#
#	font 		name of font, can have .ttf
#	fontsize 	size of font you want the image to have - higher size, more accurate, more GPU Ram
#	outputname	where it will end up (defaults to <font>.png)
#	colour 		solid colour : ( (r,g,b,a) )
# 	border 		border if needed : ( (r,g,b,a), border percent )
# 	dropshadow 	dropshadow if needed : ( (r,g,b,a), dropshadow percent offset )
#	radial* 	radial colouring : ( (r,g,b,a),(r,g,b,a) ) [* also horizontal, vertical colouring]
#
#  ****************************************************************************************************************

def createImageFromHash(descriptor,mapping = {}):
	defaults = { "font":"arial","colour":(255,255,255,255),"renderwidth":512,"fontsize":48 }
	for k in defaults.keys():														# copy in defaults if not found.
		if k not in descriptor:
			descriptor[k] = defaults[k]
	if "outputname" not in descriptor:												# if no output name use font name.
		descriptor["outputname"] = descriptor["font"]

	fs = FontSource(descriptor["font"],mapping,descriptor["fontsize"])				# create font creator.
	fs.setFontColour(descriptor["colour"])											# set solid font
	fs.setBorder((0,0,0,0),0)														# no border.
	if "border" in descriptor and descriptor["border"][1] > 0:						# is there a border ?
		fs.setBorder(descriptor["border"][0],descriptor["border"][1])				# then set it.
	if "dropshadow" in descriptor:													# is there a drop shadow ?
		fs.setDropShadow(descriptor["dropshadow"][0],descriptor["dropshadow"][1])	# then set it.

	for m in ["radial","horizontal","vertical"]: 									# look for horizontal, vertical, radial colours.
		if m in descriptor:															# if found
			fs.setNonSolidColour(m[0],descriptor[m][0],descriptor[m][1])			# use it.

	fi = FontImage(fs,defaults["renderwidth"])										# create font imager.
	#fi.image.show()

	writeFile = descriptor["outputname"]											# this is where its written to.
	if writeFile[-4:] == ".ttf":													# remove .ttf, if any.
		writeFile = writeFile[:-4]
	writeFile = writeFile.lower() 													# always make filenames lower case
	fi.write(writeFile+".png")														# and write it out.
	del fs 																			# tidy up.
	del fi
	return "Generated '"+writeFile+".png' and '"+writeFile+" subimages.txt' from font '"+descriptor["font"]+"'"
														# print it.
#  ****************************************************************************************************************
#	
#		The command line version, which takes a string describing the font; it's meant to be a bit like CSS.
#	
#  ****************************************************************************************************************
# 	The description string is items, seperated by semicolons. The items are (default in brackets)
#
# 	font:name 					(None)
# 	border:#xxxxxx 				(None)
# 	colour:#xxxxxx 				(Solid,White)
# 	radial:#xxxxxx,#xxxxxx 	
# 	horizontal:#xxxxxx,#xxxxxx
# 	vertical:#xxxxxx,#xxxxxx 
# 	dropshadow:#xxxxxx,nn 		(No drop shadow)
# 	renderwidth:nn 				(512)
# 	fontsize:nn					(48)
# 	outputname:name				(None, set when font set to name w/o ttf)
#
#	These are almost equivalent to the createImageFromHash() function, except that this supports both #xxxxxx (RGB) colours
# 	and text name colours. To be nice I will allow the incorrect spelling of the word "colour"
#
class CLIException(Exception):
	pass

class CommandLineInterpreter:
	def __init__(self):
		defColours = ["white","#ffffff","silver","#c0c0c0","gray","#808080","black","#000000","red","#ff0000","maroon","#800000","yellow","#ffff00","olive","#808000","lime","#00ff00","green","#008000","aqua","#00ffff","teal","#008080","blue","#0000ff","navy","#000080","fuchsia","#ff00ff","purple","#800080"]
		self.standardColours = {}													# list of standard colours.
		for i in range(0,int((len(defColours)+1)/2)):								# copy into the standard colours dictionary
			self.standardColours[defColours[i*2].lower()] = defColours[i*2+1]
		self.standardColours["grey"] = self.standardColours["gray"]					# trying to be helpful here .....

																					# now build syntax descriptors.
		self.syntax = { "font":"$","border":"#0","colour":"#","dropshadow":"#0","renderwidth":"0","fontsize":"0","outputname":"$" }
		for m in ["radial","horizontal","vertical"]: 								# add radial, horizontal,vertical
			self.syntax[m] = "##"

	def process(self,command):
		command = command.lower().strip() 											# all lower case and strip spaces.
		parts = [x.strip() for x in command.split(";")]								# split into bits.
		self.definition = { }														# this is the descriptor.
		for command in parts:														# do all the sub parts.
			self.processCommand(command)
		#print(self.definition)
		return createImageFromHash(self.definition)									# create font and exit with message.

	def processCommand(self,command):	
		rem = re.match("^\s*([a-z]+)\s*\:\s*(.*)$",command)							# should be name:<something>
		if rem is None:																# didn't match.
			raise CLIException("Command '"+command+"' is syntactically incorrect.")
		defCmd = "colour" if rem.group(1) == "color" else rem.group(1)				# some nationalities can't spell colour.
		if defCmd not in self.syntax:												# check it is a known descriptor.
			raise CLIException("Descriptor '"+defCmd+"' is unknown.")
		params = [x.strip() for x in rem.group(2).split(",")]						# split and strip the parameters.
		if len(params) != len(self.syntax[defCmd]):									# check parameter count matches.
			raise CLIException("Descriptor '"+defCmd+"' has "+str(len(self.syntax[defCmd]))+" parameters.")
		self.parameters = [] 														# parameter list.
		for i in range(0,len(params)):												# process each parameter.
			self.parameters.append(self.processParameter(self.syntax[defCmd][i],params[i]))
		self.definition[defCmd] = tuple(self.parameters) if len(self.parameters) > 1 else self.parameters[0]

	def processParameter(self,paramType,parameter):
		retVal = None
		if paramType == '$':														# expects a string.
			retVal = parameter 														# just return the parameter.

		if paramType == '0':														# expects a number
			rem = re.match("^[0-9]+$",parameter)									# check it is one.
			if rem is None:
				raise CLIException("Parameter '"+parameter+"' is not numeric.")
			retVal = int(parameter) 												# return its integer value.

		if paramType == '#':														# expects a colour.
			if parameter in self.standardColours:									# convert text colours to RGB
				parameter = self.standardColours[parameter]							# if it is in the standard colours dictionary.
			rem = re.match("^#[0-9a-f]+$",parameter)								# check it is legitimate.
			if rem is None or len(parameter) != 7:									# bad parameter format.
				raise CLIException("Colour '"+parameter+"' is invalid, should be #RRGGBB.")
			colours = []															# convert #RRGGBB to colours.
			for i in range(0,3):
				colours.append(int(parameter[i*2+1:i*2+3],16))
			colours.append(255)														# add A which is always 255.
			retVal = tuple(colours)

		if retVal is None:															# shouldn't happen !
			raise CLIException("Parameter '"+parameter+"' failed internally.")
		return retVal

if len(sys.argv) > 1:																# is there a parameter ?
	cli = CommandLineInterpreter()													# create an instance to process it.
	for i in sys.argv[1:]:															# for each parameter
		msg = cli.process(i)														# process it and print result
		print(msg)


#  ****************************************************************************************************************
# 		Date		Changes Made
#		----		------------
#		23 Nov 14 	First working version.
#		30 Jan 15 	Built a working version with interpolation which outputs .png and subimages.txt files.
# 
#  ****************************************************************************************************************