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
	this.addField ("Title", TYPE_ENTRY)
	this.addField ("Author", TYPE_ENTRY)
	this.addField ("Required Time", TYPE_ENTRY)
	this.addField ("About", TYPE_TEXTAREA)
	this.addField ("Standards", TYPE_TEXTAREA)
}

// Add a field to the lesson plan
function addField (name, type)
{
	field = createField (name, type)
	this.fields.push (field)
	this.mainDiv.appendChild (field)
}


// Create a field and return it
function createField (name, type)
{
	// Create the title of the field
	title = document.createElement ("span")
	text  = document.createTextNode (name)

	title.setAttribute ("class", "fieldName")
	title.appendChild (text)

	date = new Date ()
	id = new String (date.getTime ())

	// FIXME - Might be nice to have this change the cursor on mouseover
	rm = document.createElement ("span")
	rm.setAttribute ("class", "removeButton")
	rm.setAttribute ("id", id)
	rm.appendChild (document.createTextNode ("Remove"))
	rm.onclick = removeField

	// Determine which type of field we want
	switch (type)
	{
		case TYPE_ENTRY:
			entry = document.createElement ("input")
			entry.setAttribute ("type", "text")
			entry.setAttribute ("style", "width: 100%; margin-bottom: 10px;")
			break
		case TYPE_TEXTAREA:
			entry = document.createElement ("textarea")
			entry.setAttribute ("rows", 5)
			entry.setAttribute ("style", "width: 100%;")
			break
		default:
			entry = document.createElement ("textarea")
			break
	}

	// Package it all up and return it
	div = document.createElement ("div")
	div.setAttribute ("id", id)
	div.appendChild (title)
	div.appendChild (rm)
	div.appendChild (entry)

	return div
}

function removeField (event)
{
	if (confirm ("Are you sure you want to remove this field?")) {
		var id
		var mainDiv = document.getElementById ("mainDiv")

		if (event.target) {
			id = event.target.getAttribute ("id")
		} else {
			id = event.srcElement.getAttribute ("id")
		}

		div = document.getElementById (id)
		mainDiv.removeChild (div)
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
	this.fields = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = selectionChanged

	// Set the fields
	this.fields.push ("Introduction")
	this.fields.push ("Materials")
	this.fields.push ("Detailed Schedule")
	this.fields.push ("Prerequisites")
	this.fields.push ("Instructions (Methods)")

	// Add the stuff to the box
	for (field in this.fields)
	{
		div = document.createElement ("div")

		optionCheck = document.createElement("input")
		optionCheck.type = "checkbox"
		optionCheck.checked = false
		optionCheck.optionNumber = field

		optionText = document.createTextNode (this.fields[field])

		div.appendChild (optionCheck)
		div.appendChild (optionText)
		div.optionNumber = field
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
	{
		parentDiv.options[option].style.background = "#ffffff"
	}

	// Figure out the text
	switch (target.optionNumber)
	{
		case "0":
			// Introduction
			text = "Introduce the topic"
			break
		case "1":
			// Materials
			text = "Materials (such as scissors or glue) that are required"
			break
		case "2":
			// Detailed Schedule
			text = "Provide a detailed schedule for the lesson"
			break
		case "3":
			// Prerequisites
			text = "The concepts that students should bring in to this lesson"
			break
		case "4":
			// Instructions (Methods)
			text = "How the lesson is to be taught"
			break
		default:
			text = ""
			break
	}

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
			lessonPlan.addField (newBox.fields[i], TYPE_TEXTAREA)
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

