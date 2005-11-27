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

// General stuff
var lessonPlan
var newBox
var addField

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
		this.addField (item, requiredFields[item], false)
	}
}

function setupToolbar (plan)
{
	// New lesson button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("New Lesson"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	plan.toolbar.appendChild (button)

	// Save lesson button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("Save Lesson"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	plan.toolbar.appendChild (button)

	// Create Link button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("Create Link"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	plan.toolbar.appendChild (button)

	// Add Field button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("Add Field"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	button.onclick = function (event) {addField = new AddFieldDlg ()}
	plan.toolbar.appendChild (button)

	// Add Resource button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("Add Resource"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	plan.toolbar.appendChild (button)

	// Add Note button
	button = document.createElement ("span")
	button.appendChild (document.createTextNode ("Add Note"))
	button.style.padding = "5px"
	button.style.margin = "0px 5px 0px 5px"
	button.style.border = "thin solid #000000"
	button.onmouseover = overToolbarButton
	button.onmouseout = outToolbarButton
	plan.toolbar.appendChild (button)
}

function overToolbarButton (event)
{
	event.target.style.backgroundColor = "#eeeeee"
}

function outToolbarButton (event)
{
	event.target.style.backgroundColor = "#cccccc"
}

// Add a field to the lesson plan
function addField (name, field, removable)
{
	desc = field[FIELD_DESC]
	type = field[FIELD_TYPE]

	field = createField (name, desc, type, removable)
	this.fields[field.id] = field
	this.mainDiv.appendChild (field)
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

	// FIXME - Might be nice to have this change the cursor on mouseover
	if (removable) {
		rm = document.createElement ("span")
		rm.setAttribute ("class", "removeButton")
		rm.setAttribute ("id", id)
		rm.appendChild (document.createTextNode ("Remove"))
		rm.onclick = removeField
	}

	// Create the description tag
	descTag = document.createElement ("span")
	descTag.setAttribute ("class", "descriptionField")
	descTag.appendChild (document.createTextNode (desc))

	// Determine which type of field we want
	switch (type)
	{
		case TYPE_ENTRY:
			entry = document.createElement ("input")
			entry.setAttribute ("type", "text")
			entry.setAttribute ("style", "width: 100%; margin-bottom: 20px;")
			break
		case TYPE_TEXTAREA:
			entry = document.createElement ("textarea")
			entry.setAttribute ("rows", 5)
			entry.setAttribute ("style", "width: 100%; margin-bottom: 20px;")
			break
		default:
			entry = document.createElement ("textarea")
			break
	}

	// Package it all up and return it
	div = document.createElement ("div")
	div.setAttribute ("id", id)
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

		optionCheck = document.createElement("input")
		optionCheck.type = "checkbox"
		optionCheck.checked = false
		optionCheck.fieldName = field

		optionText = document.createTextNode (field)

		div.appendChild (optionCheck)
		div.appendChild (optionText)
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
	for (i in options)
	{
		if (options[i].myCheck.checked)
		{
			field = options[i].fieldName
			lessonPlan.addField (field, optionalFields[field], true)
			optionalFields[field][FIELD_USED] = true;
		}
	}
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
	for (i in options)
	{
		if (options[i].selected)
		{
			field = div.options[i].fieldName
			lessonPlan.addField (field, optionalFields[field], true)
			optionalFields[field][FIELD_USED] = true
		}
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

	// FIXME: We need to figure out a better way of getting rid of the
	// blank line at the bottom of the page...The 35-point subtract
	// covers this in my web browser. -- Cory
	windowHeight = top.innerHeight - bannerHeight - toolbarHeight - 35
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

	// Create the stuff.
	lessonPlan = new LessonPlan ()

	// Calculate the size of the window
	calculateSizes ()

	// Pop up the New Box dialog
	newBox = new NewBox ()
}

main ()

