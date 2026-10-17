import os
import re
from xml.dom import minidom

inputFileName = ''
outputFileName = ''
#--------------------------------------------------------------------
def SpriteToConstants(spriteFileName):
	spriteFile 	= file(spriteFileName)
	spriteLines = spriteFile.readlines()
	
	framesDef 	= ReadFrames(spriteLines)
	anims 		= ReadAnims(spriteLines)
	
	# Output Frames Constants
	if len(framesDef) > 0:
		frameId = 0
		while frameId < len(framesDef):
			OutputFrameConstants(framesDef[frameId])
			frameId += 1
		OutputFootTable()
	
	# Output Anims Constants
	if len(anims) > 0:
		for anim in anims:
			OutputAnimConstants(anim)
		OutputFootTable()
	

#--------------------------------------------------------------------	
def ReadFrames(spriteLines):
	#print 'ReadFrames'
	frames = []
	for line in spriteLines:
		#FRAME "Tweezers" // Index = 1, FModules = 1
		foundFrame = re.match('\s*FRAME\s+(")(?P<DESC>\w+)(")\s+(//)\s+(Index)\s+(=)\s+(?P<INDEX>\d+)(,)\s+.*', line)
		if (foundFrame != None):
			#print '   ' + foundFrame.group('DESC') + ' ' + foundFrame.group('INDEX')
			frames.append((foundFrame.group('DESC'), int(foundFrame.group('INDEX'))))
			
		#Append frame with no description to avoid wrong index
		else:
			foundFrame = re.match('\s*FRAME\s+.*', line)
			if (foundFrame != None):
				frames.append(('NULL', -1))
		
	#print len(frames)		
	return frames
	
#--------------------------------------------------------------------	

def ReadAnims(spriteLines):
	#print 'ReadAnims'
	anims = []
	for line in spriteLines:
		#ANIM "Alert" // Index = 3, AFrames = 12
		foundAnim = re.match('\s*ANIM\s+(")(?P<DESC>\w+)(")\s+(//)\s+(Index)\s+(=)\s+(?P<INDEX>\d+)(,)\s+.*', line)
		if (foundAnim != None):
			#print '   ' + foundAnim.group('DESC') + ' ' + foundAnim.group('INDEX')
			anims.append((foundAnim.group('DESC'), int(foundAnim.group('INDEX'))))
		
	#print len(anims)		
	return anims
	
#--------------------------------------------------------------------
def OutputHeader():
	outputFile = file(outputFileName, 'a')
	outputFile.seek(2)
	outputFile.write('//macro for ' + inputFileName + '\'s frame and anim index')
	
#--------------------------------------------------------------------	
def OutputFootTable():
	outputFile = file(outputFileName, 'a')
	outputFile.write('\n\n')
	
#--------------------------------------------------------------------			
def OutputFrameConstants(frame):
	outputFile = file(outputFileName, 'a')
	
	indexName 	= frame[0]
	indexValue 	= str(frame[1])	
	
	if(indexName != 'NULL'):
		#Constants
		outputText = '\n#define	' + 'Frame_' + inputFileName.rstrip('.sprite') + '_' + indexName + '	' + indexValue
		outputFile.write(outputText,)
		
#--------------------------------------------------------------------			
def OutputAnimConstants(anim):
	outputFile = file(outputFileName, 'a')
	
	indexName 	= anim[0]
	indexValue 	= str(anim[1])
	
	outputText = '\n#define	Anim_' + indexName + '	' + indexValue
	outputFile.write(outputText)
	
#--------------------------------------------------------------------			
#--------------------------------------------------------------------	
#--------------------------------------------------------------------
# Command line
import sys
import optparse
spritefiles = ''
if __name__ == '__main__':
	# Run
		
	if (len(sys.argv) > 1):
		#if output file exist, delete that
		outputFileName = sys.argv[1]
		if os.path.exists(outputFileName):
			os.remove(outputFileName)
			
		os.system('dir /b *.sprite > spritefiles.txt')
		spritefiles 	= file('spritefiles.txt')
		spritefile = spritefiles.readlines()
		for i in spritefile:
			inputFileName = i.strip()
			OutputHeader();
			SpriteToConstants(inputFileName)
	else:
		print 'Have to specify sprite file'
	
