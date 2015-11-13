Setvirtualresolution(640,480)
ResetTimer()

/**
 *
 *	JavaDoc Comment demo
 *
// @param someParam<ref as int> comment
// @return xxxxxx

*/
//Check for required custom font files in media folder


fileok=GetFileExists("custom.png")
fileok=fileok+GetFileExists("custom subimages.txt")
If fileok<>2
    Repeat
        Print ("Both 'custom.png' and 'custom subimages.txt'")
        Print ("need to be in your media folder")
        Sync()
    Until Timer()>5
EndIF

pos = 0

If fileok=2
    //Get Custom Font
    fontimage = LoadImage ("custom.png")
	fontImage2 = LoadImage("demo.png")

	repeat
		CreateText(1,"Default Font")
		SetTextSize(1,25)
		SetTextPosition(1,10,50)

		CreateText(2,"Custom Font")
		SetTextSize(2,45)
		SetTextPosition(2,10,100)
		SetTextFontImage(2,fontimage)
		SetTextColor(2,255,128,0,255)

		CreateText(3,"My demo Custom font")
		SetTextSize(3,45)
		SetTextFontImage(3,fontImage2)
		SetTextPosition(3,10,pos)
		pos = mod(pos+5,600)
		Sync()

    Until Timer()>40
EndIf
End  
