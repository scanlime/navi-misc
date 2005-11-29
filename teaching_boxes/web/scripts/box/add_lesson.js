//----------------------------
// AddLessonDlg Class
//----------------------------

var http

function AddLessonDlg ()
{
	// Members
	this.div = top.document.getElementById ('add_lesson')
	this.select = top.document.getElementById ('lessonSelect')
	this.addButton = top.document.getElementById ('lessonAdd')
	this.createButton = top.document.getElementById ('lessonCreateNew')
	this.cancelButton = top.document.getElementById ('lessonCancel')
	this.lessons = new Array ()
	this.lessonObjs = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = lessonSelectionChanged
	this.addButtonClicked = lessonAddButtonClicked
	this.createButtonClicked = lessonCreateButtonClicked
	this.cancelButtonClicked = lessonCancelButtonClicked
	this.dataLoaded = lessonDataLoaded

    // Clear the old DLG stuff out
    this.select.innerHTML = ""
    this.select.appendChild (document.createTextNode ("Loading..."))

	// Add the stuff to the box
	http = new XMLHttpRequest ()
	http.open ('get', 'cgi-bin/dosomething.py?getAllLessons')
	http.onreadystatechange = this.dataLoaded
	http.classObj = this
	http.send (null)

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
	this.select.classObj = this

	this.addButton.onclick = this.addButtonClicked
	this.addButton.classObj = this

	this.createButton.onclick = this.createButtonClicked
	this.createButton.classObj = this

	this.cancelButton.onclick = this.cancelButtonClicked
	this.cancelButton.classObj = this

	// Make the dialog visible
	this.div.style.display = "block"
	this.div.style.position = "absolute"
	this.div.style.left = (windowWidth / 2) - (this.div.clientWidth / 2)
	this.div.style.top = (windowHeight / 2) - (this.div.clientHeight / 2) + bannerHeight
	this.div.style.visibility = "visible"
	this.div.style.cursor = "default"
}

function lessonDataLoaded (event)
{
	if (http.readyState == 4)
	{
		var response = http.responseText
		var update = new Array ()
		var lessons = new Array ()

		if (response.indexOf ('|') != -1)
		{
			update = response.split ('\n')
			for (line in update)
			{
				var data
                var id
				var name
				var desc

				data = update[line].split('|')
                id = data[0]
				name = data[1]
				desc = data[2]

				if (!desc)
					continue

				lessons.push ([id, name, desc])
			}
		}

		http.classObj.lessons = lessons

        // Clear the old DLG stuff out
        http.classObj.select.innerHTML = ""


		for (lesson in lessons)
		{
            var fieldID = lessons[lesson][0]
			var fieldName = lessons[lesson][1]
			var fieldDesc = lessons[lesson][2]
			var div = document.createElement ("div")
			div.appendChild (document.createTextNode (fieldName))
			div.fieldName = fieldName
			div.desc = fieldDesc
            div.linkID = fieldID
			div.selected = false
			div.style.padding = "3px"
			http.classObj.lessonObjs.push (div)
			http.classObj.select.appendChild (div)
		}
	}
}

// Selection changed handler
function lessonSelectionChanged (event)
{
	parentDiv = this.classObj.div
	select = this.classObj.select
	div = top.document.getElementById ('lessonDescription')

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
	for (lesson in this.classObj.lessonObjs)
	{
		this.classObj.lessonObjs[lesson].style.background = "#ffffff"
		this.classObj.lessonObjs[lesson].selected = false
	}

	// Grab the description
	if (target.fieldName)
		text = target.desc
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
function lessonAddButtonClicked ()
{
	div = addLesson.div
	div.style.visibility = "hidden"

	// Everything we do here will be in the backend.  Tell the web-server
	// to add this lesson to the selected box.

	// Determine which entry is selected
	var selectedObj
	for (lesson in this.classObj.lessonObjs)
	{
		if (this.classObj.lessonObjs[lesson].selected)
		{
			selectedObj = this.classObj.lessonObjs[lesson]
			break
		}
	}

	if (!selectedObj)
		return

    // When we modify the link object, it will signal
    // the box to save itself.
	box.linkedLessonsObj.linkObj.addLink (
        selectedObj.linkID,
        selectedObj.fieldName,
        selectedObj.desc, 0, "")
}

// Cancel button clicked
function lessonCancelButtonClicked ()
{
	div = addLesson.div
	div.style.visibility = "hidden"
}

// Create new clicked
function lessonCreateButtonClicked ()
{
	div = addLesson.div
	div.style.visibility = "hidden"
	top.window.location = "new_lesson.html"
}

