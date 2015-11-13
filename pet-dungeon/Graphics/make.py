#
# 				Extract tiles wanted for Dungeon.
#

from PIL import Image,ImageDraw

class TileImage:
	def __init__(self):
		self.tileImage = Image.open("rpgtiles.png")
		self.cRom = []
		f = open("petgraphics.bin","rb")
		for i in range(0,2048):
			self.cRom.append(ord(f.read(1)))
		self.tileSet = Image.new("RGBA",(8*32,2*32),(0,0,0,255))
		self.tileID = 0

	def create(self,name,petCharacter,x,y,isTile = False):
		x = int(x/32) * 32
		y = int(y/32) * 32
		part = self.tileImage.crop((x,y,x+32,y+32))
		part.save("Pet_Gfx_"+name+".png")
		if isTile:
			self.tileSet.paste(part,(self.tileID*32,0),part)
		del part

		img = Image.new("RGB",(32,32))
		for x in range(0,8):
			for y in range(0,8):
				n = self.cRom[(petCharacter & 127)*8+y]
				if petCharacter < 128:
					n = 255-n
				if n & (128 >> x) == 0:
					for x1 in range(0,4):
						for y1 in range(0,4):
							img.putpixel((x*4+x1,y*4+y1),0xFFFFFF) 
		img.save("Gfx_"+name+".png")

		if isTile:
			self.tileSet.paste(img,(self.tileID*32,32))
			self.tileID = self.tileID + 1
		del img

	def saveTiles(self):
		self.tileSet.save("tileset.png")

tiles = TileImage()

# 32 (Walls) Space
tiles.create("wall",32,721,426,True)

# 102 (Door) 4 level hash
tiles.create("door",102,687,498,True)

# 127 (Frame) Diagonal Blocks
tiles.create("frame",127,148,591,True)

# 135 (Gold) Inverse-G
tiles.create("gold",135,560,688)

# 160 (Path) Solid Block White
tiles.create("path",160,689,465,True)

# 160 (Room) Solid Block White
tiles.create("room",160,1204,437,True)

# 209 (Player) Black Circle on White
tiles.create("player",209,1071,45)

# 214 (Spider) Black X on White
tiles.create("monster_1",214,947,135)

# 215 (Grue) Black Hole on White
tiles.create("monster_2",215,1328,200)

# 216 (Dragon) Club on White
tiles.create("monster_3",216,1972,180)

# 217 (Snake) Bar on white
tiles.create("monster_4",217,1582,143)

# 218 (Nuibus) Diamond on white
tiles.create("monster_5",218,1294,176)

# 219 (Wyvern) Black + on White
tiles.create("monster_6",219,1169,275)

tiles.saveTiles()