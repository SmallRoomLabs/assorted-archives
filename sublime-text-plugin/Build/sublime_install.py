# ************************************************************************************************************************************
#
#   This Python script installs Sublime Text support for AGK2. It's a script, because I get round the problem of not knowing where 
#   AGK2 and Sublime Text are by looking for them. This avoids meddling with the Windows PATH environment variable.
#
#   This installs by building the file from its components and copying it into the packages directory. To do this will often
#   mean running it in Adminstrator mode. If you are not happy with this, set InstallMode to False (below) and it will just
#   build the package, you can then install it manually.
#   
#   Paul Robson 1 December 2014.
#
# ************************************************************************************************************************************

import zipfile,zlib                                                                         # modules needed to create package.
import os,sys,shutil

installMode = True                                                                          # if this is false, we only describe

def searchTree(rootDir):                                                                    # Scans part of the directory tree
    global compilerDir,sublimeDir,foundDir                                                  # looking for the AGK Compiler and 
    print("Searching for executables in "+rootDir)
    for root,dirs,files in os.walk(rootDir):                                                # the Sublime text executable
        if "AGKCompiler.exe" in files:
            compilerDir = root 
        if "subl.exe" in files:
            sublimeDir = root


    if compilerDir != "" and sublimeDir != "":
            foundDir = True

#
#   We try and figure out where AGK2 and Sublime Text are installed.  We need the first for the compiler path, and the
#   second to know where to copy the package we will build.
#
compilerDir = ""                                                                            # where the AGK Compiler is
sublimeDir = ""                                                                             # where the Sublime Text executable is.
foundDir = False                                                                            # set to true when not found them.

if False:                                                                                   # enabling this allows us to shortcut.
    print("**** CHEATING ****")
    compilerDir = "c:\\Program Files (x86)\\The Game Creators\\AGK2\\Tier 1\\Compiler"      # (defaults for Win8.1 x64 machine)
    sublimeDir = "c:\\Program Files (x86)\\Sublime Text 3"
    foundDir = True

if not foundDir:                                                                            # Find these executables so we don't                                            
    searchTree("C:\\Program Files (x86)")                                                   # have to tinker with the path.
if not foundDir:                                                                            # look in various Windows paths
    searchTree("C:\\Program Files")
if not foundDir:
    searchTree("C:\\")                                                                      # then look everywhere.
assert(foundDir)                                                                            # you'll have to hard code it !

print("Found AGK Compiler in : "+compilerDir)                                               # give a running commentary.
print("Found Sublime Text in : "+sublimeDir)
print()

#
#   Now we know where the build file needs to reference, we can create that file.
#
buildSource = """                                                                           
{
    "cmd": ["$COMPILER\\AGKCompiler.exe", "-run", "main.agc"],
    "variants": [
        { "cmd": ["$COMPILER\\AGKCompiler.exe", "-bct", "main.agc"],
          "name": "AGK Compile and Broadcast"
        },
        { "cmd": ["$COMPILER\\AGKCompiler.exe", "-agk", "main.agc"],
          "name": "AGK Compile only"
        }
    ]
}
"""

buildSource = buildSource.replace("$COMPILER",compilerDir)                                  # we create one specifically for this.
buildSource = buildSource.replace("\\","\\\\")                                              # double up backslashes, it's JSON.
handle = open("AppGameKit2.sublime-build","w")                                         # create the file.
handle.write(buildSource)
handle.close()

#
#   Create the zip file. Python has this built in (using zlib). We delete the old package, create a new one here
#   and add the syntax and build files to it.
#
if os.path.isfile("AppGameKit2.sublime-package"):                                           # delete zip file, if it exists
    os.remove("AppGameKit2.sublime-package")
packageZip = zipfile.ZipFile('AppGameKit2.sublime-package','w')                             # open a zip file for writing.
packageZip.write("AppGameKit2.tmLanguage",compress_type = zipfile.ZIP_DEFLATED)             # add the syntax file to it.
packageZip.write("AppGameKit2.sublime-build",compress_type = zipfile.ZIP_DEFLATED)          # add the build file to it.
packageZip.write("AppGameKit2.sublime-completions",compress_type = zipfile.ZIP_DEFLATED)    # add the completions file to it.
packageZip.close()                                                                          # we have now built the package.

#
#   Now we may physically copy the file to the Packages directory. This may well require Administrator elevation for the
#   Command Prompt, as if it is in c:\Program Files (x86) or c:\Program Files it won't allow copying in.
#
print("Copy AppGameKit2.sublime-package to "+sublimeDir+"\\Packages")                       # say what we'd do.
if installMode:
    shutil.copy("AppGameKit2.sublime-package",sublimeDir+"\\Packages")
else:
    print("Skipped, testing mode.")

# TODO: Broadcast does not work (i/f changed I think). 

