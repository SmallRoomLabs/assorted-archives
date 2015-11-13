; ***************************************************************************************
; ***************************************************************************************
;
;       Dungeon by Brian Sawyer, commented by Paul Robson.
;
; ***************************************************************************************
; ***************************************************************************************
0 clr : poke 59468,12
1 rem " dungeon copyright (c) 1979 brian sawyer"
2 rem " 1310 dover hill road"
3 rem " santa barbara, ca. 93103"
4 :
5 rem " cursor #15, nov/dec 1979"
6 rem " box 550, goleta, ca. 93017"
7 rem " lines 61000-65000 (c) 1979 cursor magazine"
8 :
10 rem " as of 12/29/79"
; ***************************************************************************************
;
;       Initialisation Section
;
; ***************************************************************************************
90 programName$ = "dungeon":cursorEdition$ = "15": gosub displayTitle@62000
;
;       Set up screen size and memory size
;
100 rowCount = 23:colCount = 40:mapMemSize = rowCount * colCount:mapGapSize = (25 - rowCount) * 40:rowCount = rowCount - 1:colCount = colCount - 1
130 rem " trick: dg$() strings go at end of memory!"
;
;       Allocate memory for the display using string arrays.
;
140 dim storage$(24):space$ = "                                        "
150 for i = 0 to 24:storage$(i) = space$ + "": next i
;
;       Set up screen control strings.
;
160 clearLine1$ = "{19}{17}                                       {19}{17}"
170 clearLine2$ = "{19}                                       {19}"
180 print "{147}setting up..."
; ***************************************************************************************
;
;       Initialise Map - first get Map Data address to use.
;
; ***************************************************************************************
190 mapData = peek (memoryTop) + 256 * peek (memoryTop + 1) - mapMemSize:screenPtr = 32768
;
;       Clear Map Data
;
200 for i = mapData + 40 to mapData + mapMemSize - 41: poke i,32: next i
;
;       Initialise Player
;
210 hitPoints = 50:gold = 0:experience = 0:prevExperience = 0:goldRemaining = 0:killCount = 0:lastGoldScanCount = 0:goldFound = 0:monsterPos = 0:monsterTurns = 0:playerTile = 160:ET = 160
;
;       Reset timer and create maze
;
220 timer$ = "000000":endMazeCreateTime = timer + 3600
230 gosub createMap@380
240 mapData = mapData - mapGapSize
;
;       Find empty slot for player on map.
;
250 playerPos = int ( rnd (1) * mapMemSize + mapData): if peek (playerPos) < > 160 then 250
260 endMazeCreateTime = 0: gosub waitClear@1410:playerPos = playerPos + screenPtr - mapData:playerTile = peek (playerPos): gosub openDisplay@600: poke playerPos,209:playerTile = 160
; ***************************************************************************************
;
;       Main Player Loop. Get next command.
;
; ***************************************************************************************
270 poke keyboard1,0: poke keyboard2,255: gosub getNextDirection@1240
;
;       Adjust HP for movement, check if dead.
;
280 hitPoints = hitPoints - .15 - 2 * canFloat: if hitPoints < 0 then 1190
;
;       Get offset, check if on the path / walking through walls, hit edge
;
290 offset = val ( mid$ ("808182404142000102",movement * 2 - 1,2)) - 41
;
;       Check for walking through space with shift key down (can Float)
;
300 if ( peek (playerPos + offset) = 32) and (canFloat < > 1) then mainLoop@270
310 if peek (playerPos + offset) = 127 then mainLoop@270
320 poke playerPos,playerTile:playerPos = playerPos + offset:playerTile = peek (playerPos): poke playerPos,209: gosub openDisplay@600: poke playerPos,209
;
;       Found Gold
;
330 if playerTile = 135 then gosub foundGold@1200
;
;       Found Monster
;
340 if playerTile > = 214 and playerTile < = 219 then gosub attackMonster@1000
350 if monsterPos > 0 then monsterTurns = monsterTurns + 1
360 if monsterTurns > 1 then gosub moveMonster@830
370 goto mainLoop@270
; ***************************************************************************************
;
;       Create the map.
;
; ***************************************************************************************
380 print int ((endMazeCreateTime - timer) / 60);"{157}  {145}"
;
;       Work out the size of the room to open
;
400 xCellSize = int ( rnd (1) * 9 + 2):yCellSize = int ( rnd (1) * 9 + 2)
;
;       Work out its position.
;
410 xCell = int ( rnd (1) * (rowCount - yCellSize - 1)) + 1:yCell = int ( rnd (1) * (colCount - xCellSize - 1)) + 1:mapPos = mapData + 40 * xCell + yCell
;
;       Check it is on the map space.
;
420 if mapPos + 40 * yCellSize + xCellSize > = mapData + mapMemSize then 530
;
;       Check it is all character 32 (e.g. walls - space character.)
;
430 for x = 0 to yCellSize + 1: for y = 0 to xCellSize + 1: if peek (mapPos + (x * 40) + y) < > 32 then 530
440 next y,x
;
;       Open up the room space
;
450 for x = 1 to yCellSize: for y = 1 to xCellSize: poke mapPos + (x * 40) + y,160: next y,x
;
;       Work down from cell offset by two to the bottom of the screen
;
460 for x = mapPos + 42 + (yCellSize * 40) to mapData + 999 step 40
;
;       If found a block fill the path in with 160, put a door in 102
;
470 if peek (x) = 160 then for y = mapPos + 42 to x step 40: poke y,160: next : poke y - 80,102: goto 490
480 next x
;
;       Work right from cell offset down by two to the right of the screen
;
490 for x = mapPos + 81 + xCellSize to mapPos + 121 + xCellSize: if (x - mapData) / 40 = int ((x - mapData) / 40) then 520
;
;       If found a block fill the path in with 160, put a door in 102
;
500 if peek (x) = 160 then for y = mapPos + 81 to x - 1: poke y,160: next : poke y - 1,102: goto 520
510 next 
;
;       Put a random monster in the room
;
520 yMonster = int ( rnd (1) * yCellSize) + 1:xMonster = int ( rnd (1) * xCellSize + 1): poke mapPos + xMonster + yMonster * 40, int ( rnd (1) * 6 + 214)
;
;       Go back if more time.
;
530 if timer < endMazeCreateTime then createMap@380
;
;       Put Gold on Map.
;
540 for goldItem = 1 to 11
550 location = int ( rnd (1) * mapMemSize) + mapData: if peek (location) < > 160 then 550
560 poke location,135:goldRemaining = goldRemaining + 1: next 
;
;       Frame Map
;
570 for row = 0 to rowCount: poke mapData + 40 * row,127: poke mapData + 40 * row + colCount,127: next row
580 for col = 0 to colCount: poke mapData + col,127: poke mapData + col + 40 * rowCount,127: next col
590 return 
; ***************************************************************************************
;
;       Open the part of the display the player is on.
;
; ***************************************************************************************
600 notUsed:K = - 40:notUsed:J = 3:notUsed:M = 40:notUsed:R = 3:goldScanCount = 0
610 if showMap = 1 then notUsed:K = - 80:notUsed:J = 5:notUsed:M = 80:notUsed:R = 4:showMap = 0
620 playerScreen = playerPos - 32767 - notUsed:R
;
;       Section here is the S command, appears not to have been implemented
;
630 if playerScreen + 32811 > 33768 then notUsed:M = 0
640 for dy = - 40 to 40 step 40: for dx = 1 to 3: if dy = 0 and dx = 2 then 820
650 mapSquareOpen = playerScreen + dy + dx:mapTile = peek (mapSquareOpen + mapData): poke mapSquareOpen + screenPtr,mapTile
660 if mapTile < 135 or mapTile = 160 then 820
;
;       Found treasure or gold in scan
;
670 mapTile = mapTile - 128: if mapTile < > 7 then 710
;
;       Found gold in scan
;
680 goldFound = 1 + goldFound + int ((gold + 1) * ( rnd (1)))
690 goldScanCount = goldScanCount + 1: if goldScanCount > lastGoldScanCount then gosub waitClear@1410: print "gold is near!": gosub showMessageDelay@1430:lastGoldScanCount = goldRemaining + 1
700 goto 820
;
;       Found monster in scan
;
710 monsterGraphic = mapTile + 128:monsterTurns = 0: poke mapSquareOpen + mapData,160
;
;       Identify Monster
;
720 if mapTile = 86 then monsterName$ = "spider":monHPcalc = 3
730 if mapTile = 87 then monsterName$ = "grue":monHPcalc = 7
740 if mapTile = 88 then monsterName$ = "dragon":monHPcalc = 1
750 if mapTile = 89 then monsterName$ = "snake":monHPcalc = 2
760 if mapTile = 90 then monsterName$ = "nuibus":monHPcalc = 9
770 if mapTile = 91 then monsterName$ = "wyvern":monHPcalc = 5
;
;       Set up monster
;
780 monHPcalc = int ( rnd (1) * hitPoints + (prevExperience / monHPcalc) + hitPoints / 4)
790 if monsterPos > 0 then poke mapData + monsterPos,monsterTile
800 monsterTile = mapTile + 128:monsterPos = mapSquareOpen
810 gosub waitClear@1410: print "a "monsterName$;" with";monHPcalc;"points is near.": gosub showMessageDelay@1430:monsterHP = monHPcalc
820 next dx: next dy:lastGoldScanCount = goldScanCount: return 
; ***************************************************************************************
;
;       Move monster. First work out vertical movement
;
; ***************************************************************************************
830 actualMove = 0:movement = 0:monstScreenPos = monsterPos + screenPtr: if abs (monstScreenPos + 40 - playerPos) < abs (monstScreenPos - playerPos) then movement = 40
840 if abs (monstScreenPos - 40 - playerPos) < abs (monstScreenPos - playerPos) then movement = - 40
850 if ( peek (monsterPos + movement + mapData) = 209) or ( peek (monsterPos + movement + mapData) > 128) then actualMove = actualMove + movement
;
;       Work out horizontal movement.
;
860 if abs (monstScreenPos - 1 - playerPos) < abs (monstScreenPos - playerPos) then movement = - 1
870 if abs (monstScreenPos + 1 - playerPos) < abs (monstScreenPos - playerPos) then movement = 1
880 if ( peek (monsterPos + actualMove + movement + mapData) = 209) or ( peek (monsterPos + actualMove + movement + mapData) > 128) then actualMove = actualMove + movement
;
;       If movement hits player then move
;
890 movement = actualMove: if monstScreenPos + movement = playerPos then 960
900 if monstScreenPos + movement < playerPos then 940
;
;       Flip to the other side ?
;
910 if movement = 41 then movement = 39
920 if movement = - 39 then movement = - 41
930 goto 960
940 if movement = 31 then movement = 41
950 if movement = - 41 then movement = - 39
;
;       Check if can move
;
960 if peek (monsterPos + movement + mapData) < 128 then poke monstScreenPos,monsterGraphic: return 
;
;       Actually move monster.
;
970 poke monstScreenPos,ET:monsterPos = monsterPos + movement:ET = peek (monsterPos + screenPtr): poke monsterPos + screenPtr,monsterGraphic: if monsterPos + screenPtr = playerPos then ET = 160: gosub attackMonster@1000
980 return 
;
;       Unused
;
990 monsterTile = V + 128:monsterPos = Y
; ***************************************************************************************
;
;       Attack a monster by running into him/her/it.
;
; ***************************************************************************************
1000 gosub waitClear@1410: print "{18}an attack!!!": gosub showMessageDelay@1430:power = hitPoints + experience
;
;       Work out damage done, check if player dead.
;
1010 playerDamage = rnd (1) * monsterHP / 2 + monsterHP / 4:monsterDamage = rnd (1) * power / 2 + power / 4:monsterHP = int (monsterHP - monsterDamage):hitPoints = int (hitPoints - playerDamage)
1020 if hitPoints < 1 then 1190
;
;       Check if monster offers a bribe.
;
1030 if (monsterHP / (hitPoints + 1) < 2) then 1090
1040 gosub waitClear@1410: print "the "monsterName$" will leave..": gosub showMessageDelay@1430
1050 gosub waitClear@1410: print "if you give it half your gold.": gosub showMessageDelay@1430
1060 gosub waitClear@1410: print "will you consent to this (y or n) ";
1070 gosub getFlashKey@1500
1080 if key1$ = "y" then gold = gold - gold / 2:playerTile = 160:monsterPos = 0:monsterTurns = 0: poke playerPos,209: return 
;
;       Check result of fight.
;
1090 if monsterHP > 1 then 1160
;
;       Monster is dead.
;
1100 gosub waitClear@1410:playerTile = 160:monsterTurns = 0:monsterPos = 0: poke playerPos,209: print "the "monsterName$" is dead!": gosub showMessageDelay@1430
1110 experience = experience + i:killCount = killCount + 1
1120 if experience < prevExperience * 2 then 1180
1130 prevExperience = experience:hitPoints = hitPoints * 3
1140 gosub waitClear@1410: print "your hit pts. have been raised.": gosub showMessageDelay@1430
1150 goto 1180
;
;       Monster survived.
;
1160 gosub waitClear@1410: print "the "monsterName$" has"monsterHP"hit points": gosub showMessageDelay@1430
1170 gosub displayStatus@1340
1180 return 
;
;       Player was killed.
;
1190 gosub waitClear@1410: print "you're dead!": gosub showMessageDelay@1430: goto quitGame@1350
; ***************************************************************************************
;
;       Found Gold
;
; ***************************************************************************************
1200 gold = gold + goldFound: gosub waitClear@1410: print "you found"goldFound"gold pieces!": gosub showMessageDelay@1430
1210 poke playerPos - screenPtr + mapData,160:playerTile = 160
1220 goldRemaining = goldRemaining - 1: if goldRemaining > 0 then return 
1230 goto quitGame@1350
; ***************************************************************************************
;
;       Get direction, delaying for message, handle S and Q options.
;
; ***************************************************************************************
1240 if showMsgFlag = 0 then gosub showMessageDelay@1430
1250 gosub displayStatus@1340
1260 if showMsgFlag then if timer > msgTime then gosub waitClear@1410: print "you may move."
;
;       Get keyboard command
;
1270 get key1$: if key1$ = "" then 1260
;
;       Check for numeric and floating
;
1280 movement = asc (key1$):canFloat = abs (movement > 127):movement = movement and 127
;
;       Handle wait and heal
;
1290 if movement = asc ("5") then hitPoints = hitPoints + 10 + sqr (experience / hitPoints)
;
;       Handle movement
;
1300 if movement > 48 and movement < 58 then movement = movement - 48:msgTime = 0: gosub waitClear@1410: return 
;
;       Handle quit and show (show is not implemented correctly.)
;
1310 if key1$ = "s" then showMap = 1:hitPoints = hitPoints - 20
1320 if key1$ = "q" then quitGame@1350
1330 goto 1250
; ***************************************************************************************
;
;       Display Status on Line 2.
;
; ***************************************************************************************
1340 print clearLine2$;"hit pts."; int (hitPoints + .5);"{157}  exp.";experience;"{157}  gold";gold;"   ": return 
; ***************************************************************************************
;
;       End Game code.
;
; ***************************************************************************************
1350 gosub waitClear@1410: print clearLine2$;"gold:";gold;" exp:";experience;" killed";killCount;"beasts"
;
;       Reveal the map.
;
1360 for N = mapGapSize to mapMemSize - 1 + mapGapSize:movement = peek (mapData + N): poke screenPtr + N,movement: next 
;
;       Empty Keyboard Buffer
;
1375 get key1$: if key1$ < > "" then 1375
;
;       Ask for a replay
;
1380 gosub waitClear@1410: print "want to play again";
1390 gosub getFlashKey@1500: if key1$ < > "n" then 180
1400 msgTime = 0: gosub waitClear@1410: print "{145}";: end 
; ***************************************************************************************
;
;       Wait if required and Clear
;
; ***************************************************************************************
1410 if showMsgFlag then if timer < msgTime then waitClear@1410
1420 print clearLine1$;:showMsgFlag = 0: return 
; ***************************************************************************************
;
;       Set up message being shown for 3 seconds.
;
; ***************************************************************************************
1430 msgTime = timer + 3 * 60:showMsgFlag = 1: return 
1440 if peek (32768) = 0 then stop 
1450 return 
; ***************************************************************************************
;
;       Get a keystroke with a flashing ? prompt
;
; ***************************************************************************************
1500 flashTime = 0:flashChar = 1
1510 get key1$: if key1$ < > "" then 1550
1520 if timer > flashTime then print mid$ ("? ",flashChar,1);"{157}";:flashTime = timer + 30:flashChar = 3 - flashChar
1530 goto 1510
1550 print "? ";key1$: return 
; ***************************************************************************************
;
;       Clear screen, setup keyboard and run program
;
; ***************************************************************************************
60300 print "{147}": clr : gosub keyboardSetup@60400: goto 100
;
;       Set up the keyboard access for different models of the PET.
;
60400 keyboard1 = 525:memoryTop = 134:keyboard2 = 515:cret$ = chr$ (13)
60410 if peek (50000) = 0 then return 
60420 keyboard1 = 158:memoryTop = 52:keyboard2 = 151
60430 return 
;
;       Print 4 horizontal lines.
;
60500 for i = 1 to 10: print "{192}{192}{192}{192}";: next i: return 
; ***************************************************************************************
;
;       Display the title page and set up the keyboard.
;
; ***************************************************************************************
62000 print "{147}{17}{17}"; tab( 9);"cursor #";cursorEdition$;"  ";programName$
62010 print "{17} copyright (c) 1979  by brian sawyer{17}"
62020 gosub printLine@60500
62030 print "{17}search for gold in the ancient ruins"
62080 print "{17}{17}{17}press {18}return{146} to begin"
62090 get key2$: if key2$ = "" then 62090
62100 goto preInitialise@60300
