//------------------------------------
// Class Box
//------------------------------------

// Constructor for the LessonPlan object
function Box (name, desc)
{
	// Members
	this.fields  = new Array ()
	this.mainDiv = document.getElementById ('mainDiv')
	this.toolbar = top.document.getElementById ('toolbar')
	this.name = name
	this.desc = desc

	// Methods
	this.addField     = addField
	this.setupToolbar = setupToolbar
	this.saveYourself = saveBoxToDisk

	// Title
	title = document.createElement ("h1")
	text = document.createTextNode (name)
	title.appendChild (text)
	this.mainDiv.appendChild (title)

	// Description
	this.mainDiv.appendChild (document.createTextNode (desc))

	// Toolbar
	this.setupToolbar (this)

	// Fields
	myField = [
		"These lessons are contained within this box.",
		TYPE_LINK,
		"LinkedLessons"]
	
	this.linkedLessonsObj = this.addField ("Lessons", myField, false, false)
}

function saveBoxToDisk ()
{
	// TODO: Use AJAX to send a request to the server to save
	// the data.  The following code is just template code.
	/*
		http = new XMLHttpRequest ()
		http.open ('post', '/cgi/saveBox.cgi?name=' + this.name)
		http.send (data)
	*/
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
		"Add Lesson",
		"Create a new lesson and add it to the box.",
		STOCK_OPEN,
		function () {addLesson = new AddLessonDlg ()});

	tab.appendChild (tr)
	plan.toolbar.appendChild (tab)
}

// Add a field to the lesson plan
function addField (name, field, removable, doMceReplace)
{
	newField = createField (name, field, removable)
	this.fields[newField.id] = newField
	this.mainDiv.appendChild (newField)
	return newField
}

// Create a field and return it
function createField (name, field, removable)
{
	desc = field[FIELD_DESC]
	type = field[FIELD_TYPE]

	// Create the title of the field
	title = document.createElement ("span")
	text  = document.createTextNode (name)

	title.setAttribute ("class", "fieldName")
	title.appendChild (text)

	date = new Date ()
	id = new String (date.getTime ())
	var rmL = null

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
		case TYPE_LINK:
			rmL = document.createElement ("span")
			rmL.setAttribute ("class", "removeButton")
			rmL.setAttribute ("id", id)
			rmL.appendChild (document.createTextNode ("Remove All"))

			linkID = field[FIELD_LINKID]
			entry = document.createElement ("div")
			entry.style.width = "90%"
			entry.style.margin = "20px"
			entry.id = linkID
			entry.linkObj = new LinkField (entry, rmL)
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
	if (removable)
		div.appendChild (rm)

	if (rmL)
	{
		div.appendChild (rmL)
		div.linkObj = entry.linkObj
	}

	div.appendChild (document.createElement ("br"))
	div.appendChild (descTag)
	div.appendChild (entry)
	div.fieldName = name

	return div
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

function onCancelClick (event)
{
	newBox.style.display = "none"
	top.document.getElementById ("iframeDiv").style.display = "block"
}

function onCreateClick (event)
{
	// Get the name and description fields
	boxName = top.document.getElementById ("boxName").value
	boxDesc = top.document.getElementById ("boxDesc").value

	// Create the stuff.
	box = new Box (boxName, boxDesc)

	// Calculate the size of the window
	calculateSizes ()

	newBox.style.display = "none"
	top.document.getElementById ("iframeDiv").style.display = "block"
}

// Main function that does all the kickoff stuff
function main ()
{
	// Make sure the main window knows to call that every time we
	// resize.
	top.onresize = calculateSizes

	// Calculate the size of the window
	calculateSizes ()

	// Pop up the New Box dialog
	newBox = top.document.getElementById ("new_box")
	newBox.style.display = "block"
	newBox.style.position = "absolute"
	newBox.style.left = (windowWidth / 2) - (newBox.clientWidth / 2)
	newBox.style.top = (windowHeight / 2) - (newBox.clientHeight / 2) + bannerHeight
	newBox.style.visibility = "visible"
	newBox.style.cursor = "default"

	newCancel = top.document.getElementById ("newCancel")
	newCancel.onclick = onCancelClick

	newCreate = top.document.getElementById ("newCreate")
	newCreate.onclick = onCreateClick

	// newBox = new NewBox ()
}

main ()

