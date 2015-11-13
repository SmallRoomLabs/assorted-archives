################################################################################################################################
#
#								fnt2agk.py by Paul Robson (paulscottrobson@googlemail.com)
#								==========================================================
#
#	This function takes .png/.fnt files produced by programs like bmglyph and littera and generates .png and .txt files 
#	suitable for use with AGK2.
#
#	From https://bitbucket.org/agkdev/fnt2agk - please feel free to submit bugs/updates/suggestions here or if you 
#	prefer to me at the given email address. Enhancements please discuss with me first.
#
#	This requires Python (written under 3.4 but should work under 2.7) and the Pillow fork of the Python Imaging Library.
#	Download these from:
#
#	https://www.python.org/
#	https://pillow.readthedocs.org/
#
#	This was written on Windows 8.1 but should work on a Mac or Linux, I just haven't tested it.
#	
################################################################################################################################

from PIL import Image,ImageDraw
import re,sys

#
#	This class contains a description of a single character
#
class CharDescriptor:
	def __init__(self,descriptor):
		self.descriptor = descriptor 												# save the character descriptor.
		self.descriptor["xoffset"] = max(self.descriptor["xoffset"],0)				# we do not support negative offsets
		self.descriptor["yoffset"] = max(self.descriptor["yoffset"],0)				# in AGK there's no kerning etc.

	def width(self):
		return max(self.descriptor["width"] + self.descriptor["xoffset"],self.descriptor["xadvance"])

	def height(self):
		return self.descriptor["height"] + self.descriptor["yoffset"]

	def renderPosition(self,x,y,spaceAtTop):
		self.xRender = x 															# set the rendering position
		self.yRender = y - spaceAtTop 												# adjust the rendering position for blank space.
		self.spaceAtTop = spaceAtTop 												# where we end up.
		self.tgtPos = (x+self.descriptor["xoffset"],y+self.descriptor["yoffset"]-spaceAtTop)	# where the square ends up

	def getYOffset(self):
		return self.descriptor["yoffset"]

	def renderCharacter(self,tgtImage,srcImage,maxHeight):							# render the image.
		x1 = self.descriptor["x"]													# short hand for top left
		y1 = self.descriptor["y"]

		if False: 																	# a basic check for the right rectangle
			draw = ImageDraw.Draw(tgtImage)
			draw.rectangle([self.xRender,self.yRender+self.spaceAtTop,self.xRender+self.width(),self.yRender+maxHeight], fill = "red")
			del draw
																					# box containing the character
		charBox = srcImage.crop((x1,y1,x1+self.descriptor["width"]-1,y1+self.descriptor["height"]-1))		
																					# draw it here	
		tgtImage.paste(charBox,self.tgtPos,charBox)

	def getSubImageData(self,stdHeight):
		return ("{0}:{1}:{2}:{3}:{4}").format(self.descriptor["id"],self.xRender,self.yRender+self.spaceAtTop,self.width(),stdHeight-self.spaceAtTop)

#
#	This class represents a single .FNT/.PNG pairing and contains code to generate the appropriate files for 
#	AGK2. Yes, I know the generation code should be abstracted but it's only a little script.
#
class FontHolder:
	def __init__(self,imageFont,descriptorFile):
		self.imageInstance = Image.open(imageFont) 									# open the image font file.
		self.characterData = {} 													# mapping of ASCII code -> character class
		for line in open(descriptorFile).readlines():								# scan through the descriptor files.
			line = line.strip().lower()												# preprocess the line.
			if line[:5] == "char ":													# found a character line
				self.loadCharacter(line[5:].strip())								# load the character in.
		assert(32 in self.characterData)											# there must be a space.

	def loadCharacter(self,line):
		components = "id,x,y,width,height,xoffset,yoffset,xadvance".split(",")		# bits that make up the regex
		componentList = components 													# keep the list.
		components = [x+"\\s*=\\s*([\\-]*\\d+)\\s*" for x in components]			# make them regex parts
		regEx = "".join(components)													# stick them together
		result = re.match(regEx,line)												# match the line against the .FNT line format
		assert(result is not None)													# check it worked.
		result = result.groups() 													# only interested in the result groups.
		result = [int(n) for n in result]											# convert them to integers.
		descriptor = {} 															# now convert to a python hash
		for n in range(0,len(componentList)):										# using the component list for keys.
			descriptor[componentList[n]] = result[n]
		id = descriptor["id"]														# extract the id
		assert(not id in self.characterData)										# no duplicates, obviously.
		if id >= 32 and id <= 127:													# nothing outside this range allowed.
			self.characterData[id] = CharDescriptor(descriptor)						# create a new character descriptor object.

		#print(id,self.characterData[id].width(),self.characterData[id].height())

	def getFontHeight(self):
		maxHeight = -1 																# gotta be more than this
		for k in self.characterData.keys():											# find the tallest character there is.
			maxHeight = max(maxHeight,self.characterData[k].height())				# we don't use the built in one.
		return maxHeight

	def getSmallestYOffset(self):
		minOffset = 99999															# get the smallest Y offset. We can move
		for k in self.characterData.keys():											# everything down by this.
			minOffset = min(minOffset,self.characterData[k].getYOffset())
		return minOffset

	def formatCharacters(self):
		reqWidth = self.characterData[32].width() * 16								# the width is enough for ten spaces
		width = 16 																	# width is multiple of 16 and power of 2.
		while width < reqWidth:														# make it so ....
			width = width * 2
		maxHeight = self.getFontHeight() 											# get the biggest character, we are not packing madly
		minOffset = self.getSmallestYOffset()										# get space we can lose at the top.

		x = 0																		# these are the positions we put the characters at.
		y = 0
		for cCode in self.characterData.keys():										# now work through, processing them.
			charWidth = self.characterData[cCode].width()
			if charWidth + x >= width:												# does it fit on this line.
				x = 0 																# go to start of next line of characters.
				y = y + maxHeight + 2 - minOffset													
			self.characterData[cCode].renderPosition(x,y,minOffset)					# it will be drawn here.
			x = x + charWidth + 2													# move to next slot.

		height = int((y + maxHeight + 6) / 6) * 6									# make it a large enough multiple of 6
		return (width,height)														# return width and height.

	def renderGraphic(self,size):
		fontImage = Image.new("RGBA",size,(255,255,255,0))							# create image of required size, trans white
		maxHeight = self.getFontHeight()
		for cCode in self.characterData.keys():										# render everything on it.
			self.characterData[cCode].renderCharacter(fontImage,self.imageInstance,self.getFontHeight())
		return fontImage 

	def renderSubImageData(self,subImagefile):										
		handle = open(subImagefile,"w")												# create the subimage file
		stdHeight = self.getFontHeight() 											# rendering everything at the same height
		for cCode in self.characterData.keys():										# work through the characters
			subLine = self.characterData[cCode].getSubImageData(stdHeight)
			handle.write(subLine+"\n")
		handle.close()																# close it.

	def generate(self,pngFile,subImagefile):
		size = self.formatCharacters()												# how big the PNG is and where things go
		image = self.renderGraphic(size)											# create an image 
		image.save(pngFile)															# write the image out.
		del image 																	# dispose of the image
		self.renderSubImageData(subImagefile)										# render the subimage file

#
#	This function converts two named files to two new files
#
def convert(srcPngFile,srcFntFile,tgtPngFile,tgtTextFile):
	print("    {0} & {1} to {2} and {3}".format(srcPngFile,srcFntFile,tgtPngFile,tgtTextFile))
	assert(srcPngFile != tgtPngFile)												# it will overwrite it.
	holder = FontHolder(srcPngFile,srcFntFile)
	holder.generate(tgtPngFile,tgtTextFile)
	del holder

print("*** fnt2agk : bitmap font converter by paulscottrobson@googlemail.com ***")
print()
#
#	This simpler function takes the stubs (see the examples)
#
def simpleConvert(src,tgt):
	convert(src+".png",src+".fnt",tgt+".png",tgt+" subimages.txt")

#
#	If you want this as a script which automatically does the conversion put function calls like
#	the ones below (the two do the same thing, convert a littera fnt type to a demo AGK type)
# 	removing the comments, and just run the program. It does not error if there are no parameters
#

#convert("src\\littera.png","src\\littera.fnt","demo.png","demo subimages.txt")

#simpleConvert("src\\littera","demo")

if len(sys.argv) > 1:
	assert(len(sys.argv) == 3)
	simpleConvert(sys.argv[1],sys.argv[2])

