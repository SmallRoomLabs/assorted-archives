#
#	Python : Apple Basic -> PNG Converter.
# 
import re
from PIL import Image,ImageDraw

class Converter:
	def __init__(self,sourceFile):
		code = open(sourceFile).readlines()																	# read the code in.
		code = [self.removeLine(c.strip()) for c in code if c[:2] != "//"]									# remove comments and strip spaces, line nos.
		code = "\n".join(code).lower()																		# make one long string, lower case
		code = code.replace("\n",":").replace("::",":")														# make it one long string
		code = code.replace("hplot to","to")																# make hplot to into just to, continuation.
		code = code.replace("to",";")																		# colon seperate coordinate pairs
		code = code.replace("x.m","0").replace("y.m","0")													# replace x.m, y.m with 0,0
		code = code.replace(" ","").replace("/distance","").replace("/di","")								# remove spaces, replace /di with empty
		code = code.replace("-dist.rect.y(distance)","-178")
		code = code.replace("-dist.rect.y(di)","-178")
		code = code.replace(":","")
		code = [c.strip() for c in code.split("hplot") if c.strip() != ""]									# split around hplot, gives sequence of coordinates

		self.drawList = code 																				# save it
		self.targetFile = sourceFile[:-3]+"png"																# save the target file name.

	def removeLine(self,txt):
		while txt[0] >= '0' and txt[0] <= '9':
			txt = txt[2:]
		return txt.strip()

	def render(self):
		image = Image.new("RGBA",(160*2,120*2),(255,255,255,0))
		imageDraw = ImageDraw.Draw(image)
		for listItem in self.drawList:
			coords = listItem.split(";")	
			isFirst = True
			for c in coords:
				points = c.split(",")
				x1 = self.evaluate(points[0])+80
				y1 = self.evaluate(points[1])+100
				if not isFirst:
					imageDraw.line((x1*2,y1*2,x2*2,y2*2),fill = (0,0,0,255),width = 3)
				x2 = x1
				y2 = y1
				isFirst = False
		image.save(self.targetFile)
		del image

	def evaluate(self,expr):
		expr = expr.replace("-","+-")
		expr = [float(x) for x in expr.split("+")]
		return sum(expr)


Converter("balrog.src").render()
Converter("carrion.src").render()
Converter("daemon.src").render()
Converter("gremlin.src").render()
Converter("orc.src").render()
Converter("rat.src").render()
Converter("skeleton.src").render()
Converter("thief.src").render()
Converter("viper.src").render()

# Mimic, draw using chest code.