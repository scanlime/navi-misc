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
	text = document.createTextNode ("New Lesson Plan")
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
			entry.style.border = "thin solid #7777cc"
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
// Main functions
//-----------------------------

// Handle window resizes
function calculateSizes ()
{
	// Setup the sizes of the navigation window and lesson plan
	// Get the size of the window
	bannerHeight = top.document.getElementById ('banner').height
	toolbarHeight = top.document.getElementById ('toolbar').clientHeight + 14
	navbarHeight = top.document.getElementById ('navigation').clientHeight - 5

	windowHeight = top.innerHeight - bannerHeight - toolbarHeight - navbarHeight - 0
	windowWidth = top.innerWidth

	// Set the height and width of stuff
	main = top.document.getElementById ('iframe')
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

