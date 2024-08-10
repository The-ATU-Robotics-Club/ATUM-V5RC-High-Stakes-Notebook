import os

# These words are removed from the header filename before searching for
# corresponding source files; add these to "collect" source files together
# when multiple correspond to one header. 
apiKeywords = []
implKeywords = ["robot", "In", "De"]

# Where to output the LaTeX.
mainAPI = "software/code/api.tex"
implementation = "software/code/implementation.tex"

# Where to look for the files.
includeAPIDir = "software/code/files/api/include"
srcAPIDir = "software/code/files/api/src"
includeImplDir = "software/code/files/impl/include"
srcImplDir = "software/code/files/impl/src"

def getRootsAndFiles(dirname):
    rootsAndFiles = []
    for root, _, files in os.walk(dirname):
        for file in files:
            rootsAndFiles.append((root, file))
    return rootsAndFiles

def inputFile(root, file, output):
    output.write(f"\subsection{{{file}}}\n")
    output.write(f"\lstinputlisting[language=C++]{{{root + '/' + file}}}\n")
    
def handleDirs(includeDir, srcDir, output, keywords = []):
    for includeRoot, includeFile in includeDir:
        inputFile(includeRoot, includeFile, output)
        for keyword in keywords:
            includeFile = includeFile.replace(keyword, '')
        for i in range(len(srcDir)):
            srcRoot, srcFile = srcDir[i]
            # Check if include file name is in the source file name.
            # Also check if the last two letters in the source file name are in
            # the include file name (to fix cases like time and timer). 
            if includeFile[0:-4] in srcFile and srcFile[-6:-4] in includeFile:
                inputFile(srcRoot, srcFile, output)

includeAPI = getRootsAndFiles(includeAPIDir)
srcAPI = getRootsAndFiles(srcAPIDir)
includeImpl = getRootsAndFiles(includeImplDir)
srcImpl = getRootsAndFiles(srcImplDir)

with open(mainAPI, 'w') as output: 
    handleDirs(includeAPI, srcAPI, output, apiKeywords)

with open(implementation, 'w') as output:
    handleDirs(includeImpl, srcImpl, output, implKeywords)