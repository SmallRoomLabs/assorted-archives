import xml.etree.ElementTree as xml
import os,sys
from PIL import Image

#
#		Represents a collection of custom properties.
#
class CustomPropertyCollection:
	def __init__(self):
		self.properties = {}															# properties defined
		self.hasKey = False

	def set(self,propertyName,propertyValue):
		propertyName = propertyName.lower().strip()										# preprocess the key
		self.properties[propertyName] = propertyValue									# store the property value
		self.hasKey = True 																# now has a key.

	def render(self):
		keys = [k for k in self.properties.keys()]										# get list of keys as a list.
		keys.sort()																		# sort by key name
		return ";".join([k+";"+self.properties[k] for k in keys])						# build semicolon seperated list.

#
#		Represents an object or tile image.
#
class TileImage:
	def __init__(self):
		self.width = 0 																	# tile/image size.
		self.height = 0
		self.source = "" 																# file the tile/image is in.
		self.properties = CustomPropertyCollection() 									# tiles custom properties, if any.
		self.gid = 0 																	# gid (e.g. tiled's internal ID) without flip bits
		self.allocatedID = 0 															# sequential ID allocated by me.
		self.sourceX = 0 																# position in tile/image file.
		self.sourceY = 0 
		self.usageCount = 0 															# number of usages in objects or layers + no of properties

	def render(self):
		return "{0}:{1}:{0}:{2}:{3}:{4}".format(self.allocatedID,self.gid,self.width,self.height,self.properties.render())

	def incrementUsageCount(self):
		self.usageCount = self.usageCount + 1

	def getSize(self):
		return [width,height]
#
#		Represents a collection of images. Closely coupled to TileImage object, so we are accessing that object's member
#		variables directly. 
#
class ImageCollection:
	def __init__(self):
		self.images = {}																# known tile or object images.

	def parse(self,xmlRoot):
		for child in xmlRoot:															# work through XML.
			if child.tag == "tileset":													# find tilesets which contain tile collections/images.
				for parts in child:
					if parts.tag == "image":											# have we found image, which is a tile set.
						self.createFromTileset(int(child.attrib["firstgid"]),int(child.attrib["tilewidth"]),int(child.attrib["tileheight"]),parts.attrib)
					if parts.tag == "tile":												# have we found a tile ?
						self.createFromSingleTile(parts,int(child.attrib["firstgid"]))
		print("Found {0} graphics in tilesets and graphic objects".format(len(self.images.keys())))

	def render(self,handle):
		idList = [x for x in self.images.keys()]										# get list of ids as a tuple, so can delete from it.
		handle.write("    map.imageCount = {0}\n".format(len(idList)))					# set total
		handle.write("    map.images.length = map.imageCount\n")						# allocate array.
		for id in idList:
			handle.write("    _TILImageUnpack(map.images[{0}],\"{1}\")\n".format(self.images[id].allocatedID,self.images[id].render()))

	def imageIsUsed(self,imageID):
		self.images[imageID].incrementUsageCount()

	def removeUnused(self):
		idList = [x for x in self.images.keys()]										# get list of ids as a tuple, so can delete from it.
		for id in idList:																# scan through
			if self.images[id].usageCount == 0:											# delete those unused.
				del self.images[id]
		print("Usage analysis leaves {0} tiles or graphic objects".format(len(self.images.keys())))

	def renumber(self):
		idList = [x for x in self.images.keys()]										# get and sort the keys
		self.toExternal = {}															# internal -> external mapping.
		self.toInternal = {}															# external -> internal mapping.
		newID = 1
		for id in idList:																# for each image
			self.images[id].allocatedID = newID											# give it our internal ID.
			self.toExternal[newID] = id 												# put in the translation tables.
			self.toInternal[id] = newID 	
			newID = newID+1 															# bump that ID.

		print("Allocated new internal image numbers")

	def toInternalGID(self,gid):
		if gid == 0:
			return 0
		flipBits = gid & 0xF0000000
		return self.toInternal[gid & 0x00FFFFFF] + (flipBits >> 4)

	def createFromTileset(self,firstgid,tileWidth,tileHeight,source):
		for y in range(0,int(int(source["height"])/tileHeight)):						# work through all the tiles in the image.
			for x in range(0,int(int(source["width"])/tileWidth)):
				img = TileImage() 														# create tile image
				img.width = tileWidth 													# set up size
				img.height = tileHeight 
				img.source = source["source"]											# save file source.
				img.gid = firstgid 														# save the gid
				img.sourceX = x * tileWidth 											# save position in tile set.
				img.sourceY = y * tileHeight
				firstgid = firstgid + 1 												# go to next gid
				self.images[img.gid] = img 												# store in image dictionary.

	def createFromSingleTile(self,xmlRoot,firstgid):
		gid = int(xmlRoot.attrib["id"]) + firstgid										# what ID is this object.
		imageRoot = xmlRoot.find("image")												# is there an image defined here ?
		if imageRoot is not None:														# then create it.
			img = TileImage()
			img.width = int(imageRoot.attrib["width"])									# save width and height.
			img.height = int(imageRoot.attrib["height"])
			img.source = imageRoot.attrib["source"]										# save image source (x and y are zero)
			img.gid = gid 																# save gid 
			assert(gid not in self.images)												# check not duplicate.
			self.images[img.gid] = img 													# store in image dictionary.
		propRoot = xmlRoot.find("properties")											# does this tile have any custom properties.
		if propRoot is not None:											
			assert(gid in self.images)													# if so, check that the image is defined.			
			for prop in propRoot:														# work through the properties.
				self.images[gid].properties.set(prop.attrib["name"],prop.attrib["value"]) # and store them away.
				self.images[gid].usageCount = self.images[gid].usageCount + 1			# increment the usage count, all tiles w/properties are
																						# exported whether used in objects or layers or not.
#
#	This is a single TMX object.
#								
class MapObject:
	def __init__(self,xmlRoot,depth,imageCollection):										
		self.depth = depth 																# remember depth.				
		self.imageCollection = imageCollection
		attr = xmlRoot.attrib 															# get the attributes
		self.rotation = 0.0 															# these are optional
		self.width = 0.0
		self.height = 0.0
		self.id = int(attr["id"])														# get the mandatory ones.
		self.gid = 0
		if "gid" in attr:																# gid supplied ?
			self.gid = int(attr["gid"])														
			n = self.gid & 0x0FFFFFFF													# remove the flip bits from the GID.
			imageCollection.imageIsUsed(n)												# bump image usage count for the base GID.
		self.x = float(attr["x"])														# Get positions.
		self.y = float(attr["y"])														
		if "rotation" in attr:															# Get optional ones.
			self.rotation = float(attr["rotation"])
		if "width" in attr:
			self.width = float(attr["width"])
		if "height" in attr:
			self.height = float(attr["height"])
		self.properties = CustomPropertyCollection()	 								# object may well have custom properties.
		if xmlRoot.find("properties") is not None:										# do we have properties.
			for prop in xmlRoot.find("properties"):										# scan through them
				self.properties.set(prop.attrib["name"],prop.attrib["value"])			# add them to the custom property object.

	def render(self):
		return "{0}:{1}:{2}:{3}:{4}:{5}:{6}:{7}:{8}".format(self.id,self.imageCollection.toInternalGID(self.gid),self.depth,self.x,self.y,self.width,self.height,self.rotation,self.properties.render())

#
#	This is a collection of objects, a quasi layer.
#																						
class ObjectGroup:
	def __init__(self,depth,imageCollection):
		self.depth = depth 																# remember depth group is at
		self.imageCollection = imageCollection 											# and what image collection we are using.
		self.objects = []																# objects in this group.

	def parse(self,xmlRoot):
		for obj in xmlRoot:																# work through the children
			assert(obj.tag == "object")													# should only be objects here I think
			self.objects.append(MapObject(obj,self.depth,self.imageCollection))			# create an object and add it to the list.
#
#	This is a layer.
#
class Layer:
	def __init__(self,depth,imageCollection):
		self.depth = depth 																# save depth and image collection.
		self.imageCollection = imageCollection

	def parse(self,xmlRoot):
		self.name = xmlRoot.attrib["name"]												# get name and map size.
		self.mapWidth = int(xmlRoot.attrib["width"])
		self.mapHeight = int(xmlRoot.attrib["height"])
		self.tiles = []																	# array of tiles.
		for t in xmlRoot.find("data"):													# read them in.
			n = int(t.attrib["gid"])
			self.tiles.append(n)														# append to tile array
			n = n & 0x0FFFFFFF 															# remove flip bits.
			if n > 0:																	# if not empty increment usage count.
				self.imageCollection.imageIsUsed(n)
		assert(len(self.tiles) == self.mapWidth * self.mapHeight)						# check count correct

	def render(self,handle,index):
		name = "map.layers["+str(index)+"]"												# the base for the layer.
		handle.write("    {0}.name = \"{1}\"\n".format(name,self.name))					# standard members
		handle.write("    {0}.mapWidth = {1}\n".format(name,self.mapWidth))
		handle.write("    {0}.mapHeight = {1}\n".format(name,self.mapHeight))
		handle.write("    {0}.depth = {1}\n".format(name,self.depth))
		handle.write("    {0}.rows.length = {0}.mapHeight\n".format(name))
		self.localTiles = [ 0 ] * len(self.tiles)										# convert TMX gids to internal ones.
		for i in range(0,len(self.tiles)):
			if self.tiles[i] > 0:
				self.localTiles[i] = self.imageCollection.toInternalGID(self.tiles[i])
		assert(len(self.localTiles) == self.mapWidth * self.mapHeight)					# check arrays right size.
		for row in range(0,self.mapHeight):												# for each row
			section = self.localTiles[row*self.mapWidth:(row+1)*self.mapWidth]			# get subsection for this row.
			section.append(0)															# we add an extra one as an 'end marker'
			assert(len(section) == self.mapWidth+1)										# check size.
			if not self.isClear(section):												# something in it.
				section = self.compress(section)
				self.dumpCode(handle,name+".rows[{0}]".format(row),section)
		handle.write("    for i = 0 to {0}\n".format(self.mapHeight-1))
		handle.write("        if {0}.rows[i].nextWrite = 0 then {0}.rows[i].rowEmpty = 1\n".format(name))
		handle.write("    next i\n")

	def isClear(self,list):
		for n in list:
			if n != 0:
				return False
		return True

	def compress(self,data):
		encode = []																		# result array
		n = 0 																			# start here
		while n < len(data):															# work to the end
			count = 1 																	# how many repeats ?
			while n+count < len(data) and data[n+count] == data[n]:
				count = count + 1
			if count > 2:																# only bother for 2+
				encode.append(-count)
			else:																		# if not encoding, count is 1 whatever
				count = 1
			encode.append(data[n])	 													# encode data
			n = n + count 																# skip over.
		return encode

	def dumpCode(self,handle,rowAddress,data):
		chunkSize = 40																	# how much data per line.
		while len(data) > 0:															# while more to do.
			split = min(chunkSize,len(data))											# work out where to split this time.
			if len(data) < chunkSize + 10:
				split = len(data)
			while data[split-1] < 0:													# can't be on an RLE compression point.
				split = split + 1
			code = ",".join([str(x) for x in data[:split]])								# get the code string.
			data = data[split:]
			handle.write("    _TILRowUnpack({0},\"{1}\")\n".format(rowAddress,code))
#
#	This is a collection of objectgroups and layers. They are put together because of the use of the order in the 
#	XML file to specify depth.
#
class LayerObjectCollection:
	def __init__(self,imageCollection):
		self.objCollections = []														# array of layers and object groups.
		self.layerCollections = []
		self.imageCollection = imageCollection 											# associated image collection.

	def parse(self,xmlRoot):
		depthLevel = 1																	# order of layers/objects specifies depth.
		for child in xmlRoot:
			if child.tag == "layer":													# layer
				layer = Layer(depthLevel,self.imageCollection)							# create layero
				layer.parse(child)														# and parse the XML
				self.layerCollections.append(layer)										# add to layer list
				depthLevel = depthLevel + 1												# up onelevel.
			if child.tag == "objectgroup":												# object group
				ogroup = ObjectGroup(depthLevel,self.imageCollection)					# create new one.
				ogroup.parse(child) 													# parse for objects.
				self.objCollections.append(ogroup)										# add to objectgroup list
				depthLevel = depthLevel + 1												# up one level.
		print("Found {0} layer(s)".format(len(self.layerCollections)))
		print("Found {0} object group(s)".format(len(self.objCollections)))
		self.depthCount = depthLevel - 1

	def renderObjectGroups(self,handle):
		count = 0 																		# count the number of objects
		for og in self.objCollections:
			count = count + len(og.objects)
		handle.write("    map.objectCount = {0}\n".format(count))						# define number of objects
		handle.write("    map.objects.length = map.objectCount\n")						# allocate array space.
		index = 1
		for og in self.objCollections:													# work through each collection
			for obj in og.objects:														# work through each group.
				handle.write("    _TILObjectUnpack(map.objects[{0}],\"{1}\")\n".format(index,obj.render()))
				index = index + 1

	def renderLayers(self,handle):
		handle.write("    map.layerCount = {0}\n".format(len(self.layerCollections)))	# define number of layers
		handle.write("    map.layers.length = map.layerCount\n")						# allocate array space.
		for i in range(1,len(self.layerCollections)+1):									# and render them, knowing what number they are/
			self.layerCollections[i-1].render(handle,i)										
#
#	And this represents a single map.
#
class Map:
	def __init__(self):
		self.imageCollection = ImageCollection() 										# create an image collection
		self.layerCollection = LayerObjectCollection(self.imageCollection)				# create collection of layers/objects.

	def parse(self,xmlFile):
		self.name = os.path.basename(xmlFile[:-4])										# get base name, strip tmx.
		self.name = self.name[0].upper()+self.name[1:].lower()							# camel case it
		self.directory = os.path.dirname(xmlFile)										# get directory file is in.
		print("Processing \"{0}\"".format(xmlFile))
		xmlRoot = xml.parse(xmlFile).getroot()											# load TMX & access the root
		self.imageCollection.parse(xmlRoot)
		self.layerCollection.parse(xmlRoot)
		self.imageCollection.removeUnused()
		self.imageCollection.renumber()
		self.mapWidth = int(xmlRoot.attrib["width"])									# get various constnats.
		self.mapHeight = int(xmlRoot.attrib["height"])
		self.tileWidth = int(xmlRoot.attrib["tilewidth"])
		self.tileHeight = int(xmlRoot.attrib["tileheight"])
		self.orientation = xmlRoot.attrib["orientation"]
		background = "00000000"															# default background colour.
		if "backgroundcolor" in xmlRoot.attrib:
			background = xmlRoot.attrib["backgroundcolor"][1:]+"FF"						# get background colour as RGB (make it RGBA)
		self.background = []															# convert to an array of numbers.
		for i in range(1,5):
			self.background.append(int(background[i*2-2:i*2],16))

	def render(self,handle):
		handle.write("// **** This code is automatically generated ****\n")
		handle.write("function TILSetup_"+self.name+"(map ref as TILMap)\n")
		handle.write("    i as integer\n")
		handle.write("    map.name = \"{0}\"\n".format(self.name.lower()))				# copying stuff in.
		handle.write("    map.orientation = \"{0}\"\n".format(self.orientation.lower()))
		handle.write("    map.mapWidth = {0}\n".format(self.mapWidth))
		handle.write("    map.mapHeight = {0}\n".format(self.mapHeight))
		handle.write("    map.tileWidth = {0}\n".format(self.tileWidth))
		handle.write("    map.tileHeight = {0}\n".format(self.tileHeight))
		handle.write("    map.depthCount = {0}\n".format(self.layerCollection.depthCount))
		for i in range(0,4):
			handle.write("    map.background[{0}] = {1}\n".format(i,self.background[i]))
																						# Load the atlas image.
		handle.write("    map.atlasImage = LoadImage(\"{0}.png\")\n".format(self.getAtlasStub()))

		self.imageCollection.render(handle) 											# Render the image collection.
		self.layerCollection.renderObjectGroups(handle) 								# Render the object groups.
		self.layerCollection.renderLayers(handle)										# Render the layers

		handle.write("    for i = 1 to map.imageCount\n")								# Now load in the actual sub images.
		handle.write("        map.images[i].image = LoadSubImage(map.atlasImage,\"img\"+str(map.images[i].imageID))\n")
		handle.write("    next i\n")
		handle.write("    for i = 1 to map.objectCount\n")								# Now load in the actual sub images.
		handle.write("        map.objects[i].image = LoadSubImage(map.atlasImage,\"img\"+str(map.objects[i].imageID && 0x00FFFFFF))\n")
		handle.write("    next i\n")
		handle.write("endfunction\n")

	def getAtlasStub(self):
		return "tmximg_"+self.name.lower()

	def renderAtlas(self,directory):
		self.images = []																# build collection of images
		for id in self.imageCollection.images.keys():									# from the ... image collection :)
			img = self.imageCollection.images[id]
			newObj = { "id":img.allocatedID, "xSource":img.sourceX, "ySource":img.sourceY, "width":img.width, "height":img.height }
			newObj["source"] = self.directory + os.sep + img.source
			self.images.append(newObj)
		self.images.sort(key=lambda k: k["width"], reverse=True)						# sort this so the list is widest
		width = 256																		# establish the width, should be power of 2
		while width < self.images[0]["width"]:											# keep doubling it till the widest fits.
			width = width * 2
		height = 0
		for i in range(0,len(self.images)):												# work through each image in turn.
			x = 0 																		# initialise start position to (0,0)
			y = 0
			foundSlot = False 	
			while not foundSlot: 														# keep going until found a slot.
				self.images[i]["x1"] = x 												# save current position.
				self.images[i]["y1"] = y 
				self.images[i]["x2"] = x + self.images[i]["width"]
				self.images[i]["y2"] = y + self.images[i]["height"]
				foundSlot = True 														# default to true 
				for c in range(0,i):													# check collision against all previous.
					if self.collide(i,c):												# if there's a collision.
						foundSlot = False 												# then this one isn't going to work.
				if not foundSlot:														# collided ?
					x = x + 16 															# try 16 pixels to the left.
					if x + self.images[i]["width"] > width:								# if it doesn't fit.
						x = 0 															# try next row down.
						y = y + 16
			height = max(height,self.images[i]["y2"])									# work out the final height.
		imgAtlas = Image.new("RGBA",(width,height),(255,255,255,0))						# create the blank image
		loadedImages = {}																# images loaded to make the final one.
		for img in self.images:
			if img["source"] not in loadedImages:										# load the image if not already available.
				loadedImages[img["source"]] = Image.open(img["source"])
			cropBox = (img["xSource"],img["ySource"],img["xSource"]+img["width"],img["ySource"]+img["height"])
			crop = loadedImages[img["source"]].crop(cropBox)
			pasteBox = (img["x1"],img["y1"])
			imgAtlas.paste(crop,pasteBox)
		imgAtlas.save(directory+os.sep+self.getAtlasStub()+".png")						# and save the atlas image
		print("Writing to "+directory+os.sep+self.getAtlasStub()+".png")

		self.images.sort(key=lambda k: k["id"])											# sort list by id
		subimage = open(directory+os.sep+self.getAtlasStub()+" subimages.txt","w")
		for img in self.images:
			subimage.write("img{0}:{1}:{2}:{3}:{4}\n".format(img["id"],img["x1"],img["y1"],img["width"],img["height"]))
		subimage.close()
		print("Writing subimages file")

	def collide(self,n1,n2):
		n1 = self.images[n1]															# convert to object references.
		n2 = self.images[n2]
		separate = 	n1["x2"] <= n2["x1"] or \
					n1["x1"] >= n2["x2"] or \
					n1["y1"] >= n2["y2"] or \
					n1["y2"] <= n2["y1"]
		return not separate

map = Map()
map.parse("tilesets\\isometric.tmx")
map.render(open("testProject\\tmx_isometric.agc","w"))
map.renderAtlas("testProject\\media")

map = Map()
map.parse("tilesets\\isometric_grass_and_water.tmx")
map.render(open("testProject\\tmx_isometric2.agc","w"))
map.renderAtlas("testProject\\media")

map = Map()
map.parse("tilesets\\square.tmx")
map.render(open("testProject\\tmx_square.agc","w"))
map.renderAtlas("testProject\\media")



