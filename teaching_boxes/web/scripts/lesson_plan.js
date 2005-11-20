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

// General stuff
var lessonPlan
var newBox

// Fields and their descriptions
// Required fields
requiredFields = new Array ();
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

// Optional fields
optionalFields = new Array ();
optionalFields["Introduction"] = [
	"Introduce the topic",
	TYPE_TEXTAREA]

optionalFields["Materials"] = [
	"Materials (such as scissors or glue) that are required",
	TYPE_TEXTAREA]

optionalFields["Detailed Schedule"] = [
	"Provide a detailed schedule for the lesson",
	TYPE_TEXTAREA]

optionalFields["Prerequisites"] = [
	"The concepts that students should bring in to this lesson",
	TYPE_TEXTAREA]

optionalFields["Instructions (Methods)"] = [
	"How the lesson is to be taught",
	TYPE_TEXTAREA]

//------------------------------------
// Class LessonPlan
//------------------------------------

// Constructor for the LessonPlan object
function LessonPlan ()
{
	// Members
	this.fields  = new Array ()
	this.mainDiv = document.getElementById ('mainDiv')

	// Methods
	this.addField    = addField

	// Title
	title = document.createElement ("h1")
	text = document.createTextNode ("New Plan")
	title.appendChild (text)
	this.mainDiv.appendChild (title)

	// Fields
	for (item in requiredFields)
	{
		this.addField (item, requiredFields[item], false)
	}
}

// Add a field to the lesson plan
function addField (name, field, removable)
{
	desc = field[0]
	type = field[1]

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
	this.createButton = top.document.getElementById ('create')
	this.cancelButton = top.document.getElementById ('cancel')
	this.options = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = selectionChanged

	// Add the stuff to the box
	for (field in optionalFields)
	{
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
	this.select.onclick = selectionChanged
	this.createButton.onclick = createButtonClicked
	this.cancelButton.onclick = cancelButtonClicked

	// Make the dialog visible
	this.div.style.display = "block"
	this.div.style.position = "absolute"
	this.div.style.left = (windowWidth / 2) - (this.div.clientWidth / 2)
	this.div.style.top = (windowHeight / 2) - (this.div.clientHeight / 2) + bannerHeight
	this.div.style.visibility = "visible"
}

// Selection changed handler
function selectionChanged (event)
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
		text = optionalFields[target.fieldName][0]
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
function createButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"

	// Figure out which fields were selected and add them to the view
	options = div.options
	for (i in options)
	{
		if (options[i].myCheck.checked)
		{
			field = div.options[i].fieldName
			lessonPlan.addField (field, optionalFields[field], true)
		}
	}
}

// Cancel button clicked
function cancelButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"
	history.back ()
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
	toolbarHeight = top.document.getElementById ('toolbar').clientHeight

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
	// Calculate the size of the window
	calculateSizes ()

	// Make sure the main window knows to call that every time we
	// resize.
	top.onresize = calculateSizes

	// Create the stuff.
	lessonPlan = new LessonPlan ()

	// Pop up the New Box dialog
	newBox = new NewBox ()
}

main ()

