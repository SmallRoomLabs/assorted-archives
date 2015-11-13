// **** This code is automatically generated ****
function TILSetup_Isometric(map ref as TILMap)
    i as integer
    map.name = "isometric"
    map.orientation = "isometric"
    map.mapWidth = 40
    map.mapHeight = 40
    map.tileWidth = 64
    map.tileHeight = 32
    map.depthCount = 2
    map.background[0] = 128
    map.background[1] = 34
    map.background[2] = 15
    map.background[3] = 255
    map.atlasImage = LoadImage("tmximg_isometric.png")
    map.imageCount = 3
    map.images.length = map.imageCount
    _TILImageUnpack(map.images[1],"1:1:1:64:128:obstacle;1")
    _TILImageUnpack(map.images[2],"2:2:2:64:128:")
    _TILImageUnpack(map.images[3],"3:4:3:66:37:")
    map.objectCount = 10
    map.objects.length = map.objectCount
    _TILObjectUnpack(map.objects[1],"1:0:2:1261.0:428.0:141.0:74.0:0.0:")
    _TILObjectUnpack(map.objects[2],"2:0:2:498.0:35.0:156.0:42.0:0.0:")
    _TILObjectUnpack(map.objects[3],"3:0:2:576.0:224.0:448.0:128.0:0.0:")
    _TILObjectUnpack(map.objects[4],"4:0:2:2183.0:747.0:228.0:96.0:0.0:")
    _TILObjectUnpack(map.objects[5],"5:0:2:960.0:416.0:192.0:96.0:0.0:")
    _TILObjectUnpack(map.objects[6],"6:0:2:1100.0:556.0:138.0:23.0:0.0:")
    _TILObjectUnpack(map.objects[7],"7:0:2:218.0:217.0:0.0:0.0:0.0:")
    _TILObjectUnpack(map.objects[8],"8:0:2:243.0:174.0:0.0:0.0:0.0:")
    _TILObjectUnpack(map.objects[9],"9:0:2:302.0:219.0:0.0:0.0:0.0:")
    _TILObjectUnpack(map.objects[10],"10:0:2:357.0:125.0:0.0:0.0:0.0:")
    map.layerCount = 1
    map.layers.length = map.layerCount
    map.layers[1].name = "Ground"
    map.layers[1].mapWidth = 40
    map.layers[1].mapHeight = 40
    map.layers[1].depth = 1
    map.layers[1].rows.length = map.layers[1].mapHeight
    _TILRowUnpack(map.layers[1].rows[0],"2,0,-8,2,0,-29,2,0")
    _TILRowUnpack(map.layers[1].rows[1],"0,-39,2,0")
    _TILRowUnpack(map.layers[1].rows[2],"3,2,3,2,3,-35,2,0")
    _TILRowUnpack(map.layers[1].rows[3],"3,3,2,3,-36,2,0")
    _TILRowUnpack(map.layers[1].rows[4],"3,2,3,-37,2,0")
    _TILRowUnpack(map.layers[1].rows[5],"3,-39,2,0")
    _TILRowUnpack(map.layers[1].rows[6],"3,1,1,-37,2,0")
    _TILRowUnpack(map.layers[1].rows[7],"3,1,1,-37,2,0")
    _TILRowUnpack(map.layers[1].rows[8],"2,2,1,-37,2,0")
    _TILRowUnpack(map.layers[1].rows[9],"3,2,1,1,-36,2,0")
    _TILRowUnpack(map.layers[1].rows[10],"3,-4,1,-35,2,0")
    _TILRowUnpack(map.layers[1].rows[11],"0,2,1,2,2,1,-34,2,0")
    _TILRowUnpack(map.layers[1].rows[12],"-3,2,1,1,-35,2,0")
    _TILRowUnpack(map.layers[1].rows[13],"-4,2,1,2,1,-33,2,0")
    _TILRowUnpack(map.layers[1].rows[14],"-4,2,1,2,1,-33,2,0")
    _TILRowUnpack(map.layers[1].rows[15],"-4,2,1,2,1,1,2,2,1,-29,2,0")
    _TILRowUnpack(map.layers[1].rows[16],"-4,2,1,2,1,1,2,1,-30,2,0")
    _TILRowUnpack(map.layers[1].rows[17],"-3,2,-3,1,-3,2,1,-30,2,0")
    _TILRowUnpack(map.layers[1].rows[18],"2,-3,1,2,1,1,2,1,-5,3,2,1,1,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[19],"1,1,-4,2,1,1,3,3,2,-4,3,1,1,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[20],"-7,2,1,-8,3,1,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[21],"-8,2,-7,3,2,0,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[22],"1,1,-6,2,1,-5,3,2,3,1,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[23],"-3,1,-5,2,1,-8,3,-23,2,0")
    _TILRowUnpack(map.layers[1].rows[24],"-4,1,-6,2,-8,3,-22,2,0")
    _TILRowUnpack(map.layers[1].rows[25],"2,1,3,3,-3,1,-3,2,-5,3,2,3,3,2,2,1,1,-18,2,0")
    _TILRowUnpack(map.layers[1].rows[26],"2,1,3,3,2,1,-5,2,-7,3,2,2,1,-19,2,0")
    _TILRowUnpack(map.layers[1].rows[27],"2,2,1,3,2,2,1,1,0,2,2,-6,3,2,2,1,-20,2,0")
    _TILRowUnpack(map.layers[1].rows[28],"2,2,1,1,-3,2,1,1,0,2,-7,3,1,1,-20,2,0")
    _TILRowUnpack(map.layers[1].rows[29],"-3,2,1,-5,2,1,2,-6,3,1,1,-21,2,0")
    _TILRowUnpack(map.layers[1].rows[30],"1,-3,2,1,2,1,-3,2,1,1,2,-5,3,-22,2,0")
    _TILRowUnpack(map.layers[1].rows[31],"-3,1,2,2,-3,1,-3,2,1,1,2,2,-3,3,-22,2,0")
    _TILRowUnpack(map.layers[1].rows[32],"-3,2,1,1,0,1,0,1,-3,2,1,-4,2,1,-22,2,0")
    _TILRowUnpack(map.layers[1].rows[33],"-4,2,-4,1,0,1,1,2,2,1,1,2,2,1,-22,2,0")
    _TILRowUnpack(map.layers[1].rows[34],"-6,2,-3,1,2,1,-3,2,1,1,-24,2,0")
    _TILRowUnpack(map.layers[1].rows[35],"-3,0,-9,2,1,-27,2,0")
    _TILRowUnpack(map.layers[1].rows[36],"2,2,0,0,1,-7,2,1,-27,2,0")
    _TILRowUnpack(map.layers[1].rows[37],"-3,2,0,1,1,-3,2,1,-30,2,0")
    _TILRowUnpack(map.layers[1].rows[38],"-5,2,-4,1,2,1,-29,2,0")
    _TILRowUnpack(map.layers[1].rows[39],"-8,2,1,1,-30,2,0")
    for i = 0 to 39
        if map.layers[1].rows[i].nextWrite = 0 then map.layers[1].rows[i].rowEmpty = 1
    next i
    for i = 1 to map.imageCount
        map.images[i].image = LoadSubImage(map.atlasImage,"img"+str(map.images[i].imageID))
    next i
    for i = 1 to map.objectCount
        map.objects[i].image = LoadSubImage(map.atlasImage,"img"+str(map.objects[i].imageID && 0x00FFFFFF))
    next i
endfunction
