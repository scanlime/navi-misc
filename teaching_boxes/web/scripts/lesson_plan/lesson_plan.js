//------------------------------------
// Class LessonPlan
//------------------------------------

// Constructor for the LessonPlan object
function LessonPlan (name, desc)
{
	// Members
	this.fields  = new Array ()
	this.mainDiv = document.getElementById ('mainDiv')
	this.toolbar = top.document.getElementById ('toolbar')
    this.name    = name
    this.desc    = desc

	// Methods
	this.addField     = addField
	this.setupToolbar = setupToolbar
	this.addLink	  = addLink
	this.saveYourself = saveYourself

	// Title
	title = document.createElement ("h1")
	text = document.createTextNode (name)
	title.appendChild (text)
	this.mainDiv.appendChild (title)

	// Description
	this.mainDiv.appendChild (document.createTextNode (desc))

	// Toolbar
	this.setupToolbar (this)

    // Work around a bug in the MCE
    addFields = function ()
    {
        // Fields
        for (item in requiredFields)
        {
            var myField = lessonPlan.addField (item, requiredFields[item], false, true)

			// Bug fix for Firefox.  It displays the first dynamic box as a grey box
			// with larger text.  This creates a new box that is well formed, and
			// sets up the old one to be deleted.
			if (requiredFields[item][FIELD_TYPE] == TYPE_TEXTAREA && firstBox == true)
			{
				firstBox = myField
				lessonPlan.addField (item, requiredFields[item], false, true)
			}	
        }

		// If we have a box to delete, do it here.
		if (firstBox)
		{
			myRemoveFields = function (firstField) {
				id = firstField.id
				fieldName = firstField.fieldName
				lessonPlan.mainDiv.removeChild (firstField)
				delete lessonPlan.fields[id]
				firstBox = false
			}

			// Bug fix for Firefox, it crashes without this.
			setTimeout('myRemoveFields(firstBox)', 200)
		}
    }

    setTimeout('addFields()', 1)
}

function saveYourself ()
{
}

function addLink (fieldName, id, name, desc, linkType, addr)
{
	var field = document.getElementById (fieldName)
	field.linkObj.addLink (id, name, desc, linkType, addr)
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
		"Add a link to another lesson",
		STOCK_LINK,
		function (e) {addLink = new AddLinkDlg ()})

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

	tr = createButton (tr,
		"Add to Box",
		"Add this lesson to a teaching box",
		STOCK_BOX,
		function (e) {addToBox = new AddToBoxDlg ()})

	tab.appendChild (tr)
	plan.toolbar.appendChild (tab)
}

// Add a field to the lesson plan
function addField (name, field, removable, mceReplace)
{
	newField = createField (name, field, removable)
	this.fields[newField.id] = newField
	this.mainDiv.appendChild (newField)

    if (mceReplace && document.getElementById('mceReplaceMe' + newField.id))
    {
        tinyMCE.execCommand ("mceAddControl", true, "mceReplaceMe" + newField.id)
    }

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

function onCancelClick (event)
{
	newBox.style.display = "none"
	top.document.getElementById ("iframeDiv").style.display = "block"
    history.back ()
}

function onCreateClick (event)
{
	// Get the name and description fields
	var name = top.document.getElementById ("lessonName").value
	var desc = top.document.getElementById ("lessonDesc").value

	// Create the stuff.
	lessonPlan = new LessonPlan (name, desc)

	// Calculate the size of the window
	calculateSizes ()

    // Make everything visible again
	newBox.style.display = "none"
	top.document.getElementById ("iframeDiv").style.display = "block"

    // Show the Add Initial Fields dialog
    addInitFieldsDlg = new AddInitFieldsDlg ()
}

// Main function that does all the kickoff stuff
function main ()
{
	// Make sure the main window knows to call that every time we
	// resize.
	top.onresize = calculateSizes

	// Calculate the size of the window
	calculateSizes ()

	newBox = top.document.getElementById ("new_lesson")
	newBox.style.display = "block"
	newBox.style.position = "absolute"
	newBox.style.left = (windowWidth / 2) - (newBox.clientWidth / 2)
	newBox.style.top = (windowHeight / 2) - (newBox.clientHeight / 2) + bannerHeight
	newBox.style.visibility = "visible"
	newBox.style.cursor = "default"

	var newCancel = top.document.getElementById ("newCancel")
	newCancel.onclick = onCancelClick

	var newCreate = top.document.getElementById ("newCreate")
	newCreate.onclick = onCreateClick
}

main ()

