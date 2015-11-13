#  ****************************************************************************************************************
#  ****************************************************************************************************************
# 
# 		File:		libdemo.py 
# 		Purpose:	Using ttf2agk as a library so you can script media creation if you like.
# 		Author:		Paul Robson
# 		Created:	03-Feb-15
# 
#  ****************************************************************************************************************
#  ****************************************************************************************************************

import ttf2agk
#  ****************************************************************************************************************
#
#		There are three ways of doing this - you can use one or all, it doesn't matter which. Also, obviously,
#		you can do it by hand or you can make a batch file or shell script as well.
#
#  ****************************************************************************************************************
#
#							You can use it as a library this way, creating objects
#
#  ****************************************************************************************************************

mapping = { "A":"C","C":"A" }													# Swaps A and C around, example of remapping
																				# allows introduction of other characters.
fs = ttf2agk.FontSource("Twinkle",mapping)										# Create font with that mapping
fs.setFontColour((255,255,0,255)).setBorder((0,32,255,255))						# Set font and border colour (size default)
fs.setRadialInterpolation((255,0,255,255),(255,255,0,255))						# Radial intrepretation of fill.
fi = ttf2agk.FontImage(fs)														# Create a font image using it (def 512 wide)
fi.write("TestApp/media/demo.png")												# Write it out.
print("Created demo.png")

#  ****************************************************************************************************************
#
#					or you can do it this way, using the createImageFromHash helper function.
#
#  ****************************************************************************************************************

dsc = { "font":"Arkitech.ttf","colour":(255,128,0,255),"border":((0,128,0,255),3),"dropshadow":((0,0,255,255),10) }
dsc["vertical"] = ((255,0,0,255),(0,255,0,255))									# add to the descriptor
msg = ttf2agk.createImageFromHash(dsc)											# returns a descriptive message
print(msg)				

#  ****************************************************************************************************************
#
#						or you can do it this way, using the 'texty CSS' style
#
#  ****************************************************************************************************************

cli = ttf2agk.CommandLineInterpreter()											# create a CLI object which processes these.
css = "font:Arial;vertical:#ff8000,black;border:blue,1;dropshadow:#00ffff,5"	# it's sort of CSS, really, a bit.
msg = cli.process(css)															# go do it.
print(msg)																		# and print the message.