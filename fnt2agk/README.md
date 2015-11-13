# README #

### What is this repository for? ###

This is an application to convert .fnt/.png files as produced by many bitmap font generating programs into a
form suitable for AppGameKit 2.

Many programs (e.g. bmglyph) and websites (e.g. Littera) generate bitmap fonts as a combination of a .png file
which is the graphics, and a .fnt file which specifies where they are on the .png file (and provides kerning
and other information). This script converts a font set of this type into one directly useable by AppGameKit 2
- a png file arranged differently and a 'subimages' text file which describes the position of each font character
in that png.

Incidentally, fonts do not have to be white - you can use colour effects, when you 'colour' a font in AGK2 it
applies a tint of that colour, so you can get different colour fonts out of the same core font and have 
gradient patterns and so on.

### How do I get set up? ###

fnt2agk has two dependencies, Python and Pillow. It was developed on Python 3.4 but should run in version 2 of
Python. Pillow is an updated fork of the standard Python Imaging Library (PIL) which it now has replaced as the
standard Python library. If you have the old PIL for some reason it may still work.

http://www.python.org	has the Python executables (tick add to path when installing)
https://pillow.readthedocs.org/  explains how to add Pillow to it.

It was developed under Windows 8.1 but should work on Mac and Linux if you so wish.

The main deployment issue is it is easier to run Python applications if you have it in the Path. The current 
Windows installer for 3.4 can do this automatically for you, but it does not do it by default ; you have to turn
this option on in the installer.

This contains sample .fnt files and .png files so it is quite a sizeable download, relatively, but the only
thing you need outside python/pillow is the fnt2agk.py script.

### How to use it ###

fnt2agk is a command line script. It can be used in one of two ways

#### As a command line utility ####

As provided, it converts a single pair to a second pair. There are two parameters, which are 'source' and 'target'.

So, if you have two files in the "FNT" format, e.g. retro.png and retro.fnt then you can execute the following from
the Windows command line.

python agk2fnt.py retro result 

This will take the files retro.png and retro.fnt (from the first parameter) and produce files result.png and 
"result subimages.txt" (note the space) in the same directory. You can also use directory names e.g.

python agk2fnt.py source\retro ..\media\result

for example.


#### As a script ####

Usage here is similar but different. In this scenario, you actually amend the fnt2agk.py file to produce a script which
you can run to do the conversions without typing in the names every time. Examples of this are given in the 
script itself. You just run it with:

python agk2fnt.py 

Or indeed if you run it from the GUI and associate .py files with the Python interpreter, though I haven't actually tried
this.

### Who do I talk to? ###

Repository owned by paulscottrobson@gmail.com
Bugs/Ideas/etc. welcome.

Paul Robson 01-Dec-14
