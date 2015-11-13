import os,sys,random
import xml.etree.ElementTree as XML
from PIL import Image,ImageDraw
#
#		RLE Compress/Uncompress class. Decided on this to save space, most maps contain areas of horizontally
#		repeated tiles.
#
class RLECompress:
	#	
	#	RLE Compress numeric array. As all numbers are +ve uses -ve to signifiy repeat count
	#
	def compress(self,data):
		result = []															# final array.
		n = 0 																# pointer into that.
		while n < len(data):												# until compressed everything.
			if n+2 < len(data) and data[n] == data[n+1] and data[n+1] == data[n+2]:
				count = 0													# number of values that repeat
				common = data[n]											# which repeats
				while n < len(data) and common == data[n]:					# keep going while more repeating
					count = count + 1
					n = n + 1
				result.append(-count)										# append -count indicating repeat
				result.append(common)										# append thing that's repeating
			else:
				result.append(data[n])										# normal, no repeat.
				n = n + 1
		return result
	#
	#	RLE Expand numeric array.
	#
	def expand(self,data):
		result = []															# result array
		n = 0																
		while n < len(data):												# work through compressed data
			if data[n] < 0:													# expand RLE compression
				for i in range(0,-data[n]):
					result.append(data[n+1])
				n = n + 2
			else:															# non compressed.
				result.append(data[n])
				n = n + 1
		return result
	#
	#	RLE Simple test.
	#
	def test(self,count=1000):
		random.seed(42)
		totCompress = 0
		totExpand = 0
		for p in range(0,count):											# However many passes....
			if p % 100 == 0:
				print("Pass "+str(p))
			tilesUsed = []													# create list of tiles used on map
			for i in range(0,14):												
				tilesUsed.append(random.randint(1,512))
			source = [ 0 ] * 1000											# blank array of tiles.
			for i in range(1,int(len(source)*2/3)):							# fill with tiles sparsely.
				source[random.randint(1,len(source)-1)] = tilesUsed[random.randint(1,len(tilesUsed)-1)]
			compress = self.compress(source)								# compress the tiles.
			expand = self.expand(compress)									# and expand.
			assert(source == expand)
			totCompress += len(compress) 									# add to totals
			totExpand += len(expand)
		print(totCompress/totExpand * 100)									# print overall compression.
		return source
	#
	#	Generate code to create array as string. Calls a function _TILExpand() which expands the compressed
	#	layer data into the data structures. Note that uncompressed data works as well.
	#	
	def generate(self,data,isCompressed,tgt):
		p = 0																# position in data
		code = ""															# return code
		while p < len(data):												# keep going till finished.
			count = min(len(data)-p,32)										# work out how long the data is.
			if data[p+count-1] < 0:											# stop repeat on a boundary.
				count = count + 1
			section = [str(x) for x in data[p:p+count]]						# convert section to text string
			assert(len(section) == count)									# check sizes are right !
			code = code + '_TILExpand('+tgt+","+str(isCompressed)+',"' + ",".join(section)+'")\n'	# generate code
			p = p + count													# skip over that many entries
		return code
		
#
#	Represents a tileset object.
#
class TileSet:
	def __init__(self,xmlRoot,tileMap):
		attrib = xmlRoot.attrib 											# get attributes list.
		assert(int(attrib["tilewidth"]) == tileMap.tileWidth)				# tile sizes must match
		assert(int(attrib["tileheight"]) == tileMap.tileHeight)
		self.tileWidth = tileMap.tileWidth 									# copy tile sizes.
		self.tileHeight = tileMap.tileHeight
		self.firstID = int(attrib["firstgid"])								# first ID of graphic.
		self.fileName = tileMap.directory + os.sep + attrib["name"] +".png"	# actual file name
		self.isLoaded = False 												# tile set is not loaded.

	def renderTile(self,image,tileID,x,y):
		if not self.isLoaded:												# load graphic if not already done so
			self.loadGraphic()
		baseID = tileID & 0x3FFFFFFF										# remove flipping bits
		if baseID >= self.firstID and baseID < self.firstID+self.imageCount:# tile in this set ?
			n = baseID - self.firstID 										# tile number within this set.
			xs = int(n % (self.width/self.tileWidth)) * self.tileWidth 		# where is the tile on the png ?
			ys = int(n / (self.width/self.tileWidth)) * self.tileHeight
																			# Crop it out.
			tileGfx = self.image.crop([xs,ys,xs+self.tileWidth,ys+self.tileHeight])
			if tileID & 0x40000000 != 0:									# Apply flips according to bits 31,32
				tileGfx = tileGfx.transpose(Image.FLIP_LEFT_RIGHT)
			if tileID & 0x80000000 != 0:
				tileGfx = tileGfx.transpose(Image.FLIP_TOP_BOTTOM)
			image.paste(tileGfx,(x,y))										# paste onto atlas png
			del tileGfx														# throw away used graphic.

	def loadGraphic(self):
		self.image = Image.open(self.fileName)
		self.width = self.image.size[0]
		self.height = self.image.size[1]
		self.imageCount = self.width * self.height / self.tileWidth / self.tileHeight
		self.isLoaded = True
#
#	Represents a layer object
#
class Layer:
	def __init__(self,xmlRoot,tileMap):
		attrib = xmlRoot.attrib
		assert(int(attrib["width"]) == tileMap.mapWidth)					# map sizes should match
		assert(int(attrib["height"]) == tileMap.mapHeight)
		self.mapWidth = tileMap.mapWidth									# copy map width/height in.
		self.mapHeight = tileMap.mapHeight
		self.name = attrib["name"].lower()									# get name, make lower case.
		self.tiles = []					
		for part in xmlRoot:												# work through data
			assert(part.tag == "data")										# should be data
			for tile in part:												# work through data attributes.
				self.tiles.append(int(tile.attrib["gid"]))
		assert(len(self.tiles) == self.mapWidth * self.mapHeight)			# check the counts match.

	def analyseLayer(self,map):
		for tile in self.tiles:												# for each tile
			if str(tile) not in map:										# set count to zero if not used.
				map[str(tile)] = 0
			map[str(tile)] = map[str(tile)] + 1 							# bump the count.
#			
#	Represent tilemap object.
#
class TileMap:
	def __init__(self,xmlName):
		self.compressor = RLECompress() 									# compressor object.
		self.directory = os.path.dirname(xmlName)							# save working directory
		self.baseName = os.path.basename(xmlName)[:-4].lower()				# get base name, remove .tmx
		self.tree = XML.parse(xmlName)										# read in the XML.
		print("Read in "+xmlName)
		self.root = self.tree.getroot()										# access the root.
		assert(self.root.tag == "map")										# which should be a map.
		#assert(self.root.attrib["orientation"] == "orthogonal")				# only support orthogonal at present
		#assert(self.root.attrib["renderorder"] == "right-down")				# and right/down render order.
		self.mapWidth = int(self.root.attrib["width"])						# get raw information.
		self.mapHeight = int(self.root.attrib["height"])
		self.tileWidth = int(self.root.attrib["tilewidth"])						
		self.tileHeight = int(self.root.attrib["tileheight"])
		self.background = self.root.attrib["backgroundcolor"]
		self.tileSets = []													# tile sets
		self.layers = [] 													# layers
		for child in self.root:												# work through the tiles and layers
			if child.tag == "tileset":
				self.tileSets.append(TileSet(child,self))					# add a tile set object.
			if child.tag == "layer":	
				self.layers.append(Layer(child,self))						# add a layer object
		print("Read in "+str(len(self.tileSets))+" tilesets.")				# be informative.
		print("Read in "+str(len(self.layers))+" layers.")
		self.tileUsageCount = { "0":0 } 									# tile number -> usage count.
		for layer in self.layers:											# work through the layers.
			layer.analyseLayer(self.tileUsageCount)							# seeing what goes in each layer.
		print("Layers use "+str(len(self.tileUsageCount.keys()))+" tiles.")
		self.createTileMapping() 											# create tile mapping.
	#
	#	We renumber tiles. Creates two mappings - original -> AGK and AGK -> original
	#
	def createTileMapping(self):
		self.tileIDToagkID = {}												# map tile ID to AGK ID.
		tileList = [int(x) for x in self.tileUsageCount.keys()]				# get list of tiles as numbers
		tileList.sort()														# sort numerically.
		n = 0
		for tile in tileList:												# for each tile firstID
			self.tileIDToagkID[tile] = n
			n = n + 1
		self.agkIDTotileID = tileList
	#
	#	Generate code for layers.
	#
	def generateCode(self,appDirectory):
		codeFileName = appDirectory + os.sep + self.baseName+"_tilecode.agc"# Code file name
		print("Generating code "+codeFileName)
		codeFile = open(codeFileName,"w")									# open code file for writing.
		codeFile.write("// ** Generated by tmxconvert.py. Do not hand edit **\n")
		funcName = "TileSetup_"+self.baseName 								# name of initialisation function
		codeFile.write("function "+funcName+"(map ref as TILMap)\n")		# open function.
		codeFile.write('    map.tileWidth = '+str(self.tileWidth)+"\n")		# Copy sizing information.
		codeFile.write('    map.tileHeight = '+str(self.tileHeight)+"\n")
		codeFile.write('    map.mapWidth = '+str(self.mapWidth)+"\n")
		codeFile.write('    map.mapHeight = '+str(self.mapHeight)+"\n")
		codeFile.write('    map.uniqueTileCount = '+str(len(self.tileIDToagkID))+"\n")
		codeFile.write('    map.tileImages.length = map.uniqueTileCount\n')	# Set size of image array.
		codeFile.write("    map.agkToTileMapID.nextWritePosition = 0\n")	# clear AGK->Tile and write out.
		code = self.compressor.generate(self.agkIDTotileID,0,"map.agkToTileMapID")	
		codeFile.write(self.indent(code,4))									# Indent to fit.
		codeFile.write("    map.layerCount = "+str(len(self.layers))+"\n")	# Set layer count
		codeFile.write("    map.layers.length = map.layerCount\n")		 	# Set array sizes.
		codeFile.write("    map.layerNames.length = map.layerCount\n")
		codeFile.write('    map.tileImage = LoadImage("'+self.baseName+'_tiles.png")\n')
		codeFile.write(' 	i as integer\n')
		codeFile.write(' 	for i = 1 to map.uniqueTileCount\n')
		codeFile.write(' 		map.tileImages[i] = LoadSubImage(map.tileImage,"tile"+str(i))\n')
		codeFile.write('	next i\n')
		for n in range(1,4):												# Set background.
			col = int(self.background[n*2-1:n*2+1],16)
			codeFile.write("    map.background["+str(n)+"] = "+str(col)+"\n")
		uncompressedCount = 0 												# track compression.
		compressedCount = 0
		for n in range(1,len(self.layers)+1):								# For each layer
			name = "map.layers["+str(n)+"]"									# Shorthand
			codeFile.write("    map.layerNames["+str(n)+'] = "'+self.layers[n-1].name+'"\n')
			codeFile.write("    "+name+".nextWritePosition = 0\n")			# Erase the array.
			data = self.layers[n-1].tiles									# Get the layer data
			uncompressedCount = uncompressedCount + len(data)
			data = [self.tileIDToagkID[x] for x in data]					# Map the tile ID to AGK ID
			data = self.compressor.compress(data)							# RLE encode the data.
			compressedCount = compressedCount + len(data)
			code = self.compressor.generate(data,1,name)					# Create code to copy it.
			codeFile.write(self.indent(code,4))								# Indent and write it out.
		codeFile.write("endfunction\n")
		codeFile.close()
		print("RLE Compression "+str(100-int(100*compressedCount/uncompressedCount))+" %")
		return self
	#
	#	Indent code block
	#
	def indent(self,code,indent):
		code = code.split("\n")
		indent = " " * indent
		return "\n".join([indent+c for c in code]).rstrip()+"\n"
	#
	#	Generate atlas.
	#
	def generateAtlas(self,appDirectory):
		width = 256 														# width of atlas graphics.
		tilesPerLine =int(width / self.tileWidth) 							# how many per line.
		lines = int((len(self.agkIDTotileID)+(tilesPerLine-1))/tilesPerLine)# how many tiles up.
																			# Create image.
		image = Image.new("RGBA",(width,lines * self.tileHeight),(255,255,255,0))
		for i in range(0,len(self.agkIDTotileID)):							# work through all tiles.
			tileID = self.agkIDTotileID[i]									# this is the one to render.
			for t in self.tileSets:											# ask each tileset to render it.
				t.renderTile(image,tileID,i % tilesPerLine * self.tileWidth,int(i / tilesPerLine) * self.tileHeight)

		stem = appDirectory + os.sep + "media"+os.sep+self.baseName 		# base name of output
		image.save(stem+"_tiles.png")										# write the .PNG out.
		print("Created "+stem+"_tiles.png tile graphic file.")
		del image
		subText = open(stem+"_tiles subimages.txt","w")						# open subimages file.
		for i in range(0,len(self.agkIDTotileID)):							# and output the positions
			subText.write("tile{0}:{1}:{2}:{3}:{4}\n".format(i, i % tilesPerLine*self.tileWidth,int(i / tilesPerLine) * self.tileHeight,self.tileWidth,self.tileHeight))
		subText.close()
		print("Created "+stem+"_tiles subimages.txt tile atlas file.")

mediaDirectories = []														# scan for media directories and .tmx
tmxFiles = []
for root,dirs,files in os.walk("."):
	if "media" in dirs:														# found a media directory
		mediaDirectories.append(root)
	for f in files:
		if f[-4:] == ".tmx" and f[0] != "_":								# found a tmx file, not beginning with _
			tmxFiles.append(root+os.sep+f)

if len(mediaDirectories) != 1:												# must only be one media directory.
	raise Exception(str(len(mediaDirectories))+" media directories found at "+str(mediaDirectories))

for tmxFile in tmxFiles:
	tmap = TileMap(tmxFile)
	tmap.generateCode(mediaDirectories[0])
	tmap.generateAtlas(mediaDirectories[0])

