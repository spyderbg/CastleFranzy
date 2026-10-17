----------------------------------------------------------------------
-- Animation Selector
-- updated 2009-03-11
----------------------------------------------------------------------

global g_minFrame = -16777216
global g_maxFrame = 16777216

global g_maxSpinnerStartTimeOffset = 0
global g_minSpinnerEndTimeOffset = 0

global g_animationManagerFloater

global g_animationManagerWidth = 300
global g_animationManagerHeight = 560

global g_editTextNewLine = "\r\n"
-- edittext cannot display more than 1024 characters per line
global g_editTextMaxLineLength = 1024 

global g_groupBoxWidth = g_animationManagerWidth * 0.90
global g_groupBoxHalfWidth = ( g_groupBoxWidth / 2 )
global g_groupBoxQuarterWidth = ( g_groupBoxWidth / 4 ) + 10
global g_groupBoxThirdWidth = ( g_groupBoxWidth / 3 ) - 5

global g_spinnerDefaultValue = 0

function CloseAnimationSelector = 
(
	if g_animationManagerFloater != undefined then 
	(
		closeRolloutFloater g_animationManagerFloater
	)
)

function PostResetCallback = 
(
	CloseAnimationSelector()
)

function PostNewCallback = 
(
	CloseAnimationSelector()
)

function PostOpenCallback = 
(
	CloseAnimationSelector()
)

-- need global refs for callbacks when this file is included in macroscript
global postResetCallbackRef = PostResetCallback
global postNewCallbackRef = PostNewCallback
global postOpenCallbackRef = PostOpenCallback

function toLowerCase str =
(
	upper="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	lower="abcdefghijklmnopqrstuvwxyz"

	for i = 1 to str.count do
	(
		notFound = true
		for j = 1 to upper.count while notFound do
		(
			if (str[i] == upper[j]) then
			(
				str[i] = lower[j]
				notFound = false
			)
		)
	)
	
	return str
)

function SetSpinnerRange _spinner minRange maxRange =
(
	_spinner.range = [  minRange, maxRange, _spinner.range.z ]
)

function SetAnimationRange _interval =
(
	if _interval.start >= _interval.end then
	(
		animationRange = interval _interval.start ( _interval.start + 1 )
	)
	else
	(
		animationRange = _interval
	)
)

function IsAllElementsEqual array =
(
	if array.count <= 0 then
	(
		return undefined
	)
	else if array.count == 1 do
	(
		return true
	)
	
	local bAllElementsEqual = true
	
	for elementId = 2 to array.count do
	(
		if array[1] != array[elementId] do
		(
			bAllElementsEqual = false
			break
		)
	)
	
	return bAllElementsEqual
)

function GetArrayOfSelectedValues baseArray indexArray = 
(
	local newArray = #()
	for index in indexArray do
	(
		append newArray baseArray[index]
	)
	return newArray
)

function GetAnimationClipNames = 
(
	local clipNames = #()
	local numAnimClips = CM_GetNumAnimationClips()
	for clipId = 1 to numAnimClips do
	(
		append clipNames ( CM_GetAnimationClipName clipId )
	)
	
	return clipNames
)

function GetAnimationClipStartTimes = 
(
	local clipStartTimes = #()
	local numAnimClips = CM_GetNumAnimationClips()
	for clipId = 1 to numAnimClips do
	(
		append clipStartTimes ( CM_GetAnimationClipStart clipId )
	)
	
	return clipStartTimes
)

function GetAnimationClipEndTimes = 
(
	local clipEndTimes = #()
	local numAnimClips = CM_GetNumAnimationClips()
	for clipId = 1 to numAnimClips do
	(
		append clipEndTimes ( CM_GetAnimationClipEnd clipId )
	)
	
	return clipEndTimes
)

function GetGroupIdByTime requestedEventGroupTime = 
(
	local requestedEventGroupId = -1
	local numEventGroups = CM_GetNumEventGroups()
	
	if numEventGroups != undefined then
	(
		for eventGroupId = 1 to numEventGroups do
		(
			local eventGroupTime = CM_GetEventGroupTime eventGroupId
			if eventGroupTime == requestedEventGroupTime then
			(
				requestedEventGroupId = eventGroupId
				break
			)
		)
	)
	
	return requestedEventGroupId
)

function GetEventGroupTimes = 
(
	local eventGroupTimes = #()
	local numEventGroups = CM_GetNumEventGroups()
	
	if numEventGroups != undefined then
	(
		for eventGroupId = 1 to numEventGroups do
		(
			local eventGroupTime = CM_GetEventGroupTime eventGroupId
			append eventGroupTimes eventGroupTime
		)
	)
	
	return eventGroupTimes
)

function GetSortedEventGroupTimes = 
(
	local eventGroupTimes = GetEventGroupTimes()
	sort eventGroupTimes
)

function isEndTag name =
(
	return matchPattern name pattern:"end_*"
)

function isAutoTag name =
(
	return matchPattern name pattern:"AUTO_*"
)

function GetCurrentAnimClipsSelected =
(
	return g_animationManagerFloater.rollouts[1].anim_sel.selection as array
)

function GetCurrentEventGroupsSelected = 
(
	return g_animationManagerFloater.rollouts[2].eventGroupList.selection as array
)

function ValidateAnimClipName animClipName &errorMsg: = 
(
	-- avoid adding anims with blank names
	if animClipName == "" do
	(
		if errorMsg != unsupplied then 
		(
			errorMsg = "Animation clips with blank names are not allowed"
		)
		return false
	)
			
	-- avoid adding names starting with the auto prefix
	-- auto anim clips are created by IrrColladaMax exporter
	if ( isAutoTag animClipName ) do
	(
		if errorMsg != unsupplied then 
		(
			errorMsg = "Animation clip names starting with 'AUTO_' are not allowed"
		)
		return false
	)
	
	-- avoid adding anims with the same name
	indx = CM_GetAnimationClipIndexByName animClipName
	if indx > 0 do
	(
		if errorMsg != unsupplied then 
		(
			errorMsg = "An animation clip of that name already exists"
		)
		return false
	)
	
	return true
)

function CleanAnimClips tagnames starttimes endtimes fromInfo:#() = 
(
	numAnimClips = tagnames.count
	if numAnimClips > 0 then
	(
		local oldNames = #()
		local newNames = #()
		local tmpNameArray = #()
		local nameConflictFromInfo = #()
		
		local oldtimes = #()
		local newtimes = #()
		local modifiedIntervalClipNames = #()
		local invalidTimeFromInfo = #()
		
		-- add to 'new' animation clips
		for i = 1 to numAnimClips do
		(
			local animClipName = tagnames[i]
			local starttime = starttimes[i]
			local endtime = endtimes[i]
			
			-- avoid name conflicts
			(
				local nameConflictPostfix = 1
				local index = 1
				while index >= 1 do
				(
					index = findItem tmpNameArray animClipName
					if index >= 1 then
					(
						animClipName = tagnames[i] + ( nameConflictPostfix as string )
						nameConflictPostfix = nameConflictPostfix + 1
					)
				)
				
				if animClipName != tagnames[i] do
				(
					append oldNames tagnames[i]
					append newNames animClipName
					if fromInfo.count >= i then
					(
						append nameConflictFromInfo fromInfo[i]
					)
				)
				
				append tmpNameArray animClipName
			)
			
			-- avoid invalid clip ranges
			(
				if ( endtime < starttime ) do
				(
					endtime = starttime
					
					local _oldInterval = interval 0 0
					_oldInterval.start = starttimes[i]
					_oldInterval.end = endtimes[i]
					append oldtimes _oldInterval
					
					append newtimes ( interval starttime endtime )
					append modifiedIntervalClipNames animClipName
					
					if fromInfo.count >= i then
					(
						append invalidTimeFromInfo fromInfo[i]
					)
				)
			)
			
			tagnames[i] = animClipName
			starttimes[i] = starttime
			endtimes[i] = endtime
		)
		
		-- alert user of automatic name changes
		if oldNames.count > 0 do
		(
		  local messageString = "The following animation clips have had their name changed automatically converted :\n"
			messageString = messageString + "\n"
			
			for i = 1 to oldNames.count do
			(
				messageString = messageString + oldNames[i]
				if nameConflictFromInfo.count >= i then 
				(
					messageString = messageString + " from " + nameConflictFromInfo[i];
				)
				messageString = messageString + " ==> " + newNames[i] + "\n"
			)
			
			MessageBox messageString title:"Backward Compatibility Warning"
		)
		
		-- alert user of automatic interval changes
		if oldtimes.count > 0 do
		(
		  local messageString = "The following animation clips have had their interval changed automatically :\n"
			messageString = messageString + "\n"
			
			for i = 1 to oldtimes.count do
			(
				messageString = messageString + modifiedIntervalClipNames[i] + " [ " + ( oldtimes[i].start as string ) + " , " + ( oldtimes[i].end as string ) + " ] "
				if invalidTimeFromInfo.count >= i then 
				(
					messageString = messageString + " from " + invalidTimeFromInfo[i];
				)
				messageString = messageString + " ==> " +  " [ " + ( newtimes[i].start as string ) + " , " + ( newtimes[i].end as string ) + " ] " + "\n"
			)
			
			MessageBox messageString title:"Backward Compatibility Warning"
		)		
	)
)

function ProvideBackwardCompatibility = 
(
	local tagnames = #()
	local starttimes = #()
	local endtimes = #()
	local nodeNames = #()
	local numDeprecatedAnimClips = 0
	
	-- get deprecated animation clips in note tracks
	for object in objects where hasNoteTracks object do
	(
		local objTagNames = #()
		local objStartTimes = #()
		local objEndTimes = #()
		local objNumDeprecatedAnimClips = 0
		
		local animnotes = getNoteTrack object 1
		local notekeys = animnotes.keys
		
		for n in notekeys where not isEndTag(n.value) do
		(
			append objTagNames n.value
		)
		
		objNumDeprecatedAnimClips = objTagNames.count
				
		for i = 1 to notekeys.count where not isEndTag(notekeys[i].value) do
		(
			append objStartTimes notekeys[i].time
		)
		
		objEndTimes.count = objNumDeprecatedAnimClips
		for i = 1 to objNumDeprecatedAnimClips do
		(
			local endtagname = "end_" + objTagNames[i]
			for n in notekeys do
			(
				if n.value == endtagname do objEndTimes[i] = n.time
 			)
		)
		
		for i = 1 to objTagNames.count do
		(
			objTagNames[i] = toLowerCase( objTagNames[i] )
		)
		
		join tagnames objTagNames
		join starttimes objStartTimes
		join endtimes objEndTimes
		numDeprecatedAnimClips = numDeprecatedAnimClips + objNumDeprecatedAnimClips
		
		for i = 1 to objNumDeprecatedAnimClips do
		(
			append nodeNames object.name
		)
	)
	
	-- add deprecated anim clips to 'new' animation clips if necessary
	numAnimClips = CM_GetNumAnimationClips()
	if numDeprecatedAnimClips > 0 and numAnimClips < 1 then
	(
		CleanAnimClips tagnames starttimes endtimes fromInfo:nodeNames

		for i = 1 to numDeprecatedAnimClips do
		(			
			CM_AddAnimationClip tagnames[i] starttimes[i] endtimes[i]
		)
	)
)

--------------------------------------------------------------------------------
-- Create/Edit Event Dialogs
--------------------------------------------------------------------------------

-- event dialog modes
-- global EVENT_DIALOG_MODE_CREATE = 0
-- global EVENT_DIALOG_MODE_EDIT = EVENT_DIALOG_MODE_CREATE + 1
-- global g_eventDialogMode = EVENT_DIALOG_MODE_CREATE
global g_eventDialogCurrentEventGroupId = -1 -- used to differentiate whether dialog is in create or edit mode
global g_eventDialogButtonWidth = ( g_groupBoxQuarterWidth + 15 )
global g_eventDialogEventListHeight = 80
global g_eventDialogConfirmButtonOffset = ([30,0])
global g_eventDialogSyntaxRulesButtonOffset = ([60,0])
global g_eventDialogDisallowedCharacters = #( ";" , "<" , ">" , "&" , "\\" , " " )

global g_eventDialogPreviousPosition = undefined
global g_eventDialogCreateEventSyncSpinnerCheckBoxState = undefined

rollout EventGroupRollout "Event Group"
(
	checkbox createEventSyncSpinnerCheckBox "Sync Spinner With Current Time" checked:true
	spinner createEventSpinner "Time" range:[g_minFrame,g_maxFrame,g_spinnerDefaultValue] type:#integer width:g_groupBoxQuarterWidth align:#left
	edittext eventList "Events :" height:g_eventDialogEventListHeight
	button syntaxRulesButton "Syntax Rules" align:#right across:3 offset:g_eventDialogSyntaxRulesButtonOffset
	button confirmButton "Ok" width:g_eventDialogButtonWidth align:#right  offset:g_eventDialogConfirmButtonOffset
	button cancelButton "Cancel" width:g_eventDialogButtonWidth align:#right

	-- utils
	function GetFormattedEvents eventText = 
	(
		local events = filterString eventText g_editTextNewLine
		
		-- remove disallowed characters
		local filteredEvents = #()
		for eventId = 1 to events.count do
		(
			local filteredEventString = events[eventId]
			for disallowedCharId = 1 to g_eventDialogDisallowedCharacters.count do
			(
				local splitString = filterString filteredEventString g_eventDialogDisallowedCharacters[disallowedCharId]
				local newEventString = ""
				for splitStringChunkId = 1 to splitString.count do
				(
					append newEventString splitString[splitStringChunkId]
				)
				filteredEventString = newEventString
			)
			
			-- truncate event strings that are too long
			filteredEventString = substring filteredEventString 1 g_editTextMaxLineLength
			
			if filteredEventString.count > 0 then
			(
				append filteredEvents filteredEventString
			)
		)
				
		return filteredEvents
	)
	
	function ConstructEventListString eventArray = 
	(
		local eventListString = ""
		for eventId = 1 to eventArray.count do
		(
			append eventListString ( eventArray[eventId] + g_editTextNewLine )
		)
		return eventListString
	)
	
	function ValidateEventGroupTime eventGroupTime = 
	(
		local eventGroupId = GetGroupIdByTime eventGroupTime
		return eventGroupId < 0 or g_eventDialogCurrentEventGroupId == eventGroupId
	)
		
	-- refresh funcs				
	function RefreshCreateEventGroupButton = 
	(	
		-- local eventGroupTime = createEventSpinner.value
		-- confirmButton.enabled = ValidateEventGroupTime eventGroupTime
	)
			
	function RefreshSpinnerFromSliderTime = 
	(
		-- if createEventSyncSpinnerCheckBox.state then
		-- (
			-- createEventSpinner.value = sliderTime
			-- RefreshCreateEventGroupButton()
		-- )
	)
	
  function RefreshSliderTimeFromSpinner = 
	(
		if createEventSyncSpinnerCheckBox.state then
		(
			local newAnimationRange = animationRange
			if createEventSpinner.value < animationRange.start then
			(
				newAnimationRange.start = createEventSpinner.value
			)
			else if createEventSpinner.value > animationRange.end then
			(
				newAnimationRange.end = createEventSpinner.value
			)
			
			animationRange = newAnimationRange
			sliderTime = createEventSpinner.value
		)
	)
	
	function Refresh = 
	(
		RefreshCreateEventGroupButton()
	)
	
	-- callbacks
	function CurrentTimeChangeCallback = 
	(
		-- RefreshSpinnerFromSliderTime()
	)
	
	-- Event Handlers
	function OnSpinnerValueChanged = 
	(
		RefreshSliderTimeFromSpinner()
		RefreshCreateEventGroupButton()
	)
	
	on createEventSyncSpinnerCheckBox changed bChecked do
	(
		g_eventDialogCreateEventSyncSpinnerCheckBoxState = bChecked
		if bChecked then
		(
			RefreshSliderTimeFromSpinner()
		)
		-- RefreshSpinnerFromSliderTime()
	)
	
	on createEventSpinner buttonup bCancel do
	(
		if bCancel == false then
		(
			OnSpinnerValueChanged()
		)
	)

	on createEventSpinner entered bUsedArrows bCancel do
	(
		if bUsedArrows == false and bCancel == false
			do
		(
			OnSpinnerValueChanged()
		)
	)
	
	on confirmButton pressed do 
	(
		local eventGroupTime = createEventSpinner.value
		local events = GetFormattedEvents eventList.text
		eventList.text = ConstructEventListString events

		-- validate
		local bValidEventGroup = true
		if not ( ValidateEventGroupTime eventGroupTime ) then
		(
			MessageBox ( "Event Group with time " + ( eventGroupTime as string ) + " already exists." ) title:"Invalid Event Group"
			bValidEventGroup = false
		)
		
		if bValidEventGroup and events.count < 1 then
		(
			MessageBox "At least one valid event must exist to edit/create an event group." title:"Invalid Event Group"
			bValidEventGroup = false
		)

		if bValidEventGroup then
		(
			local eventGroupId = -1		
			if g_eventDialogCurrentEventGroupId == -1 then
			(
				eventGroupId = CM_AddEventGroup eventGroupTime
			)
			else
			(
				eventGroupId = g_eventDialogCurrentEventGroupId
				CM_SetEventGroupTime eventGroupId eventGroupTime
				CM_DeleteAllEventGroupEvents eventGroupId
			)
			
			for eventId = 1 to events.count do
			(
				CM_AddEventGroupEvent eventGroupId events[eventId]
			)

			g_animationManagerFloater.rollouts[2].Refresh()
			DestroyDialog EventGroupRollout
		)
	)
	
	on cancelButton pressed do
	(
		DestroyDialog EventGroupRollout
	)
	
	on syntaxRulesButton pressed do 
	(
		local msg = ""
		
		append msg "The following characters will be automatically removed from the event strings : \n\n"
		for char in g_eventDialogDisallowedCharacters do
		(
			-- hack to display ampersant correctly in dialog box
			if char == "&" then
			(
				char = "&&"
			)

			append msg ( "\t" + "'" + char + "'" + "\n" )
		)
		append msg "\n"
		MessageBox msg title:"Syntax Rules"
	)
		
	on eventList entered NOTUSED do
	(
		local events = GetFormattedEvents eventList.text
		eventList.text = ConstructEventListString events
	)
			
	on EventGroupRollout close do
	(
		g_eventDialogPreviousPosition = GetDialogPos EventGroupRollout
	)
	
	on EventGroupRollout open do
	(
		-- set spinner range
		-- SetSpinnerRange createEventSpinner animationRange.start animationRange.end
		
		if g_eventDialogCurrentEventGroupId == -1 then
		(
			eventList.text = ""
			createEventSpinner.value = sliderTime
		)
		else
		(
			-- set event group time
			local eventGroupTime = CM_GetEventGroupTime g_eventDialogCurrentEventGroupId
			createEventSpinner.value = eventGroupTime
						
			-- set events
			local numEvents = CM_GetNumEventGroupEvents g_eventDialogCurrentEventGroupId
			local events = #()
			for eventId = 1 to numEvents do
			(
				append events ( CM_GetEventGroupEvent g_eventDialogCurrentEventGroupId eventId )
			)
			eventList.text = ConstructEventListString events
		)
		
		Refresh()
		setFocus createEventSpinner
		
		if g_eventDialogPreviousPosition != undefined then
		(
			SetDialogPos EventGroupRollout g_eventDialogPreviousPosition
		)		
		
		if g_eventDialogCreateEventSyncSpinnerCheckBoxState != undefined then
		(
			createEventSyncSpinnerCheckBox.state = g_eventDialogCreateEventSyncSpinnerCheckBoxState
		)
		
		RefreshSliderTimeFromSpinner()
		
		-- unRegisterTimeCallback EventGroupRollout.CurrentTimeChangeCallback
		-- registerTimeCallback EventGroupRollout.CurrentTimeChangeCallback 
	)	
)

function openAnimationSelector =
(
	global beginSpinnerOffset = [5,0]
	global endSpinnerOffset = [-45,0]	
	
	--------------------------------------------------------------------------------
	-- Animation Clips
	--------------------------------------------------------------------------------
	rollout Animation_Selector "Animation Selector" category:1
	(			
		group "Creation"
		(		
			checkbox createAnimClipLockSpinnersCheckBox "Sync Spinners With Animation Range" checked:true
			edittext createAnimClipNameEditText "Name :" align:#left
			spinner createAnimClipStartTimeSpinner "Begin :" range:[g_minFrame,g_maxFrame,g_spinnerDefaultValue] type:#integer align:#left across:2 width:g_groupBoxHalfWidth
			spinner createAnimClipEndTimeSpinner "End :" range:[g_minFrame,g_maxFrame,g_spinnerDefaultValue] type:#integer align:#left width:g_groupBoxHalfWidth
			button createNewClipButton "Create" width:g_groupBoxThirdWidth align:#left
			-- label dummy_00 "_"
		)
		
		-- widget groups
		group "Selection"
		(
			checkbox AnimOnSelect "Animate when selected"
			multilistbox anim_sel height:10
			button clearSelectionButton "Clear Selection" align:#left width:( g_groupBoxHalfWidth - 6 )across:2
			button selectAllButton "Select All" align:#left width:( g_groupBoxHalfWidth - 6 )
			button deleteButton "Delete Selected" align:#left width:( g_groupBoxHalfWidth - 6 ) across:2
			button createFromMixerButton "Create Clips From Mixer" align:#left width:( g_groupBoxHalfWidth - 6 )
		)
		
		group "Edition"
		(
			edittext editAnimClipNameEditText "Name :"
			spinner editAnimClipStartTimeSpinner "Begin :" range:[g_minFrame,g_maxFrame,g_spinnerDefaultValue] type:#integer align:#left width:g_groupBoxHalfWidth across:2
			spinner editAnimClipEndTimeSpinner "End :" range:[g_minFrame,g_maxFrame,g_spinnerDefaultValue] type:#integer align:#left width:g_groupBoxHalfWidth
		)
		
		-- subrollout testSub "testSub"
		
		-- utils		
		function ConstrainEditAnimClipSpinnerRanges _startSpinner _endSpinner = 
		(
			local currentSelectionSet = GetCurrentAnimClipsSelected()
			if currentSelectionSet.count > 0 do
			(
				local endTimesSelected = GetArrayOfSelectedValues ( GetAnimationClipEndTimes() ) currentSelectionSet
				local maxTime = amin endTimesSelected
				SetSpinnerRange _startSpinner _startSpinner.range.x ( maxTime )
				
				local startTimesSelected = GetArrayOfSelectedValues ( GetAnimationClipStartTimes() ) currentSelectionSet
				local minTime = amax startTimesSelected
				SetSpinnerRange _endSpinner ( minTime ) _endSpinner.range.y
			)
		)
		
		function GetCreateAnimClipStartSpinnerMax = 
		(
			local maxStart = createAnimClipEndTimeSpinner.value
			if createAnimClipLockSpinnersCheckBox.state then
			(
				maxStart = maxStart - 1
			)
			return maxStart
		)
		
		function GetCreateAnimClipEndSpinnerMin = 
		(
			local minEnd = createAnimClipStartTimeSpinner.value
			if createAnimClipLockSpinnersCheckBox.state then
			(
				minEnd = minEnd + 1
			)
			return minEnd
		)
		
		-- RefreshAnimationSelector funcs
		function RefreshEditAnimClipNameEditText = 
		(
			local currentSelectionSet = GetCurrentAnimClipsSelected()		
			local clipNames = GetAnimationClipNames()
			
			if currentSelectionSet.count <= 0 then
			(			
				editAnimClipNameEditText.text = ""
				editAnimClipNameEditText.readOnly = true
			)
			else if currentSelectionSet.count == 1 then
			(
				editAnimClipNameEditText.text = clipNames[currentSelectionSet[1]]
				editAnimClipNameEditText.readOnly = false				
			)
			else
			(
				editAnimClipNameEditText.text = "Multiple Clips Selected"
				editAnimClipNameEditText.readOnly = true
			)
		)
		
		function RefreshEditAnimClipSpinner _spinner timeArray = 
		(
			local currentSelectionSet = GetCurrentAnimClipsSelected()
			local selectedTimes = GetArrayOfSelectedValues timeArray currentSelectionSet
			local bAllElementsEqual = IsAllElementsEqual selectedTimes
								
			if currentSelectionSet.count <= 0 then
			(			
				_spinner.value = g_spinnerDefaultValue
				_spinner.Indeterminate = true
				_spinner.enabled = false
			)
			else if bAllElementsEqual then
			(
				_spinner.value = selectedTimes[1]
				_spinner.Indeterminate = false
				_spinner.enabled = true
			)
			else
			(
				_spinner.value = g_spinnerDefaultValue
				_spinner.Indeterminate = true
				_spinner.enabled = true
			)
		)
		
		function RefreshAnimationEditor = 
		(
			RefreshEditAnimClipNameEditText()
			RefreshEditAnimClipSpinner editAnimClipStartTimeSpinner ( GetAnimationClipStartTimes() )
			RefreshEditAnimClipSpinner editAnimClipEndTimeSpinner ( GetAnimationClipEndTimes() )
	
			ConstrainEditAnimClipSpinnerRanges editAnimClipStartTimeSpinner editAnimClipEndTimeSpinner
		)
		
		function DoPlayAnimation = 
		(
			local selectionSet = GetCurrentAnimClipsSelected()
			if selectionSet.count > 0 do 
			(
				if AnimOnSelect.checked then
				(
					playAnimation immediateReturn:true
				)
				else
				(
					stopAnimation()
				)
			)
		)
	
		function RefreshAnimationSelector =
		(			
			-- Get Animation Clips			
			local tagnames = GetAnimationClipNames()
			local starttimes = GetAnimationClipStartTimes()
			local endtimes = GetAnimationClipEndTimes()
		
			numAnimClips = CM_GetNumAnimationClips()
			for i = 1 to numAnimClips do
			(
				s = starttimes[i] as string
				e = endtimes[i] as string
				tagnames[i] = tagnames[i] + " [ " + s + " , " + e + " ]" 
			)
			
			-- put anims in listbox
			anim_sel.items = tagnames
			
			RefreshAnimationEditor()
		)

		function RefreshAnimationPlayback =
		(
			stopAnimation()
			selectionSet = GetCurrentAnimClipsSelected()
			if selectionSet.count > 0 do
			(							
				local endTimesSelected = GetArrayOfSelectedValues ( GetAnimationClipEndTimes() ) selectionSet
				local maxTime = amax endTimesSelected
			
				local startTimesSelected = GetArrayOfSelectedValues ( GetAnimationClipStartTimes() ) selectionSet
				local minTime = amin startTimesSelected
								
				SetAnimationRange ( interval minTime maxTime )
				slidertime = minTime
				
				DoPlayAnimation()
			)
		)
			
		-- Event handlers
		on anim_sel selectionEnd do
		(
			RefreshAnimationPlayback()
			RefreshAnimationEditor()
		)
		
		function RefreshAnimationClipTimeFromSpinner _spinner setTimeFunc =
		(
			local currentSelectionSet = GetCurrentAnimClipsSelected()
			for clipId in currentSelectionSet do
			(
				setTimeFunc clipId _spinner.value
			)
			RefreshAnimationPlayback()
			RefreshAnimationSelector()
		)
		
		on clearSelectionButton pressed do 
		(
			anim_sel.selection = #{}
			stopAnimation()
			RefreshAnimationEditor()
		)
				
		on selectAllButton pressed do
		(
			anim_sel.selection = #{1..anim_sel.items.count}
			anim_sel.selectionEnd()
		)
		
		function RefreshAnimationRangeFromCreateAnimClipSpinners = 
		(
			if createAnimClipLockSpinnersCheckBox.state then
			(
				clearSelectionButton.pressed()
				SetAnimationRange ( interval createAnimClipStartTimeSpinner.value createAnimClipEndTimeSpinner.value )
			)			
		)
		
		function RefreshCreateAnimClipSpinnersFromAnimationRange =
		(
			if createAnimClipLockSpinnersCheckBox.state then
			(
				createAnimClipStartTimeSpinner.value = animationRange.start
				createAnimClipEndTimeSpinner.value = animationRange.end
			)
			
			local maxStart = GetCreateAnimClipStartSpinnerMax()
			local minEnd = GetCreateAnimClipEndSpinnerMin()
			
			SetSpinnerRange createAnimClipStartTimeSpinner createAnimClipStartTimeSpinner.range.x maxStart		
			SetSpinnerRange createAnimClipEndTimeSpinner minEnd createAnimClipEndTimeSpinner.range.y
		)
		
		on deleteButton pressed do
		(
			stopAnimation()
			
			selections = anim_sel.selection as array
			animClipNames = GetAnimationClipNames()
			for i = 1 to selections.count do
			(
				CM_DeleteAnimationClipByName animClipNames[selections[i]]
			)
							
			RefreshAnimationSelector()
		)

		on createNewClipButton pressed do
		(
			local animName = toLowerCase createAnimClipNameEditText.text
			local bValid = false
			local errorMsg = ""
			
			bValid = ValidateAnimClipName animName errorMsg:&errorMsg
			if bValid then
			(
				-- create start key
				CM_AddAnimationClip animName createAnimClipStartTimeSpinner.value createAnimClipEndTimeSpinner.value
				RefreshAnimationSelector()
				anim_sel.selection = #{anim_sel.items.count}
				anim_sel.selectionEnd()
			)
			else
			(
				messagebox errorMsg title:"Invalid Animation Clip"
			)
		)
		
		on createAnimClipLockSpinnersCheckBox changed bChecked do
		(
			RefreshCreateAnimClipSpinnersFromAnimationRange()
		)
		
		on AnimOnSelect changed bChecked do
		(
			if bChecked then
			(
				RefreshAnimationPlayback()
			)
		)
		
		on createFromMixerButton pressed do
		(
			local bip01 = $Bip01
			
			if bip01 == undefined do
			(
				return false
			)
			
			controller = bip01.transform.controller
			mixer = controller.mixer
			
			if ( mixer == undefined) do
			(
				messagebox "The biped must have a mixer on it"
				return false
			)
			
			trackgroup = getTrackgroup mixer 1
			if trackgroup == false do
			(
				messagebox "First track group does not exist in mixer."
				return false
			)
			
			track = getTrack trackgroup 1
			if ( track == undefined) do
			(
				messagebox "No track defined in the mixer"
				return false
			)
			
			if track.numClips < 1 do
			(
				messagebox "Track in mixer has no clips"
				return false
			)
			
			local tagnames = #()
			local starttimes = #()
			local endtimes = #()
			local filenames = #()
			
			for i = 1 to track.numClips do
			(
				clip = getclip track i
				
				append tagnames ( toLowerCase ( getFilenameFile clip.filename ) )
				append starttimes clip.globstart
				append endtimes clip.globend
				append filenames clip.filename
			)
			
			CleanAnimClips tagnames starttimes endtimes fromInfo:filenames
			
			-- Add only valid clips
			local errorMessages = #()
			for i = 1 to tagnames.count do
			(
				local errorMsg = ""
				if ( ValidateAnimClipName tagnames[i] errorMsg:&errorMsg ) == true then
				(
					CM_AddAnimationClip tagnames[i] starttimes[i] endtimes[i]
				)
				else
				(
					local fullErrorMsg = tagnames[i] + " ==> "
					fullErrorMsg = fullErrorMsg + errorMsg
					append errorMessages fullErrorMsg
				)
			)
			
			if errorMessages.count > 0 do
			(
				local messageString = "The following animation clips could not be added : \n"
				messageString = messageString + "\n"
				
				for i = 1 to errorMessages.count do
				(
					messageString = messageString + errorMessages[i] + "\n"
				)
				
				MessageBox messageString title:"Invalid Animation Clips Warning"
			)
			
			RefreshAnimationSelector()
		)
		
		function OnCreateSpinnerButtonUp _spinner minRange maxRange bCancel = 
		(
			if bCancel == false then
			(
				SetSpinnerRange _spinner minRange maxRange
				RefreshAnimationRangeFromCreateAnimClipSpinners()
			)
		)
		
		function OnCreateSpinnerEntered _spinner minRange maxRange bUsedArrows bCancel = 
		(
			if bUsedArrows == false and bCancel == false
				do
			(
				SetSpinnerRange _spinner minRange maxRange
				RefreshAnimationRangeFromCreateAnimClipSpinners()
			)
		)
		
		on createAnimClipEndTimeSpinner buttonup bCancel do
		(
			local maxStart = GetCreateAnimClipStartSpinnerMax()			
			OnCreateSpinnerButtonUp createAnimClipStartTimeSpinner createAnimClipStartTimeSpinner.range.x maxStart bCancel
		)
		
		on createAnimClipEndTimeSpinner entered bUsedArrows bCancel do
		(
			local maxStart = GetCreateAnimClipStartSpinnerMax()
			OnCreateSpinnerEntered createAnimClipStartTimeSpinner createAnimClipStartTimeSpinner.range.x maxStart bUsedArrows bCancel
		)
		
		on createAnimClipStartTimeSpinner buttonup bCancel do
		(
			local minEnd = GetCreateAnimClipEndSpinnerMin()
			OnCreateSpinnerButtonUp createAnimClipEndTimeSpinner minEnd createAnimClipEndTimeSpinner.range.y bCancel
		)
		
		on createAnimClipStartTimeSpinner entered bUsedArrows bCancel do
		(
			local minEnd = GetCreateAnimClipEndSpinnerMin()
			OnCreateSpinnerEntered createAnimClipEndTimeSpinner minEnd createAnimClipEndTimeSpinner.range.y bUsedArrows bCancel
		)

		on editAnimClipNameEditText entered txt do
		(
			local newAnimClipName = toLowerCase( txt )
			local currentSelectionSet = GetCurrentAnimClipsSelected()
			local currentAnimClipName = CM_GetAnimationClipName ( currentSelectionSet[1] )
			if ( stricmp newAnimClipName currentAnimClipName ) != 0 do
			(
				local errorMsg = ""
				local bValid = ValidateAnimClipName newAnimClipName errorMsg:&errorMsg
				if bValid then
				(
					local animClipId = CM_GetAnimationClipIndexByName currentAnimClipName
					CM_SetAnimationClipName animClipId newAnimClipName
					RefreshAnimationSelector()
				)
				else
				(
					messagebox errorMsg title:"Invalid Animation Clip"
					RefreshAnimationEditor()
				)
			)
		)
	
		function OnEditSpinnerButtonDown _spinner =
		(
			_spinner.Indeterminate = false
		)
		
		function OnEditSpinnerButtonUp _spinner getTimesFunc setTimeFunc bCancel = 
		(
			if bCancel == false then
			(
				RefreshAnimationClipTimeFromSpinner _spinner setTimeFunc
			)
			else
			(
				local currentSelectionSet = GetCurrentAnimClipsSelected()
				local selectedTimes = GetArrayOfSelectedValues ( getTimesFunc() ) currentSelectionSet
				local bAllElementsEqual = IsAllElementsEqual selectedTimes
				if selectedTimes.count > 1 and bAllElementsEqual == false do
				(
					_spinner.Indeterminate = true
				)
			)
		)
		
		function OnEditSpinnerEntered _spinner setTimeFunc bUsedArrows bCancel = 
		(
			if bUsedArrows == false and bCancel == false
				do
			(
				RefreshAnimationClipTimeFromSpinner _spinner setTimeFunc
			)
		)
		
		on editAnimClipStartTimeSpinner buttondown do
		(
			OnEditSpinnerButtonDown editAnimClipStartTimeSpinner
		)
		
		on editAnimClipStartTimeSpinner buttonup bCancel do
		(
			OnEditSpinnerButtonUp editAnimClipStartTimeSpinner GetAnimationClipStartTimes CM_SetAnimationClipStart bCancel
		)
		
		on editAnimClipStartTimeSpinner entered bUsedArrows bCancel do
		(
			OnEditSpinnerEntered editAnimClipStartTimeSpinner CM_SetAnimationClipStart bUsedArrows bCancel
		)
		
		on editAnimClipEndTimeSpinner buttondown do
		(
			OnEditSpinnerButtonDown editAnimClipEndTimeSpinner
		)
		
		on editAnimClipEndTimeSpinner buttonup bCancel do
		(
			OnEditSpinnerButtonUp editAnimClipEndTimeSpinner GetAnimationClipEndTimes CM_SetAnimationClipEnd bCancel
		)
		
		on editAnimClipEndTimeSpinner entered bUsedArrows bCancel do
		(
			OnEditSpinnerEntered editAnimClipEndTimeSpinner CM_SetAnimationClipEnd bUsedArrows bCancel
		)
		
		on Animation_Selector open do
		(
			RefreshAnimationSelector()
			RefreshCreateAnimClipSpinnersFromAnimationRange()
		)

		-- callbacks
		function SetCreateClipAnimationRangeCallback = 
		(
			RefreshCreateAnimClipSpinnersFromAnimationRange()
		)
	)
	
	--------------------------------------------------------------------------------
	-- Events
	--------------------------------------------------------------------------------
	rollout Animation_Events "Events"
	(
		-- controls
		multilistbox eventGroupList height:10
		button clearSelectionButton "Clear Selection" align:#left width:g_groupBoxThirdWidth across:3
		button selectAllButton "Select All" align:#left width:g_groupBoxThirdWidth
		button deleteButton "Delete Selected" align:#left width:g_groupBoxThirdWidth
					
		function RefreshEventGroups = 
		(
			local eventGroupsSelectionSet = GetCurrentEventGroupsSelected()
			local eventGroupStrings = #()

			local eventGroupTimes = GetSortedEventGroupTimes()
			for eventGroupTimeId = 1 to eventGroupTimes.count do
			(
				local eventGroupTime = eventGroupTimes[eventGroupTimeId]
				local eventGroupId = GetGroupIdByTime eventGroupTime
				
				local eventGroupString = "[ " + ( eventGroupTime as string ) + " ]"
			
				local numEvents = CM_GetNumEventGroupEvents eventGroupId
				if numEvents > 0 then
				(
					eventGroupString = eventGroupString + "   "
				)
				
				for eventId = 1 to numEvents do
				(
					local _event = CM_GetEventGroupEvent eventGroupId eventId
					eventGroupString = eventGroupString + _event
					if eventId < numEvents then
					(
						eventGroupString = eventGroupString + " ; "
					)
				)
				
				append eventGroupStrings eventGroupString
			)
			
			append eventGroupStrings "New"
			eventGroupList.items = eventGroupStrings
		)
		
		function Refresh = 
		(
			RefreshEventGroups()
		)
		
		-- event handlers
		on Animation_Events open do
		(
			Refresh()
		)
		
		on eventGroupList doubleClicked elementIndex do
		(
			stopAnimation()
			
			if elementIndex == eventGroupList.items.count then
			(
				g_eventDialogCurrentEventGroupId = -1
			)
			else
			(
				local eventGroupTimes = GetSortedEventGroupTimes()
				local eventGroupTime = eventGroupTimes[elementIndex]
				g_eventDialogCurrentEventGroupId = GetGroupIdByTime eventGroupTime
			)
			
			local dialogStyle = #(#style_titlebar, #style_border, #style_sysmenu)
			CreateDialog EventGroupRollout style:dialogStyle modal:true escapeEnable:false width:400
		)
		
		on eventGroupList selectionEnd do
		(
			local eventGroupsSelectionSet = GetCurrentEventGroupsSelected()
			if eventGroupsSelectionSet.count > 0 then
			(
				local topEventGroupSelected  = eventGroupsSelectionSet[1]
				if topEventGroupSelected != eventGroupList.items.count then
				(
					local eventGroupTimes = GetSortedEventGroupTimes()
					local eventGroupTime = eventGroupTimes[topEventGroupSelected]
				)
			)
		)
		
		on clearSelectionButton pressed do 
		(
			eventGroupList.selection = #{}
		)
				
		on selectAllButton pressed do
		(
			eventGroupList.selection = #{1..eventGroupList.items.count}
		)
		
		on deleteButton pressed do
		(			
			local eventGroupsSelectionSet = GetCurrentEventGroupsSelected()
			
			if eventGroupsSelectionSet.count > 0 then
			(
				local eventGroupTimes = GetSortedEventGroupTimes()
				
				for eventGroupSelectionId = 1 to eventGroupsSelectionSet.count do
				(
					local eventGroupTimeId = eventGroupsSelectionSet[eventGroupSelectionId]
					if eventGroupTimeId != eventGroupList.items.count then
					(
						local eventGroupTime = eventGroupTimes[eventGroupTimeId]
						local eventGroupId = GetGroupIdByTime eventGroupTime
						if eventGroupId != -1 then
						(
							CM_DeleteEventGroup eventGroupId
						)
					)
				)
			)
			
			Refresh()
		)
	)
	
	-- close old rollout if already opened
	CloseAnimationSelector()
	
	-- add new Floater
	g_animationManagerFloater = newRolloutFloater "Animation Selector"  g_animationManagerWidth ( g_animationManagerHeight + 100 )
	addRollout Animation_Selector g_animationManagerFloater 
	AddRollout Animation_Events g_animationManagerFloater
	
	-- callbacks
	callbacks.removeScripts #animationRangeChange id:#AnimationRangeCallbackId
	callbacks.addScript #animationRangeChange "g_animationManagerFloater.rollouts[1].SetCreateClipAnimationRangeCallback()" id:#AnimationRangeCallbackId
		
	callbacks.removeScripts #systemPostReset id:#AnimationSelectorPostResetCallbackId
	callbacks.addScript #systemPostReset "postResetCallbackRef()" id:#AnimationSelectorPostResetCallbackId
	
	callbacks.removeScripts #systemPostNew id:#AnimationSelectorPostNewCallbackId
	callbacks.addScript #systemPostNew "postNewCallbackRef()" id:#AnimationSelectorPostNewCallbackId
	
	callbacks.removeScripts #filePostOpen id:#AnimationSelectorPostOpenCallbackId
	callbacks.addScript #filePostOpen "postOpenCallbackRef()" id:#AnimationSelectorPostOpenCallbackId
)

-- main
ProvideBackwardCompatibility()
openAnimationSelector()