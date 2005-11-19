// Field types
TYPE_ENTRY = 1
TYPE_TEXTAREA = 2

function LessonPlan ()
{
	// Members
	this.fields = new Array ()
	this.mainDiv = document.getElementById ('mainDiv')

	// Methods
	this.addField = addField

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
	this.addField ("Materials", TYPE_TEXTAREA)
}

function addField (name, type)
{
	field = createField (name, type)
	this.fields.push (field)
	this.mainDiv.appendChild (field)
}

function new_dialog ()
{
	document.write ("<h3>Testing</h3><br/>Testing...")
	return new Array ()
}

function createField (name, type)
{
	title = document.createElement ("h2")
	text  = document.createTextNode (name)
	title.appendChild (text)

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

	div = document.createElement ("div")
	div.appendChild (title)
	div.appendChild (entry)

	return div
}

function calculateSizes ()
{
	// Setup the sizes of the navigation window and lesson plan
	// Get the size of the window
	bannerHeight = top.document.getElementById ('banner').height
	toolbarHeight = top.document.getElementById ('toolbar').clientHeight

	// TODO: We need to figure out a better way of getting rid of the
	// blank line at the bottom of the page...The 35-point subtract
	// covers this in my web browser. -- Cory
	height = top.innerHeight - bannerHeight - toolbarHeight - 35
	width = top.innerWidth

	// Set the height and width of stuff
	main = top.document.getElementById ('main')
	main.setAttribute ("style", "height: " + height)
}


function main ()
{
	// Calculate the size of the window
	calculateSizes ()

	// Make sure the main window knows to call that every time we
	// resize.
	top.onresize = calculateSizes

	// Create the stuff.
	lessonPlan = new LessonPlan ()
}

main ()

