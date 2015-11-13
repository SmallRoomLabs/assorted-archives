// **** This code is automatically generated ****
function TILSetup_Isometric_grass_and_water(map ref as TILMap)
    i as integer
    map.name = "isometric_grass_and_water"
    map.orientation = "isometric"
    map.mapWidth = 25
    map.mapHeight = 25
    map.tileWidth = 64
    map.tileHeight = 32
    map.depthCount = 1
    map.background[0] = 0
    map.background[1] = 0
    map.background[2] = 0
    map.background[3] = 0
    map.atlasImage = LoadImage("tmximg_isometric_grass_and_water.png")
    map.imageCount = 24
    map.images.length = map.imageCount
    _TILImageUnpack(map.images[1],"1:1:1:64:64:")
    _TILImageUnpack(map.images[2],"2:2:2:64:64:")
    _TILImageUnpack(map.images[3],"3:3:3:64:64:")
    _TILImageUnpack(map.images[4],"4:4:4:64:64:")
    _TILImageUnpack(map.images[5],"5:5:5:64:64:")
    _TILImageUnpack(map.images[6],"6:6:6:64:64:")
    _TILImageUnpack(map.images[7],"7:7:7:64:64:")
    _TILImageUnpack(map.images[8],"8:8:8:64:64:")
    _TILImageUnpack(map.images[9],"9:9:9:64:64:")
    _TILImageUnpack(map.images[10],"10:10:10:64:64:")
    _TILImageUnpack(map.images[11],"11:11:11:64:64:")
    _TILImageUnpack(map.images[12],"12:12:12:64:64:")
    _TILImageUnpack(map.images[13],"13:13:13:64:64:")
    _TILImageUnpack(map.images[14],"14:14:14:64:64:")
    _TILImageUnpack(map.images[15],"15:15:15:64:64:")
    _TILImageUnpack(map.images[16],"16:16:16:64:64:")
    _TILImageUnpack(map.images[17],"17:17:17:64:64:")
    _TILImageUnpack(map.images[18],"18:18:18:64:64:")
    _TILImageUnpack(map.images[19],"19:19:19:64:64:")
    _TILImageUnpack(map.images[20],"20:20:20:64:64:")
    _TILImageUnpack(map.images[21],"21:21:21:64:64:")
    _TILImageUnpack(map.images[22],"22:22:22:64:64:")
    _TILImageUnpack(map.images[23],"23:23:23:64:64:")
    _TILImageUnpack(map.images[24],"24:24:24:64:64:")
    map.objectCount = 0
    map.objects.length = map.objectCount
    map.layerCount = 1
    map.layers.length = map.layerCount
    map.layers[1].name = "Tile Layer 1"
    map.layers[1].mapWidth = 25
    map.layers[1].mapHeight = 25
    map.layers[1].depth = 1
    map.layers[1].rows.length = map.layers[1].mapHeight
    _TILRowUnpack(map.layers[1].rows[0],"24,24,23,11,19,19,12,23,24,24,23,7,2,2,1,4,2,2,3,4,4,1,3,4,1,0")
    _TILRowUnpack(map.layers[1].rows[1],"24,23,23,14,3,3,8,12,24,24,18,4,1,3,1,3,3,4,4,1,3,1,4,3,4,0")
    _TILRowUnpack(map.layers[1].rows[2],"11,15,15,7,1,4,3,20,23,11,7,2,1,3,1,3,4,4,3,2,2,3,4,2,1,0")
    _TILRowUnpack(map.layers[1].rows[3],"18,4,3,2,2,4,3,8,12,18,2,4,4,3,3,1,4,2,1,-3,4,-3,1,0")
    _TILRowUnpack(map.layers[1].rows[4],"14,2,1,4,2,1,4,2,8,7,5,17,6,-3,3,4,3,4,4,3,2,4,3,4,0")
    _TILRowUnpack(map.layers[1].rows[5],"10,6,2,1,4,4,1,3,4,3,8,12,10,6,2,-4,1,2,1,4,2,1,1,0")
    _TILRowUnpack(map.layers[1].rows[6],"24,18,1,2,4,3,3,5,6,5,13,9,11,7,3,4,1,3,1,3,4,2,-3,4,0")
    _TILRowUnpack(map.layers[1].rows[7],"24,14,4,2,5,6,2,8,22,9,23,24,10,6,2,1,3,1,5,6,2,3,4,4,2,0")
    _TILRowUnpack(map.layers[1].rows[8],"19,7,3,1,8,7,4,1,8,12,24,23,23,10,17,6,3,1,8,7,1,-3,3,1,0")
    _TILRowUnpack(map.layers[1].rows[9],"1,2,4,2,2,3,3,4,3,20,24,-4,23,18,2,2,3,1,4,4,-3,1,0")
    _TILRowUnpack(map.layers[1].rows[10],"3,3,5,13,6,1,2,2,5,9,23,23,24,23,24,14,1,3,1,1,3,3,-3,4,0")
    _TILRowUnpack(map.layers[1].rows[11],"2,4,16,24,10,6,2,4,20,23,23,24,23,23,24,14,2,4,2,4,5,6,4,3,1,0")
    _TILRowUnpack(map.layers[1].rows[12],"3,1,20,23,24,10,6,3,8,12,24,-3,23,24,14,1,2,1,5,9,18,4,3,4,0")
    _TILRowUnpack(map.layers[1].rows[13],"4,2,8,12,23,24,18,4,3,16,-3,24,23,24,18,1,3,1,16,24,14,1,3,2,0")
    _TILRowUnpack(map.layers[1].rows[14],"4,1,2,8,12,24,14,4,1,8,15,12,24,23,11,7,2,1,2,16,23,18,1,4,2,0")
    _TILRowUnpack(map.layers[1].rows[15],"3,4,3,2,8,19,7,2,2,3,3,8,15,19,7,3,1,2,5,9,24,14,1,2,3,0")
    _TILRowUnpack(map.layers[1].rows[16],"2,2,1,4,4,1,2,5,6,-3,2,1,3,4,3,5,13,9,24,24,18,4,3,4,0")
    _TILRowUnpack(map.layers[1].rows[17],"1,4,1,3,2,5,13,9,14,3,1,3,2,4,4,5,21,19,12,24,11,7,1,2,3,0")
    _TILRowUnpack(map.layers[1].rows[18],"2,1,-3,3,20,23,24,18,4,4,2,3,1,1,8,7,5,9,23,18,1,3,4,2,0")
    _TILRowUnpack(map.layers[1].rows[19],"4,2,4,1,2,8,15,19,7,4,5,6,4,2,4,5,17,9,23,11,22,13,6,4,1,0")
    _TILRowUnpack(map.layers[1].rows[20],"3,2,2,4,4,3,2,1,4,2,8,7,4,2,3,16,24,23,11,7,16,23,18,3,1,0")
    _TILRowUnpack(map.layers[1].rows[21],"1,3,1,2,-3,3,4,2,1,3,2,3,4,3,8,15,15,7,4,8,19,7,3,4,0")
    _TILRowUnpack(map.layers[1].rows[22],"1,2,3,4,1,3,-3,4,1,4,4,3,2,3,4,1,2,4,2,1,2,2,4,1,0")
    _TILRowUnpack(map.layers[1].rows[23],"4,2,3,2,1,4,2,2,1,-3,2,4,3,3,2,3,3,2,3,2,4,1,3,1,0")
    _TILRowUnpack(map.layers[1].rows[24],"-3,1,4,1,3,3,2,1,4,2,1,3,1,3,3,4,3,4,2,1,2,3,1,1,0")
    for i = 0 to 24
        if map.layers[1].rows[i].nextWrite = 0 then map.layers[1].rows[i].rowEmpty = 1
    next i
    for i = 1 to map.imageCount
        map.images[i].image = LoadSubImage(map.atlasImage,"img"+str(map.images[i].imageID))
    next i
    for i = 1 to map.objectCount
        map.objects[i].image = LoadSubImage(map.atlasImage,"img"+str(map.objects[i].imageID && 0x00FFFFFF))
    next i
endfunction
