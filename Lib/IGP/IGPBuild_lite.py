# -*- coding: utf-8 -*-
import sys
import struct
import os
import IGPConfig_lite
import warnings
import hashlib
import subprocess

warnings.simplefilter("ignore",DeprecationWarning)

#Used to store all image positions in file.
ImageHashDictionary = {}
DataIndexDictionary = {}
GAMES = []
total = 0


def out(s):
        print("**" + s)

def writeByte(f, data):
        f.write(struct.pack('<B', data))

def readShort(f):
    try:
        return struct.unpack("<H", f.read(2))[0]
    except struct.error:
        return -1
        
def readByte(f):
    try:
        return struct.unpack("<B", f.read(1))[0]
    except struct.error:
        return -1

def readS32(file):
    try:
        return struct.unpack("<L", file.read(4))[0]
    except struct.error:
        return -1

def writeS32(f, data):
    f.write(struct.pack('<L', data))


def packImage(output, game, fileName, lang, country = ''):
      global total
 # if game == IGPConfig_lite.GAME:
 #     print("skiping:" + game)
 # else:
      print("filename" +fileName)
      if country == '':
          out("\tPacking " + str(fileName) + " " + lang)
          img = os.path.join('RES', game, lang, fileName)
      else:
          out("\tPacking " + str(fileName) + " " + lang + '_'+country)
          img = os.path.join('RES', game, lang, country, fileName)
    
      imageMd5 = getMd5(img)
      if (ImageHashDictionary.has_key(imageMd5)):
          #Image was already in pack. Store it's position in file instead to save space
          writeByte(output, 1)
          writeS32(output, ImageHashDictionary[imageMd5])
          print("already in pack")
      else:
           #Image was not in pack. Add it's hash to the dictionary for future checks and append it to the file
          ImageHashDictionary[imageMd5] = output.tell()
          writeByte(output, 0)
          putFile(output, img)
          print("not in pack")
          kk =  os.path.getsize(img)
          
          total = total + kk
          print(kk)
          print("total:")
          print(total)
          
      
def parseConfig():
    inConfigFile    = open("ads.bin", 'rb')
    outConfigFile   = open("outputRes/IGPConfig_lite", 'wb')
    K_GAMES = readShort(inConfigFile)
    outConfigFile.write("v" + IGPVersion)
    writeS32(outConfigFile, K_GAMES)
    #add current game, only to retrieve short names (for the moment)
    if (not  os.path.isdir(os.path.join(os.getcwd(),'RES', IGPConfig_lite.GAME))):
    	print ('ERROR: current game, '+IGPConfig_lite.GAME+' does not have a folder in RES')    
    	print ('If this is a game on development, please include a folder in RES similar to the others with dummy files (only the titles need to be right).')    
    	sys.exit()
    GAMES.append(IGPConfig_lite.GAME)
        
    for game in range(K_GAMES):
        name    = inConfigFile.read(4)
        prioCat = readShort(inConfigFile)
        prioWN  = readShort(inConfigFile)
        prioBS  = readShort(inConfigFile)
        newPrice= readByte(inConfigFile)
        pricesLenght = readShort(inConfigFile)
        
        GAMES.append(name)
        if (not  os.path.isdir(os.path.join(os.getcwd(),'RES', name))):
                 print ('ERROR: game '+name+' does not have a folder in RES!') 
                 sys.exit()
        
        outConfigFile.write(name)
        writeS32(outConfigFile, prioCat)
        writeS32(outConfigFile, prioWN)
        writeS32(outConfigFile, prioBS)
        #print "PrioCat:" + str(prioCat)
        #print "prioWN:" + str(prioWN)
        print "NAME:"+name +" PRIOBS:" + str(prioBS)
        writeByte(outConfigFile, newPrice)
        writeS32(outConfigFile, pricesLenght)
        for i in range(pricesLenght):
            priceByte = readByte(inConfigFile)
            writeByte(outConfigFile, priceByte) 
        
        
        for i in range(32):
            outConfigFile.write("0") #TODO Fake MD5
        writeByte(outConfigFile, 0)     #URL length
        writeByte(outConfigFile, 0)     #FILE length
    
def putFile(dest, origin):
    originFile = open(origin, 'rb')
    size = os.path.getsize(origin)
    writeS32(dest, size)
    dest.write(originFile.read())
    originFile.close()

def getMd5(fileName, excludeLine="", includeLine=""):
    """Compute md5 hash of the specified file"""
    m = hashlib.md5()
    try:
        fd = open(fileName,"rb")
    except IOError:
        print "Unable to open the file in readmode:", filename
        return
    content = fd.readlines()
    fd.close()
    for eachLine in content:
        if excludeLine and eachLine.startswith(excludeLine):
            continue
        m.update(eachLine)
    m.update(includeLine)
    return m.hexdigest()

"""
    LINK FILE STRUCTURE:
    VERSION            5b
    AMMOUNT OF LINKS   1b
        LINK_SIZE      1b
        LINK           LINK_SIZE * 1b
"""

""" method deprecated in new design
def makeMsg():
    out("Packing msg...")
    OUTPUT_MSG	    = open('outputRes/msg','wb')

    for lang in IGPConfig_lite.LANG:
        out("   " + lang)
        msg = os.path.join('RES\\common\\', lang + '.txt')
        msgFile = open(msg, 'r')
	messages = msgFile.read()[2:].split('\r\x00\n\00')
	for i in range(2):
        	writeByte(OUTPUT_MSG, len(messages[i]))
		OUTPUT_MSG.write(messages[i])
        msgFile.close()
    OUTPUT_MSG.close()
"""

def makeIGPData(version):
    out("Packing igpdata...")
    OUTPUT_DATA	    = open('outputRes/igpdata','wb')

    OUTPUT_DATA.write(IGPConfig_lite.GAME + chr(0))                  #5B Current game name
    OUTPUT_DATA.write(version + chr(0))                         #6B Current IGP Version
    writeByte(OUTPUT_DATA, len(IGPConfig_lite.LANG))                 #1B number of languages
    for lang in IGPConfig_lite.LANG:
        OUTPUT_DATA.write(lang)
    OUTPUT_DATA.close()

def makeTexts():
			prevdir = os.getcwd()
			os.chdir('RES/texts')
			textpath = os.path.join(os.path.curdir,"clean.bat")
			subprocess.Popen(textpath).communicate() 
			textpath = os.path.join(os.path.curdir,"export_strings.bat")
			subprocess.Popen(textpath).communicate() 
			os.chdir(prevdir)
			print os.getcwd()
			textLanguagesListFile = open ( 'res/texts/strings.lst', 'w' )
			textLanguagesListHeaderFile = open ( 'sources/classesToAdd/string_headers/langinfo.h', 'w' )
			textLanguagesListFile.write('strings.bar                     strings.h\n\n')
			textLanguagesListHeaderFile.write("#define LANG_COUNT\t\t" + str(len(IGPConfig_lite.LANG)))
			textLanguagesListHeaderFile.write ( '\n\nstatic int LANG_INFO[ ' + str(len(IGPConfig_lite.LANG)) + ' ][ 4 ] = \n{\n' )
			
			for lang in IGPConfig_lite.LANG:
			  filename = os.path.join('RES','texts','bin',lang+'.bin')
			#    filename = os.path.join('IGP','Classes','string_headers','langinfo.h')
			  print filename
			  if (os.path.exists(filename)):
			      textLanguagesListFile.write( '+bin\\'+lang+'.bin                             '+lang+'_SP_BIN\n') 
			      textLanguagesListFile.write( '+bin\\'+lang+'.off                             '+lang+'_SP_OFF\n\n')
			      if(lang == 'EN'):
			          textLanguagesListHeaderFile.write ( '{ EN_SP_BIN, NUM_STRINGS_EN, BIN_SIZE_EN, OFF_SIZE_EN },\n')
			      else:
			          textLanguagesListHeaderFile.write ( '{ '+lang+'_SP_BIN, NUM_STRINGS_'+lang+', BIN_SIZE_'+lang+', OFF_SIZE_'+lang+' },\n')
			  else:
			      out("##########ERROR########## skipping language not found: " + lang)  
			textLanguagesListHeaderFile.write ( '};\n')            
			textLanguagesListHeaderFile.write ( '\n static char* LANG_ORDER['+str(len(IGPConfig_lite.LANG)) +'] = \n')   
			textLanguagesListHeaderFile.write ( '{ ')      
			textLanguagesListFile.write('+endoffile.xxx                                  STRINGS_NUM_FILES\n')
			for lang in IGPConfig_lite.LANG:
			      if(lang == 'EN'):
			          textLanguagesListHeaderFile.write ( '"EN", ')
			      else:
			          textLanguagesListHeaderFile.write ( '"' + lang + '", ')    
			textLanguagesListHeaderFile.write ( '}; ')                 
			textLanguagesListFile.close()
			textLanguagesListHeaderFile.close()
			os.chdir('RES/texts')
			textpath = os.path.join(os.path.curdir,"export_stringBar.bat")
			subprocess.Popen(textpath).communicate() 
			
def makeTextures():
    out("Packing textures...")

    OUTPUT_WINDOW       = open('outputRes/window','wb')
    OUTPUT_DATA         = open('outputRes/textures','wb')
#    OUTPUT_PRICE_DATA		= open('outputRes/prices','wb')

    iUpArrow            = os.path.join('RES','common','newDesign','upArrow.png')
    iDownArrow          = os.path.join('RES','common','newDesign','downArrow.png')  
    iGameloftImage      = os.path.join('RES','common','newDesign','gameloftImage.png')
    iBuyButton          = os.path.join('RES','common','newDesign','buyButton.png')
    iBackButton         = os.path.join('RES','common','newDesign','backButton.png')
    iVideoButton        = os.path.join('RES','common','newDesign','videoButton.png')
    iSendButton         = os.path.join('RES','common','newDesign','sendButton.png')
    iInfoButton          = os.path.join('RES','common','newDesign','about_icon.png')  
    iBgImage            = os.path.join('RES','common','newDesign','tabBackground.png')
    
    iBSImage            = os.path.join('RES','common','newDesign','BS.png')
    iWNImage            = os.path.join('RES','common','newDesign','WN.png')
    iCAImage            = os.path.join('RES','common','newDesign','CA.png')
    iFBImage            = os.path.join('RES','common','newDesign','FB.png')
    iScrBg              = os.path.join('RES','common','newDesign','scrBackground.png')
    iCoverBack          = os.path.join('RES','common','newDesign','coverBack.png')
  
    putFile(OUTPUT_WINDOW, iUpArrow)
    putFile(OUTPUT_WINDOW, iDownArrow)
    putFile(OUTPUT_WINDOW, iGameloftImage)
    putFile(OUTPUT_WINDOW, iBuyButton)
    putFile(OUTPUT_WINDOW, iBackButton)
    putFile(OUTPUT_WINDOW, iVideoButton)
    putFile(OUTPUT_WINDOW, iSendButton)
    putFile(OUTPUT_WINDOW, iInfoButton)
    putFile(OUTPUT_WINDOW, iBgImage)
    putFile(OUTPUT_WINDOW, iBSImage)
    putFile(OUTPUT_WINDOW, iWNImage)
    putFile(OUTPUT_WINDOW, iCAImage)
    putFile(OUTPUT_WINDOW, iFBImage)
    putFile(OUTPUT_WINDOW, iScrBg)
    putFile(OUTPUT_WINDOW, iBgImage)
    putFile(OUTPUT_WINDOW, iCoverBack)

  #  for lang in IGPConfig_lite.LANG:
  #  	iNew = os.path.join('RES','common','newDesign','new_'+lang+'.png')
  #  	putFile(OUTPUT_WINDOW, iNew)
  #  	iNewPrice = os.path.join('RES','common','newDesign','newprice_'+lang+'.png')
  #  	putFile(OUTPUT_WINDOW, iNewPrice)
    OUTPUT_WINDOW.close()
    
    for game in GAMES:
        for lang in IGPConfig_lite.LANG:
            DataIndexDictionary[game + '_' + lang] = -1
            dirname = os.path.join('RES',game,lang)
            countryExceptions = [f for f in os.listdir(dirname) if os.path.isdir(os.path.join(dirname, f)) and f != '.svn']
            for e in countryExceptions:
                DataIndexDictionary[game + '_' + lang + '_' + e] = -1				
#        OUTPUT_PRICE_DATA.write(game)
#        #writeS32(OUTPUT_PRICE_DATA, game)
#        filePrice = os.path.join('RES',game, 'gameprice.txt')
#        filePriceFile = open(filePrice, 'rb')
#        readShort(filePriceFile)
#        prices = filePriceFile.read()
#        out(prices)
#        #writeS32(OUTPUT_PRICE_DATA, len(prices))
#        OUTPUT_PRICE_DATA.write(prices)
#    OUTPUT_PRICE_DATA.close()

    OUTPUT_DATA.seek( 4 + len(DataIndexDictionary) * (15 + 4))   # XXXX_LG_LC + '\0' +4bytes for pos in file

    for pack in DataIndexDictionary:
        out(pack)
        game = pack.split("_")[0]
        lang = pack.split("_")[1]
        if (len(pack.split("_")) > 2):
            loca = pack.split("_")[2]
        else:
            loca = ''
        DataIndexDictionary[pack] = OUTPUT_DATA.tell()
        packImage(OUTPUT_DATA, game, 'splash.png', lang, loca)
        packImage(OUTPUT_DATA, game, 'icon.png', lang, loca)
        
        if (loca == ''):
            shortTitle	=os.path.join('RES',game, lang, 'gameshorttitle.txt')
            title      = os.path.join('RES',game, lang, 'gametitle.txt')
            description = os.path.join('RES',game, lang, 'gamedescription.txt')
        else:
            shortTitle    =os.path.join('RES',game, lang,loca, 'gameshorttitle.txt')
            title      = os.path.join('RES',game, lang, loca, 'gametitle.txt')
            description = os.path.join('RES',game, lang, loca, 'gamedescription.txt')
        
       
        shortTitleFile  = open(shortTitle, 'rb')
        titleFile       = open(title, 'rb')
        descriptionFile = open(description, 'rb')
       
        readShort(shortTitleFile)
        readShort(titleFile)
        readShort(descriptionFile)
        
        title           = titleFile.read()
        shortTitle      = shortTitleFile.read()
        description     = descriptionFile.read()
        writeS32(OUTPUT_DATA, len(title))
        OUTPUT_DATA.write(title)
        writeS32(OUTPUT_DATA, len(shortTitle))
        OUTPUT_DATA.write(shortTitle)        
        writeS32(OUTPUT_DATA, len(description))
        OUTPUT_DATA.write(description)
		
        
        #print title
        #print description
        for i in range(IGPConfig_lite.NSCREENSHOTS):
            packImage(OUTPUT_DATA, game, 'SCR' + str(i) + '.png', lang, loca)            
    OUTPUT_DATA.seek(0)
    writeS32(OUTPUT_DATA, len(DataIndexDictionary))
    for pack in DataIndexDictionary:
        OUTPUT_DATA.write(pack + ".IGP")
        for i in range(15 - len(pack + ".IGP")):
            writeByte(OUTPUT_DATA, 0)
        writeS32(OUTPUT_DATA, DataIndexDictionary[pack])       
    
    OUTPUT_DATA.close()
    
########
########
INPUT_VERSION = open('version.txt','r')
IGPVersion = INPUT_VERSION.read()
INPUT_VERSION.close()
out("*******************************************")
out("Building resources for IPHONE IGP v" + IGPVersion + "...")
out("*******************************************")
parseConfig()

makeTextures()
#makeMsg()
makeIGPData(IGPVersion)
makeTexts()