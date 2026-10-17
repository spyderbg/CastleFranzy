
#
# Utility to convert strings from game2D's XML format to Excel and back.
#
# C.D. July 2006
#

import xml.dom.minidom
from pyExcelerator import *

# stuff to handle text encodings
import codecs
enc, dec, read_wrap, write_wrap = codecs.lookup('utf-8')


#
#
#

# def parseXMLStrings( inputfile, group_prefix):
    # strings = []
    # langs = set()
    # doc = xml.dom.minidom.parse( inputfile ).documentElement
    # doc.normalize()
    # for node in doc.getElementsByTagName('table') :
        # section = {}
        # sectionName = node.getAttribute('name')
        # sectionName = sectionName.lstrip(group_prefix)
        # section['name'] = sectionName
        # for child in node.childNodes :
            # if child.nodeType == child.ELEMENT_NODE :
                # name = child.tagName
                # langs.add(name)
                # data = []
                # for item in child.childNodes :
                    # if item.nodeType == child.ELEMENT_NODE :
                        # data.append( item.getAttribute('value') )
                # section[name] = data        
        # strings.append( section )
    # langs.remove('ids')

    ##ut english first...
    # langs.remove('english')
    # langlist = ['english'] + list(langs)
    # return (langlist,strings)


#
#
#

def parseXLSStrings( inputfile, group_prefix, sheet, sheet_name):
	try:
	    langs = []
	    sections = {}
	
	    name,vals = parse_xls(inputfile, group_prefix)[sheet]
	
	    #count languages
	    col = 1
	    while vals.has_key( (0,col) ) :
	        langs.append( vals[(0, col)] )
	        col += 1
	    numLangs = col - 1
	    
	    #count strings
	    row = 1
	    while vals.has_key( (row,0) ) :
	        row += 1
	    numStrings = row
	
	    for line in range(1,numStrings) :
	        sectionName = vals[(line, 0)]
	        #if setionName == '$' :
	        #    continue
	        sectionName = group_prefix + sectionName
	        if not sections.has_key( sectionName ) :
	            sections[sectionName] = { 'name' : sectionName, 'ids' : [] }
		    #print sections[sectionName]
	            for lang in langs :
	                sections[sectionName][lang] = []
	        section = sections[sectionName]
	        strId = vals[(line, 0)]
	        section['ids'].append(strId)
	        for i in range(numLangs) :
	            strn = ' '
	            if vals.has_key( (line, i + 1) ) :
	                strn = vals[(line, i + 1)]
	                # replace non-ascii characters
	                strn = strn.replace( u'\u2026' ,'...')
	                
	            section[langs[i]].append( strn )
	    for _lang in langs :
	        f = open(_lang + '_' + sheet_name + '.strings','wb')
	        f = write_wrap(f)
	        f.write('//' + _lang + '\r\n')
	        #import os
	        #os.system('pause')
	        for _line in range(1,numStrings) :
	            _sectionName = vals[(_line, 0)]
	            #print sections[_sectionName]
		    #print sections[_sectionName][_lang]
	            #print sections[_sectionName]['name']
	            #f.write('a')
	            f.write(sections[_sectionName]['name'] + "			\"" + sections[_sectionName][_lang][0] + '\"\r\n')
	      
	except Exception, exc:
		raise Exception('Error Converting Strings: [' + name + '][' + sectionName + '][' + strId + '][' + langs[i] + ']')
		
	return ( list(langs), sections.values() )
   

#
#
#

# def stringsXML2XLS( inputfile, outputfile, group_prefix) :

    # (langs,strings) = parseXMLStrings(inputfile, group_prefix)

    # wb = Workbook()
    # ws = wb.add_sheet('loc-base')

    # columns = ['section','id'] + langs
        
    # for i in range(len(columns)) :
        # ws.col(i).width = 200 * 32
        ##s.col(i).name = columns[i] # TODO: figure out how to write the column titles
        # ws.write(0, i, columns[i])

    # line = 1
    # for section in strings :
        # sectionName = section['name']
        # ids = section['ids']
        # for i in range(len(ids)) :
            # idname = ids[i]
            # ws.write(line, 0, sectionName)
            # ws.write(line, 1, idname)
            # for j in range(len(langs)) :
                # transl = section[langs[j]]
                # if i < len(transl) :
                    # ws.write(line, 2 + j, transl[i] )
                               
            # line += 1


    # wb.save(outputfile)

#
#
#

def stringsXLS2XML(inputfile, outputfile, group_prefix, caseoption) :

    (langs,strings) = parseXLSStrings(inputfile, group_prefix, 0)

    dom = xml.dom.minidom.parseString( "<strings/>" )
    doc = dom.documentElement

    groups = ['ids'] + langs

    for section in strings :
        sectionName = section['name']
        sectionNode = dom.createElement('table')
        sectionNode.setAttribute('name',sectionName)
        doc.appendChild(sectionNode)

        #dump groups
        for group in groups :
            groupSection = dom.createElement(group)
            sectionNode.appendChild(groupSection)
            for item in section[group] :
                if group == 'ids' :
                    elemName = 'id'
                else:
                    elemName = 'string'
                    if caseoption == 'up':
                        item = item.upper()
                    if caseoption == 'down':
                        item = item.lower()
                itemnode = dom.createElement( elemName )
                itemnode.setAttribute('value',item)
                groupSection.appendChild(itemnode)

    f = open(outputfile,'wb')
    f = write_wrap(f)
    f.write( dom.toprettyxml() )



def stringsXLSSheet2XML(inputfile, group_prefix, spec_name, ignoreList) :

	sheetcounter = 0
	
	xlWorkBook = parse_xls(inputfile, group_prefix)

	if spec_name != "":
		#find the specified spec sheet
		specSheetIndex = 0
		index = 0
		for sheet_name, values in xlWorkBook:
			if sheet_name == spec_name:
				specSheetIndex = index
			index += 1
		
		#organize redefined string in spec sheet
		(langs,strings) = parseXLSStrings(inputfile, group_prefix, specSheetIndex)
		redefinedString = {}
		for section in strings:
			redefinedString[section['name']] = section
	
	for sheet_name, values in xlWorkBook:
	    (langs,strings) = parseXLSStrings(inputfile, group_prefix, sheetcounter, sheet_name)
	    sheetcounter += 1
	
	    dom = xml.dom.minidom.parseString( "<strings/>" )
	    doc = dom.documentElement
	
	    groups = ['ids'] + langs
	
	    for section in strings :
	        sectionName = section['name']
	        
	        if spec_name != "":
	        	#overwrite texts redefined in spec sheet
		    	if redefinedString.has_key(sectionName):
		    		textToRemove = []
			    	for textId in section['ids']:
			    		if textId in redefinedString[sectionName]['ids']:
			    			stringIndex  = section['ids'].index(textId)
			    			newTextIndex = redefinedString[sectionName]['ids'].index(textId)
			    			isEmptyLine  = True
			    			
			    			#update text
			    			for lang in langs:
			    				newText = redefinedString[sectionName][lang][newTextIndex]
			    				if newText.strip() != "":
			    					section[lang][stringIndex] = newText
			    					isEmptyLine = False
			    			
			    			if isEmptyLine:
			    				textToRemove.append(stringIndex)
			    				
			    	#remove text
			    	textToRemove.sort()
			    	textToRemove.reverse()
			    	for stringIndex in textToRemove:
			    		for group in groups:
			    			section[group].pop(stringIndex)
	        
	        sectionNode = dom.createElement('table')
	        sectionNode.setAttribute('name',sectionName)
	        doc.appendChild(sectionNode)
	
	        # dump groups
	        for group in groups :
	            groupSection = dom.createElement(group)
	            sectionNode.appendChild(groupSection)
	            for item in section[group] :
	                elemName = 'string'
	                if group == 'ids' : elemName = 'id'
	                itemnode = dom.createElement( elemName )
	                item = removeIgnored(item, ignoreList)
	                itemnode.setAttribute('value',item)
	                groupSection.appendChild(itemnode)
	
	    #f = open(outfile + sheet_name + '.txt','wb')
	    #f = write_wrap(f)
	    #f.write( dom.toprettyxml() )


def removeIgnored(item, ignoreList):
	if item == None:
		return
	for ignoreString in ignoreList:
		item = item.replace(ignoreString, '')
	return item
# tests
#stringsXML2XLS('strings.xml','strings.xls')
#stringsXLS2XML('strings.xls','strings_out.xml')


if __name__ == '__main__':
	if len(sys.argv) < 4:
		print "Usage: stringconv.py (-xml2xls|-xls2xml|-xlssheets2xml) infile outfile [group_prefix] [--spec xls_sheet_name] [--upcase|--downcase] [--ignore string]"
	else:
		knownoptions = [ '--spec', '--upcase', '--downcase', '--ignore' ]
		infile = sys.argv[2]
		outfile  = sys.argv[3]
		
		group_prefix = ""
		ignoreList = []
		if len(sys.argv) > 4 and not (sys.argv[4] in knownoptions):
			group_prefix  = sys.argv[4]
		
		spec_name = ""	
		if '--spec' in sys.argv:
			spec_name = sys.argv[sys.argv.index('--spec') + 1]

                caseoption = None
                if '--upcase' in sys.argv:
			caseoption = 'up'
                if '--downcase' in sys.argv:
			caseoption = 'down'
		
		try:
			ignoreIndex = sys.argv.index('--ignore')
			while ignoreIndex > -1:
				ignoreList.append(sys.argv[ignoreIndex + 1])
				ignoreIndex = sys.argv.index('--ignore', ignoreIndex+2)
		except:
			pass


		if sys.argv[1] == '-xml2xls' :
                    stringsXML2XLS(infile, outfile, group_prefix)
                elif sys.argv[1] == '-xls2xml' :
                    stringsXLS2XML(infile, outfile, group_prefix, caseoption)
                elif sys.argv[1] == '-xlssheets2xml' :
                    stringsXLSSheet2XML(infile, group_prefix, spec_name, ignoreList)
                else :
                    print 'Invalid conversion requested :' + sys.argv[1]

                print 'Done'




