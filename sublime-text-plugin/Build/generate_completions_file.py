# *******************************************************************************************************************************#
#
#	This script creates the AppGameKit2.sublime-completions file, currently this is just a list of identifiers.
#
#	Written by Paul Robson 2-Dec-2014
#
# *******************************************************************************************************************************

import sys,os

#
#		This parses the CommancmdList.cmdat file which is part of the AGK Install ; I think the IDE uses it for
#		autocompletion (cut and pasted from generate_syntax_file.py)
#

def loadCommandList():
	cmd = open("CommandList.dat","rb").read(131072)											# read the whole file in as bytes
	cmd = [(x if x >= 48 and x <= 127 else 32) for x in cmd]								# remove everything that is not std char
	cmd = "".join([chr(x) for x in cmd]).split(" ") 										# build a string and split on spaces
	cmd = [x for x in cmd if x < "a" and x != ""]											# remove anything not beginning with capital
	cmd = list(set(cmd))																	# make unique
	cmd.sort()																				# sort alphabetically
	return cmd

handle = open("AppGameKit2.sublime-completions","w")										# write to this file
handle.write("""																			
{
    "scope": "source.agk",

    "completions":
    [
""")

completions = loadCommandList()																# get completions
completions = ['\t\t"'+x+'"' for x in completions]											# make JSON style
handle.write(",\n".join(completions))														# combine and write
handle.write("""
    ]
}""")

handle.close()																				# and done.