//------------------------------------
// Global variables
//------------------------------------

// Window size
windowWidth = 0
windowHeight = 0
bannerHeight = 0
toolbarHeight = 0

// Field types
TYPE_ENTRY = 1
TYPE_TEXTAREA = 2

// Field parts
FIELD_DESC = 0
FIELD_TYPE = 1
FIELD_USED = 2

// Stock Icons
STOCK_NEW = "images/new.gif"
STOCK_SAVE = "images/save.gif"
STOCK_LINK = "images/link.gif"
STOCK_FIELD = "images/field.gif"
STOCK_NOTE = "images/note.gif"
STOCK_REF = "images/reference.gif"

// General stuff
var lessonPlan
var newBox
var addField
var focusCounter = 0
var firstBox = true

// Fields and their descriptions
// Required fields
requiredFields = new Array ()
requiredFields["Title"] = [
	"The title associated with this box",
	TYPE_ENTRY]

requiredFields["Author"] = [
	"The creator(s) of this box",
	TYPE_ENTRY]

requiredFields["Required Time"] = [
	"The amount of time required to teach this lesson",
	TYPE_ENTRY]

requiredFields["About"] = [
	"A brief description of the box",
    TYPE_TEXTAREA]

requiredFields["Standards"] = [
	"The school standards covered by this box",
	TYPE_TEXTAREA]

// Optional fields.  These have three fields:
//    Description - Description of the field
//    Type - Type of field to create
//    Used - Whether it has been used or not
optionalFields = new Array ()
optionalFields["Introduction"] = [
	"Introduce the topic",
	TYPE_TEXTAREA,
	false]

optionalFields["Materials"] = [
	"Materials (such as scissors or glue) that are required",
	TYPE_TEXTAREA,
	false]

optionalFields["Detailed Schedule"] = [
	"Provide a detailed schedule for the lesson",
	TYPE_TEXTAREA,
	false]

optionalFields["Prerequisites"] = [
	"The concepts that students should bring in to this lesson",
	TYPE_TEXTAREA,
	false]

optionalFields["Instructions (Methods)"] = [
	"How the lesson is to be taught",
	TYPE_TEXTAREA,
	false]

//------------------------------------
// Class LessonPlan
//------------------------------------

// Constructor for the LessonPlan object
function LessonPlan ()
{
	// Members
	this.fields  = new Array ()
	this.mainDiv = document.getElementById ('mainDiv')
	this.toolbar = top.document.getElementById ('toolbar')

	// Methods
	this.addField     = addField
	this.setupToolbar = setupToolbar

	// Title
	title = document.createElement ("h1")
	text = document.createTextNode ("New Plan")
	title.appendChild (text)
	this.mainDiv.appendChild (title)

	// Toolbar
	this.setupToolbar (this)

	// Fields
	for (item in requiredFields)
	{
		this.addField (item, requiredFields[item], false, false)
	}
}

function borderSwitch (e)
{
	tg = e.target
	while (!tg.highlightable)
	{
		if (!tg.parentNode)
			return

		tg = tg.parentNode
	}

	tmp = tg.style.backgroundColor
	tg.style.backgroundColor = tg.activeColor
	tg.activeColor = tmp
}

function showToolTip (x, y, counter, tip)
{
	if (tipVisible || counter != currentFocus)
		return

	tipVisible = true
	div = document.createElement ("div")
	div.setAttribute ("id", "tooltip")
	div.style.backgroundColor = "#eeeeee"
	div.style.border = "thin solid black"
	div.style.padding = "5px"
	div.appendChild (document.createTextNode (tip))
	div.style.position = "absolute"
	div.style.top = "" + (y + 10) + "px"
	div.style.left = "" + (x + 10) + "px"
	top.document.body.appendChild (div)
}

function backgroundIn (e)
{
	tg = e.target
	while (!tg.highlightable)
	{
		if (!tg.parentNode)
			return

		tg = tg.parentNode
	}

	tg.style.borderLeft = tg.style.borderTop = "1px solid #5555aa"
	tg.style.borderRight = tg.style.borderBottom = "1px solid #5555aa"
	tg.style.backgroundColor = tg.sensitiveColor

	if (!tg.tipVisible)
	{
		focusCounter ++
		currentFocus = focusCounter
		tg.tipVisible = true
		x = e.clientX
		y = e.clientY
		setTimeout ("showToolTip (" + x + ", " + y + ", " + focusCounter + ", \"" + tg.tip + "\");", 500)
	}
}

function backgroundOut (e)
{
	tg = e.target
	while (!tg.highlightable)
	{
		if (!tg.parentNode)
			return
		tg = tg.parentNode
	}

	tg.style.borderLeft = tg.style.borderTop = "1px solid #cccccc"
	tg.style.borderRight = tg.style.borderBottom = "1px solid #cccccc"
	tg.style.backgroundColor = "#cccccc"
	tg.focused = false

	if (tg.tipVisible)
	{
		currentFocus = -1
		tg.tipVisible = false
		tipVisible = false
		div = top.document.getElementById ("tooltip")
		if (div)
			top.document.body.removeChild (div)
	}
}

function createButton (mytr, name, tip, icon, onClick)
{
	mytd = document.createElement ("td")
	mytr.appendChild (mytd)

	table = document.createElement ("table")
	mytd.appendChild (table)

	tr = document.createElement ("tr")
	table.appendChild (tr)

	td = document.createElement ("td")
	tr.appendChild (td)
	img = document.createElement ("img")
	td.appendChild (img)
	img.setAttribute ("src", icon)

	td1 = document.createElement ("td")
	tr.appendChild (td1)
	td1.appendChild (document.createTextNode (name))

	mytd.style.cursor = "default"
	mytd.setAttribute ('valign', "middle")
	mytd.style.borderLeft = mytd.style.borderTop = "1px solid #cccccc"
	mytd.style.borderRight = mytd.style.borderBottom = "1px solid #cccccc"
	mytd.sensitiveColor = "#aaaaee"
	mytd.activeColor = "#8888bb"
	mytd.onmouseover = backgroundIn
	mytd.onmouseout = backgroundOut
	mytd.onmousedown = borderSwitch
	mytd.onmouseup = borderSwitch
	mytd.onclick = onClick
	mytd.highlightable = true
	mytd.tip = tip
	return mytr
}

function setupToolbar (plan)
{
	tab = document.createElement ("table")
	tr = document.createElement ("tr")

	tr = createButton (tr,
		"New Lesson",
		"Create a new lesson and add it to the box.",
		STOCK_NEW);

	tr = createButton (tr,
		"Save Lesson",
		"Save the changes to the current lesson.",
		STOCK_SAVE);

	tr = createButton (tr,
		"Create Link",
		"Add a link from another box or lesson to here.",
		STOCK_LINK);

	tr = createButton (tr,
		"Add Field",
		"Add additional information to this lesson.",
		STOCK_FIELD,
		function (e) {addField = new AddFieldDlg ()})

	tr = createButton (tr,
		"Add Resource",
		"Upload a file into your box, or create a link to an external web resource.",
		STOCK_REF)

	tr = createButton (tr,
		"Add Note",
		"Add a personal note or reminder to the lesson.",
		STOCK_NOTE)

	tab.appendChild (tr)
	plan.toolbar.appendChild (tab)
}

// Add a field to the lesson plan
function addField (name, field, removable, doMceReplace)
{
	desc = field[FIELD_DESC]
	type = field[FIELD_TYPE]

	newField = createField (name, desc, type, removable)
	this.fields[newField.id] = newField
	this.mainDiv.appendChild (newField)

	id = newField.id

	return newField
}

// Create a field and return it
function createField (name, desc, type, removable)
{
	// Create the title of the field
	title = document.createElement ("span")
	text  = document.createTextNode (name)

	title.setAttribute ("class", "fieldName")
	title.appendChild (text)

	date = new Date ()
	id = new String (date.getTime ())

	if (removable) {
		rm = document.createElement ("span")
		rm.setAttribute ("class", "removeButton")
		rm.setAttribute ("id", id)
		rm.appendChild (document.createTextNode ("Remove"))
		rm.style.cursor = "pointer"
		rm.onclick = removeField
	}

	// Create the description tag
	descTag = document.createElement ("div")
	descTag.setAttribute ("class", "descriptionField")
	descTag.appendChild (document.createTextNode (desc))

	// Determine which type of field we want
	switch (type)
	{
		case TYPE_ENTRY:
			entry = document.createElement ("input")
			entry.setAttribute ("type", "text")
			entry.style.width = "90%"
			entry.style.margin = "20px"
			entry.style.border = "thin solid #cccccc"
			break
		case TYPE_TEXTAREA:
			entry = document.createElement ("div")
			entry.style.width = "90%"
			entry.style.margin = "20px"

			textarea = document.createElement ("textarea")
			entry.appendChild (textarea)
			textarea.setAttribute ("rows", 10)
			textarea.setAttribute ("id", "mceReplaceMe" + id)
			textarea.style.width = "100%"
			break
		default:
			entry = document.createElement ("textarea")
			break
	}

	// Package it all up and return it
	div = document.createElement ("div")
	div.setAttribute ("id", id)
	div.style.marginTop = "20px"
	div.appendChild (title)
	if (removable) {
		div.appendChild (rm)
	}
	div.appendChild (document.createElement ("br"))
	div.appendChild (descTag)
	div.appendChild (entry)
	div.fieldName = name

	return div
}

function removeField (event)
{
	if (confirm ("Are you sure you want to remove this field?")) {
		var id
		var mainDiv = document.getElementById ("mainDiv")

		if (event.target) {
			id = event.target.id
		} else {
			id = event.srcElement.id
		}

		fieldName = lessonPlan.fields[id].fieldName
		if (optionalFields[fieldName])
			optionalFields[fieldName][FIELD_USED] = false

		mainDiv.removeChild (lessonPlan.fields[id])
		delete lessonPlan.fields[id]
	}
}

//-----------------------------
// Class NewBox
//-----------------------------

// Create a new dialog
function NewBox ()
{
	// Members
	this.div = top.document.getElementById ('new_box')
	this.select = top.document.getElementById ('fieldsSelect')
	this.createButton = top.document.getElementById ('newCreate')
	this.cancelButton = top.document.getElementById ('newCancel')
	this.options = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = newBoxSelectionChanged
	this.createButtonClicked = newBoxCreateButtonClicked
	this.cancelButtonClicked = newBoxCancelButtonClicked

	// Add the stuff to the box
	for (field in optionalFields)
	{
		if (optionalFields[field][FIELD_USED])
			continue

		div = document.createElement ("div")
		tbl = document.createElement ("table")
		div.appendChild (tbl)

		tr = document.createElement ("tr")
		tr.style.margin = "0px"
		tr.style.border = "0px"
		tr.style.padding = "0px"
		tbl.style.margin = "0px"
		tbl.style.border = "0px"
		tbl.style.padding = "0px"

		tbl.appendChild (tr)

		td = document.createElement ("td")
		td.setAttribute ("valign", "middle")
		tr.appendChild (td)

		optionCheck = document.createElement("input")
		optionCheck.type = "checkbox"
		optionCheck.style.margin = "0px"
		optionCheck.checked = false
		optionCheck.fieldName = field
		td.appendChild (optionCheck)

		td = document.createElement ("td")
		td.setAttribute ("valign", "middle")
		tr.appendChild (td)

		optionText = document.createTextNode (field)
		td.appendChild (optionText)

		div.fieldName = field
		div.myCheck = optionCheck

		optionCheck.parentDiv = div

		this.options.push (div)
		this.select.appendChild (div)
	}

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
	this.createButton.onclick = this.createButtonClicked
	this.cancelButton.onclick = this.cancelButtonClicked

	// Make the dialog visible
	this.div.style.display = "block"
	this.div.style.position = "absolute"
	this.div.style.left = (windowWidth / 2) - (this.div.clientWidth / 2)
	this.div.style.top = (windowHeight / 2) - (this.div.clientHeight / 2) + bannerHeight
	this.div.style.visibility = "visible"
	this.div.style.cursor = "default"
}

// Selection changed handler
function newBoxSelectionChanged (event)
{
	parentDiv = newBox.div
	select = newBox.select
	div = top.document.getElementById ('description')

	// Prepare the description text
	boldText = document.createElement ("b")
	boldLabel = document.createTextNode ("Description: ")
	boldText.appendChild (boldLabel)

	// Figure out what responded to the event
	if (event.target)
	{
		target = event.target
		parent = target.parentDiv
	}
	else if (event.srcElement)
	{
		target = event.srcElement
		parent = element.srcElement.parentDiv
	}

	while (target.tagName == "TD" || target.tagName == "TR" || target.tagName == "TABLE")
	{
		target = target.parentNode
	}

	// Clear the current selection
	for (option in parentDiv.options)
		parentDiv.options[option].style.background = "#ffffff"

	// Grab the description
	if (target.fieldName)
		text = optionalFields[target.fieldName][FIELD_DESC]
	else
		text = ""

	// Set the selection color
	if (text != "")
		if (parent)
			parent.style.background = "#7777ff"
		else
			target.style.background = "#7777ff"

	// Setup the div
	descriptText = document.createTextNode (text)
	div.innerHTML = ""
	div.appendChild (boldText)
	div.appendChild (descriptText)
}

// Create button clicked
function newBoxCreateButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"

	// Figure out which fields were selected and add them to the view
	options = div.options
	objectFields = new Array ()
	var firstField = null

	for (i in options)
	{
		if (options[i].myCheck.checked)
		{
			field = options[i].fieldName
			objField = lessonPlan.addField (field, optionalFields[field], true, true)
			optionalFields[field][FIELD_USED] = true;
			objectFields.push (objField.id)

			// Create a second copy of this box to work around a bug in rendering the
			// first dynamically generated text box
			if (!firstField)
			{
				firstField = objField
				objField2 = lessonPlan.addField (field, optionalFields[field], true, true)
				objectFields.push (objField2.id)
			}
		}
	}

	for (id in objectFields)
	{
		objId = objectFields[id]

		tinyMCE.execCommand ("mceAddControl", true, "mceReplaceMe" + objId)

		obj = document.getElementById ("mceReplaceMe" + objId)
		obj.style.margin = "20px"
		obj.style.width = "90%"
	}

	// Delete the mis-rendered box.  Note that this has to be done in a timeout
	// loop due to a bug in Firefox that would cause a crash if done immediately.
	deleteBox = function ()
	{
		var mainDiv = document.getElementById ("mainDiv")

		mainDiv.removeChild (lessonPlan.fields[firstField.id])
		delete lessonPlan.fields[firstField.id]
		firstBox = false
	}

	if (firstBox)
		setTimeout (deleteBox, 100)
}

// Cancel button clicked
function newBoxCancelButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"
	history.back ()
}

//----------------------------
// AddFieldDLG Class
//----------------------------
function AddFieldDlg ()
{
	// Members
	this.div = top.document.getElementById ('add_field_box')
	this.select = top.document.getElementById ('addfieldsSelect')
	this.createButton = top.document.getElementById ('addFieldCreate')
	this.cancelButton = top.document.getElementById ('addFieldCancel')
	this.options = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = addFieldSelectionChanged
	this.createButtonClicked = addFieldCreateButtonClicked
	this.cancelButtonClicked = addFieldCancelButtonClicked

	// Clear the old DLG stuff out
	this.select.innerHTML = ""

	// Add the stuff to the box
	for (field in optionalFields)
	{
		if (optionalFields[field][FIELD_USED])
			continue

		div = document.createElement ("div")
		div.appendChild (document.createTextNode (field))
		div.fieldName = field
		div.selected = false
		div.style.padding = "3px"
		this.options.push (div)
		this.select.appendChild (div)
	}

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
	this.createButton.onclick = this.createButtonClicked
	this.cancelButton.onclick = this.cancelButtonClicked

	// Make the dialog visible
	this.div.style.display = "block"
	this.div.style.position = "absolute"
	this.div.style.left = (windowWidth / 2) - (this.div.clientWidth / 2)
	this.div.style.top = (windowHeight / 2) - (this.div.clientHeight / 2) + bannerHeight
	this.div.style.visibility = "visible"
	this.div.style.cursor = "default"
}

// Selection changed handler
function addFieldSelectionChanged (event)
{
	parentDiv = addField.div
	select = addField.select
	div = top.document.getElementById ('addFieldsdescription')

	// Prepare the description text
	boldText = document.createElement ("b")
	boldLabel = document.createTextNode ("Description: ")
	boldText.appendChild (boldLabel)

	// Figure out what responded to the event
	if (event.target)
	{
		target = event.target
		parent = target.parentDiv
	}
	else if (event.srcElement)
	{
		target = event.srcElement
		parent = element.srcElement.parentDiv
	}

	// Clear the current selection
	for (option in parentDiv.options)
	{
		parentDiv.options[option].style.background = "#ffffff"
		parentDiv.options[option].selected = false
	}

	// Grab the description
	if (target.fieldName)
		text = optionalFields[target.fieldName][FIELD_DESC]
	else
		text = ""

	// Set the selection color
	if (text != "")
		if (parent)
		{
			parent.style.background = "#7777ff"
			parent.selected = true
		}
		else
		{
			target.style.background = "#7777ff"
			target.selected = true
		}

	// Setup the div
	descriptText = document.createTextNode (text)
	div.innerHTML = ""
	div.appendChild (boldText)
	div.appendChild (descriptText)
}

// Create button clicked
function addFieldCreateButtonClicked ()
{
	div = addField.div
	div.style.visibility = "hidden"

	// Figure out which fields were selected and add them to the view
	options = div.options
	objectFields = new Array ()

	for (i in options)
	{
		if (options[i].selected)
		{
			field = div.options[i].fieldName
			objField = lessonPlan.addField (field, optionalFields[field], true, true)
			objectFields.push (objField.id)
			optionalFields[field][FIELD_USED] = true
		}
	}

	for (id in objectFields)
	{
		objId = objectFields[id]

		tinyMCE.execCommand ("mceAddControl", true, "mceReplaceMe" + objId)
		if (firstBox)
		{
			tinyMCE.execCommand ("mceRemoveControl", false, "mceReplaceMe" + objId)
			tinyMCE.execCommand ("mceAddControl", true, "mceReplaceMe" + objId)
			firstBox = false
		}

		obj = document.getElementById ("mceReplaceMe" + objId)
		obj.style.margin = "20px"
		obj.style.width = "90%"
	}
}

// Cancel button clicked
function addFieldCancelButtonClicked ()
{
	div = addField.div
	div.style.visibility = "hidden"
}

//-----------------------------
// Main functions
//-----------------------------

// Handle window resizes
function calculateSizes ()
{
	// Setup the sizes of the navigation window and lesson plan
	// Get the size of the window
	bannerHeight = top.document.getElementById ('banner').height
	toolbarHeight = top.document.getElementById ('toolbar').clientHeight + 14

	windowHeight = top.innerHeight - bannerHeight - toolbarHeight - 10
	windowWidth = top.innerWidth

	// Set the height and width of stuff
	main = top.document.getElementById ('main')
	main.setAttribute ("style", "height: " + windowHeight)
}

// Main function that does all the kickoff stuff
function main ()
{
	// Make sure the main window knows to call that every time we
	// resize.
	top.onresize = calculateSizes

	// Initialize the rich text entry
	tinyMCE.init({
		mode : "textareas",
		theme : "simple"
	});

	// Create the stuff.
	lessonPlan = new LessonPlan ()

	// Calculate the size of the window
	calculateSizes ()

	// Pop up the New Box dialog
	newBox = new NewBox ()
}

main ()

