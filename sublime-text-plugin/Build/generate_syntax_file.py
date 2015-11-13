# *******************************************************************************************************************************#
#
#	This script creates the AppGameKit2.JSON-tmLanguage file which is used to generate the AppGameKit2.tmLanguage
#	file for syntax highlighting. The conversion is done by Sublime Text's "AAAPackageDev" package whose build function
#	allows this conversion.
#
#	Written by Paul Robson 1-Dec-2014
#
# *******************************************************************************************************************************

import sys,os

#
#		This parses the CommancmdList.cmdat file which is part of the AGK Install ; I think the IDE uses it for
#		autocompletion.
#

def loadCommandList():
	cmd = open("CommandList.dat","rb").read(131072)											# read the whole file in as bytes
	cmd = [(x if x >= 48 and x <= 127 else 32) for x in cmd]								# remove everything that is not std char
	cmd = "".join([chr(x) for x in cmd]).split(" ") 										# build a string and split on spaces
	cmd = [x for x in cmd if x < "a" and x != ""]											# remove anything not beginning with capital
	cmd = list(set(cmd))																	# make unique
	cmd.sort()																				# sort alphabetically
	return cmd

#
#		This creates a list of keywords sorted with the longest first. (Sounds like a good idea !). This list will have to
#		be maintained manually.
#

def loadKeywordList():
		keyWords = "remend,for,to,not,and,or,include,constant,dim,global,end,goto,gosub,return,function,exitfunction,endfunction,"
		keyWords = keyWords + "do,loop,repeat,until,while,endwhile,for,step,next,exit,if,else,elseif,endif,select,case,casedefault,"
		keyWords = keyWords + "endcase,endselect,inc,dec,then,as,integer,string,float,type,endtype,ref"
		keyWords = keyWords.lower().split(",")												# make it a list.
		keyWords.sort(key = len)															# sort on length
		keyWords.reverse() 																	# now longest first
		return keyWords

#
#		Write code for a list, under the given name, optionally trimming the final comma.
#
def writeListCode(target,wordList,name,removeComma):
	for i in range(0,len(wordList)):														# work through all words in the list
		contents = list(wordList[i]) 														# get the word needed, split into ch.
		contents = "".join(["["+c.upper()+c.lower()+"]" for c in contents])					# fix it to [Aa] and rejoin it.
		target.write("{\n")																	# open bracket
		target.write('"match": "\\\\b'+contents+'\\\\b",\n')								# write out the regex
		target.write('"name":  "'+name+'"\n')												# and the section name
		target.write("}\n" if i == len(wordList)-1 and removeComma else "},\n")				# closing bracket, maybe comma

#
#		Write Code out - it is seperated using plings.
#

def writeFixedCode(target,str):
	str = "\n".join([x.strip() for x in str.split("!")])									# convert ! to new line
	target.write(str)

header = '{!"name": "App Game Kit 2",! "scopeName": "source.agk",! "fileTypes": ["agc"],! "patterns": ['
syntax = '{!"name": "comment.block", !"begin": "[/][/]", !"end": ".*$"!}, !!{!"name": "comment.block", !"begin": "[`]", !"end": ".*$"!}, !!{!"match": "\\\".*\\\"", !"name": "string.quoted"!}, !!!{!"match": "\\\(|\\\)", !"name": "constant.numeric"!}, !!{!"match": "\\\\b[0-9]+|\\\\b[0-9]+.?[0-9]+", !"name": "constant.numeric"!}, !!{!"match": "([a-zA-z]+)(\\\()", !"name": "support.function.agk", !"captures": {!"1": { "name": "support.function.agk" }, !"2": { "name": "constant.numeric" } !} !}, !!{!"name": "comment.block", !"begin": "\\\\b[Rr][Ee][Mm]\\\\b", !"end": ".*$"!}, !!{!"name": "comment.block", !"begin": "\\\\b[Rr][Ee][Mm][Ss][Tt][Aa][Rr][Tt]\\\\b", !"end": "[Rr][Ee][Mm][Ee][Nn][Dd]"!},'
footer = ']!,"uuid": "4d2a704b-5401-40dc-8d9f-5ba52ef602d8"!}'


target = open("AppGameKit2.JSON-tmLanguage","w")
writeFixedCode(target,header)
writeListCode(target,loadCommandList(),"entity.name.function",False)
writeFixedCode(target,syntax)
writeListCode(target,loadKeywordList(),"keyword.source.agk",True)
writeFixedCode(target,footer)
target.close()

if os.path.isfile("AppGameKit2.tmLanguage"):
	os.remove("AppGameKit2.tmLanguage")

print("Now you need to compile AppGameKit2.JSON-tmLanguage using Sublime Text.")
print("Choose Build (Automatic) and select convert to property list.")
