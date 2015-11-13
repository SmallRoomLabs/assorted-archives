# README #

### What is this repository for? ###

ttf2agk is a script which converts truetype fonts to agk bitmap fonts (e.g. a .png and a subimages.txt file) directly without using .fnt files or similar. It works with ttf files directly or with files installed on your computer (this latter has not been tested on a Mac)

It currently allows borders of varying thicknesses and colours, a drop shadow, and for the main font to be coloured a whole colour, or to change between two colours, either left to right, top to bottom or centre out (radially).

It can be used programmatically as a Python library, or as a command line program.

### How do I get set up? ###

ttf2agk has two dependencies, Python and Pillow. It was developed on Python 3.4 but should run in version 2 of Python. Pillow is an updated fork of the standard Python Imaging Library (PIL) which it now has replaced as the standard Python library. If you have the old PIL for some reason it may still work.

http://www.python.org	has the Python executables (tick add to path when installing)
https://pillow.readthedocs.org/  explains how to add Pillow to it.

It was developed under Windows 8.1 but should work on Mac and Linux if you so wish.

The main deployment issue is it is easier to run Python applications if you have it in the Path. The current  Windows installer for 3.4 can do this automatically for you, but it does not do it by default ; you have to turn this option on in the installer.

The archive contains some sample fonts and files and a sample application which draws some fonts in varying sizes. These are all to experiment with, apart from Python and Pillow, all you need is the file ttf2agk.py

### How to use it ###

ttf2agk is a command line script. It can be used in one of two ways

#### As a command line utility ####

It can be used from the command line (or a batch file, or shell file) as follows:

python ttf2agk.py [definition] [definition] [definition] 

where each definition describes a different font conversion - you can have as many definitions as you like (as they tend to be a bit long probably one ppper line is readable). The definitions are modelled on, but not directly compatible with (obviously) CSS.

An example definition is shown below

	font:Arial;vertical:#ff8000,black;border:blue,1;dropshadow:#00ffff,5

Note, for most definitions the parameter will have to be in quote marks e.g.

python ttf2agk.py "font:Arial;vertical:#ff8000,black;border:blue,1;dropshadow:#00ffff,5"

This means - use Arial font, have the colour change vertically from orange (#ff8000) to black, a blue border 1% of the character width, and a dropshadow in cyan (#00ffff) 5% offset. Note the optional use of colour names and RGB values. This will produce two files, arial.png and arial subimage.txt

The complete set of 'commands', with their defaults, is :

	font:name 					(None)					Font name
	border:#xxxxxx,n 			(None)					Border colour and percentage size
	colour:#xxxxxx 				(Solid,White)			Solid colour
	radial:#xxxxxx,#xxxxxx 								Radial colour
	horizontal:#xxxxxx,#xxxxxx							Horizontal colour
	vertical:#xxxxxx,#xxxxxx 							Vertical Colour
	dropshadow:#xxxxxx,nn 		(No drop shadow)		Add a drop shadow
	renderwidth:nn 				(512)					Set the width of the output .png (should be a power of 2)
	fontsize:nn					(48)					Set the output fontsize*
	outputname:name				(Font name)				Change the output name of the font (do not include .png)

The only mandatory one is the Font Name.

The font size is the font size that is rendered to the .png. This can be anything you like ; small values mean less space is used in the download / graphic memory, but larger values give more detailed fonts. 48 was chosen as a reasonable middle ground, but experiment to see what works. AGK will scale the text for you.

#### As a library ####

If you have a look at the program libdemo.py this shows the possible three ways it can be called from Python (one of which is analogous to the Command Line Utility)

### Who do I talk to? ###

Repository owned by paulscottrobson@gmail.com
Bugs/Ideas/etc. welcome.

Paul Robson 02-Feb-2015
