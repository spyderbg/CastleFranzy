--
-- Extracted from Battle Arena maxscripts
-- 
-- C.D. nov 08
--
function stringReplace inputstring token newtoken =
(
	newstring = ""
		
	if ( inputstring != undefined ) do
	(
		substr = inputstring
		toklen = token.count
		
		while ( substr.count > 0 ) do
		(
			pos = findString substr token
			if ( pos != undefined ) then
			(
				append newstring ( substring substr 1 (pos-1) )
				append newstring newtoken
				substr = substring substr (pos + toklen) -1
			)
			else
			(
				append newstring substr
				substr = ""
			)
		)
	)
	return newstring
)

function stringSplit inputstring = 
(
	return filterString inputstring " ,\n\t"
)

----------------------------------------------------------------------
--
-- Generic Map (key,value) object
-- Maxscript has no such object.
--
----------------------------------------------------------------------

struct Map
(
	keys = #(),
	values = #(),
	count = 0,
	
	fn insert key value = 
	(
		pos = findItem keys key
		if ( pos > 0 ) then
		(
			-- replace existing entry
			values[pos] = value
		)
		else
		(
			-- create new entry
			append keys key
			append values value 
			count = count + 1		
		)
	),
	
	fn haskey key = ( return ( findItem keys key ) > 0	),	
	
	fn getvalue key = 
	(
		pos = findItem keys key
		if ( pos > 0 ) then return values[pos] else return undefined	
	),
	
	fn findkey value =
	(
		pos = findItem values value
		if ( pos > 0 ) then return keys[pos] else return undefined		
	)
)


----------------------------------------------------------------------
--
-- Generic Set object
-- Maxscript has no such object.
--
----------------------------------------------------------------------

struct Setobj
(
	items = #(),
	count = 0,
	
	fn insert item = 
	(
		pos = findItem items item
		if ( pos > 0 ) then
		(
			return false
		)
		else
		(
			-- create new entry
			append items item
			count = count + 1
			return true		
		)
	),
	
	fn hasitem item = ( return ( findItem items item ) > 0	)
)

function getUserPropNoQuotes obj proName = 
(
	propVal = getUserProp obj proName
	if propVal != undefined do
	(
		-- remove quotes
		propVal = stringReplace propVal "\"" ""		
	)
	return propVal
)

function collectUserProperties obj =
(
	userProps = Map()
	
	userpropsbuffer = ( getUserPropBuffer obj ) as StringStream
	
	while not eof userpropsbuffer do 
	(
		line = readLine userpropsbuffer
		propName = ( stringSplit line )[1]
		if propName != undefined do
		(
			propVal = getUserPropNoQuotes obj propName
			if propVal != undefined do
			(
				userProps.insert propName propVal
			)
		)
	)
	
	return userProps
)

function removeProperty obj propName =
(
	props = collectUserProperties obj
	newprops = stringstream ""
	for i = 1 to props.count do
	(
		objPropName = props.keys[i]
		objPropVal = props.values[i]
		if objPropName != propName do
		(
			format "% = \"%\"\n" objPropName objPropVal to:newprops
		)		
	)
	setUserPropBuffer obj newprops
)

function collectAllPropertyNames sel =
(
	propNames = Setobj()
	for obj in sel do
	(
		props = collectUserProperties obj
		for propName in props.keys do
		(
			propNames.insert propName
		)	
	)
	return propNames
)

function collectAllPropertyValues sel =
(
	propValues = Setobj()
	for obj in sel do
	(
		props = collectUserProperties obj
		for propVal in props.values do
		(
			propValues.insert propVal
		)	
	)
	return propValues
)

----------------------------------------------------------------------
--
--  Properties Editor
--
--  Known bug: the editable text box does not work beyond a certain
--  number of characters (around 3932).
--
----------------------------------------------------------------------

FLOATER_WIDTH = 600
FLOATER_HEIGHT = 506
TXTBOX_MARGINX = 40
TXTBOX_MARGINY = 100
ROLLOUT2_HEIGHT = 20

function openPropertiesEditor =
(
	---
	---
	---
	
	rollout PropEditor "Properties editor"
	(
		local gameObjsNames
		local lastTxtSize

		button reload_objects "Reload Objects" across:5
		button select_pick "Select from scene"
		checkbox autoselect "Auto-select"
		button select_object "Pick"
		checkbox autopick "Auto-pick"	 

		dropdownlist obj_sel items:#("") labelOnTop:false --across:2
	
		edittext userprop_txt editable:true labelOnTop:true bold:true \
			fieldWidth:(FLOATER_WIDTH-TXTBOX_MARGINX) \
			height:(FLOATER_HEIGHT-TXTBOX_MARGINY-ROLLOUT2_HEIGHT)
		
		function displayProps = 
		(
			if obj_sel.selected != undefined do
			(
				obj = getnodebyname obj_sel.selected
				if obj != undefined then
				(
					proptext = getUserPropBuffer obj
					userprop_txt.text = proptext
					-- lastTxtSize = userprop_txt.text.count			
					userprop_txt.readOnly = false
					
					-- Bug plug
					--if ( userprop_txt.text != proptext ) do
					--(
					--	userprop_txt.readOnly = true
					--)
				)
				else
				(
					userprop_txt.text = "*** Object Deleted ***"
				)			
			)
		)			
		
		on userprop_txt changed txt do
		(
			-- Bug plug
			--print txt.count
			--print lastTxtSize 
			limitReached = ( txt.count == lastTxtSize )
			userprop_txt.readOnly = limitReached
			lastTxtSize = txt.count
			
			if not limitReached and obj_sel.selected != undefined do
			(
				obj = getnodebyname obj_sel.selected
				if obj != undefined then
				(
					txt2 = stringReplace txt "\n" "\r\n"
					setUserPropBuffer obj txt2
				)
			)
		)

		function loadObjs = 
		(
			lastTxtSize = -1
			gameObjsNames = for o in objects collect o.name	
			sort gameObjsNames 
			obj_sel.items = gameObjsNames
			displayProps()
		)
		
		function pickSelection = 
		(
			obj = getnodebyname obj_sel.selected
			if obj != undefined then
			(
				select obj
			)
		)
		
		function selectPicked =
		(
			lastTxtSize = -1
			if ( selection.count > 0 ) do
			(
				loadObjs()
				
				obj = selection[1]
				
				pos = finditem obj_sel.items obj.name
				if ( pos > 0 ) do
				(
					obj_sel.selection  = pos
				)
				
				displayProps()	
			)
		)
		
		on reload_objects pressed do loadObjs()		
		
		on obj_sel selected sel do 
		(
			lastTxtSize = -1	
			displayProps()
			if autopick.checked do pickSelection()
		)
			
		on autoselect changed isSelected do
		(
			callbacks.removeScripts #selectionSetChanged
			if isSelected do
			(
				callbacks.addScript #selectionSetChanged "thenewfloater.rollouts[1].selectPicked()" 
			)
		)
		
		on select_pick pressed do selectPicked()
		on select_object pressed do pickSelection()	
		

		on PropEditor open do 
		(
			loadObjs()
			selectPicked()	
		)
		
		on PropEditor close do
		(
			callbacks.removeScripts #selectionSetChanged
		)
	
		on PropEditor resized size do
		(
			-- Note: I can't get horizontal resizing to work...
		
			sizeX = size[1]
			sizeY = size[2]
			
			PropEditor.width = sizeX
			PropEditor.height = sizeY - ROLLOUT2_HEIGHT
			
			userprop_txt.width = PropEditor.width - TXTBOX_MARGINX			
			userprop_txt.height = PropEditor.height - TXTBOX_MARGINY
		)			
	)

	---
	---
	---
	
	rollout SerachAndReplaceRollout "User Properties search and replace"
	(					
		combobox PropNameEdit "Property name:" height:1
		combobox PropValueEdit "Property value:" height:1
		
		button selectByPropName "Select by Property"
		button selectByPropNameAndValue "Select by Property and value"			
		button setPropValue "Set Property for selected objects"	
		button removeProp "Remove Property for selected objects"		

		on SerachAndReplaceRollout open do
		(
			propNamesSet = collectAllPropertyNames objects
			PropNameEdit.items = propNamesSet.items

			propValuesSet = collectAllPropertyValues objects
			sort propValuesSet.items
			PropValueEdit.items = propValuesSet.items
		)

		on PropNameEdit entered txt do
		(
			propNameList = PropNameEdit.items
			insertItem txt propNameList 1
			PropNameEdit.items = propNameList
		)
		
		on PropValueEdit entered txt do
		(
			propValList = PropValueEdit.items
			insertItem txt propValList 1
			PropValueEdit.items = propValList
		)
				
		
		on setPropValue pressed do
		(
			propName = PropNameEdit.text
			propVal = PropValueEdit.text

			for obj in selection do
			(
				setUserProp obj propName ( "\"" + propVal + "\"" ) 
			)			
		)
		
		on removeProp pressed do
		(
			propName = PropNameEdit.text
			if propName != undefined and propName != "" do
			(
				if queryBox "Are you sure you want to remove this user property? (No Undo!)" title:"Remove user property" then
				(
					for obj in selection do
					(
						removeProperty obj propName
					)
				)
			)			
		)
		
		on selectByPropName pressed do
		(
			clearSelection()
			
			propName = PropNameEdit.text
			for obj in objects do
			(		
				propVal = getUserPropNoQuotes obj propName
				if propVal != undefined do
				(
					selectMore obj
				) 			
			)
		)
		
		on selectByPropNameAndValue pressed do
		(
			clearSelection()
			
			propName = PropNameEdit.text
			propVal = PropValueEdit.text
			for obj in objects do
			(		
				objPropVal = getUserPropNoQuotes obj propName
				if propVal != undefined and objPropVal == propVal do
				(
					selectMore obj
				) 			
			)
		)
	)
		
	global theNewFloater = newRolloutFloater "Properties editor" FLOATER_WIDTH FLOATER_HEIGHT
	addRollout PropEditor theNewFloater
	addRollout SerachAndReplaceRollout theNewFloater rolledUp:true
)

openPropertiesEditor()
