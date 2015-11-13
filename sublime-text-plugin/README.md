# README #

This is a Sublime Text 3 Plugin for App Game Kit 2. 

The program was originally created by Adam Hodgson who gave permission for this work to be used.

### What does it offer ? ###

* Free Open Source etc.
* Syntax highlighting for AppGameKit 2
* Ctrl+B running from Sublime Text, errors in the bottom of the editor and so on.
* AutoComplete has the whole AGK2 command set (e.g. not just ones you've already typed in)

### What is missing ###

* Support for broadcasting. The easiest way to fix this is to do most of the work in ST and load up
the IDE just to broadcast. Paul has helped me with this and I hope to get it working.
* Completion with information. It is supposed to be possible to have the command completions list
the parameters (so it comes up with SetVirtualResolution(width,height) and allows you to enter values)
and also to have some description of the function, but I haven't figured this out yet.
* If you want to run a program, the directory of the current file is where it looks for main.agc , so if
you are editing a file in a subdirectory, it will warn it can't find main.agc. I know some other 
build systems have the same problem. For code editing, if you like to have modules in subdirectories,
leave them in the root until you've done most of the coding then change them at the last minute.

### How do I get set up? ###

The one dependency is Python. For convenience you should ask the Python installer to add the executable
to the system path (otherwise other than python you'd type something like c:\python34\python)

The program was tested on Python 3.4 on Windows 8.1, with AGK2 Alpha 9
and Sublime Text 3.  It is Windows only at present. If someone with a Mac and ST2 will help I think it
would be fairly easy to get running on a Mac.

What the setup script does is to scan your machine *only* to find out where App Game Kit 2 and Sublime
Text are installed. This means you do not have to tinker with the PATH ; it creates a 'special' version
of the Plugin for your machine.

There are three basic setups here.  

First is fully automatic. The problem with this is it needs to be run from a Command Line at the 
Administrator level. This is because it is most likely that your Sublime Text installation is in 
c:\Program Files (x86)\Sublime Text 3. This directory cannot be written to without elevated 
privileges on Windows.

To do this , run a command shell as administrator, go to the source directory you downloaded, then 
go to the 'Build' subdirectory and run python sublime_install.py

If you have your installation in an odd place this may take a while as it will search the whole HD for
AGK2 and Sublime Text. You can edit the script to shortcut this if you know where they are.

Second is 'do it yourself'.  With this method you install the package yourself, so you don't need 
administrator mode. Edit the sublime_install.py script and change installMode = True to installMode = False
(capital "F").

Then run it as above. This will build the AppGame2Kit.sublime-package but it won't try to install it. You
can then copy it into the packages directory.

Easiest of all is to use the prebuilt one, but this will only work if your AGK2 and Sublime Text installs are
 in C:\Program Files (x86)\The Game Creators and C:\ProgramFiles (x86)\Sublime Text 3

### Contribution guidelines ###

All contributions and ideas are welcome. This has only been tested on my machine, but I can't see why it wouldn't
work.

### Who do I talk to? ###

Written by Paul Robson (paul@robsons.org.uk)

1-2 December 2014
