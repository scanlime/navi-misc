//----------------------------
// AddToBoxDlg Class
//----------------------------
var http

function AddToBoxDlg ()
{
	// Members
	this.div = top.document.getElementById ('add_to_box')
	this.select = top.document.getElementById ('boxSelect')
	this.addButton = top.document.getElementById ('boxAdd')
	this.cancelButton = top.document.getElementById ('boxCancel')
	this.options = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = boxSelectionChanged
	this.addButtonClicked = boxAddButtonClicked
	this.cancelButtonClicked = boxCancelButtonClicked
	this.dataLoaded = boxDataLoaded

	// Clear the old DLG stuff out
	this.select.innerHTML = ""
    this.select.appendChild (document.createTextNode ("Loading..."))

	// Add the stuff to the box
	http = new XMLHttpRequest ()
	http.open ('get', 'cgi-bin/dosomething.py?getAllBoxes')
	http.onreadystatechange = this.dataLoaded
	http.classObj = this
	http.send (null)

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
	this.select.classObj = this

	this.addButton.onclick = this.addButtonClicked
	this.addButton.classObj = this

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

function boxDataLoaded ()
{
	if (http.readyState == 4)
	{
		var response = http.responseText
		var update = new Array ()
		var boxes = new Array ()

		if (response.indexOf ('|') != 1)
		{
			update = response.split ('\n')
			for (line in update)
			{
				var data
				var name
				var desc

				data = update[line].split('|')
				name = data[0]
				desc = data[1]

				if (!desc)
					continue

				boxes.push ([name, desc])
			}
		}

        http.classObj.select.innerHTML = ""

		for (box in boxes)
		{
			fieldName = boxes[box][0]
			fieldDesc = boxes[box][1]
			div = document.createElement ("div")
			div.appendChild (document.createTextNode (fieldName))
			div.fieldName = fieldName
			div.desc = fieldDesc
			div.selected = false
			div.style.padding = "3px"
			http.classObj.options.push (div)
			http.classObj.select.appendChild (div)
		}

		http.classObj.boxes = boxes
	}
}

// Selection changed handler
function boxSelectionChanged (event)
{
	parentDiv = this.classObj.div
	select = this.classObj.select
	div = top.document.getElementById ('boxDescription')

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
	for (option in this.classObj.options)
	{
		this.classObj.options[option].style.background = "#ffffff"
		this.classObj.options[option].selected = false
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
function boxAddButtonClicked ()
{
	div = addToBox.div
	div.style.visibility = "hidden"

    alert ("Lesson successfully added.")

	// Everything we do here will be in the backend.  Tell the web-server
	// to add this lesson to the selected box.

	// FIXME: The following stuff doesn't belong here, but in the "Create Link"
	//        dialog box.  It's here for now to just demonstrate adding links,
	//		  and it'll show up in the "Linked Lessons" field at the bottom of
	//        the lesson.

	// Determine which entry is selected
	var selectedObj
	for (box in this.classObj.options)
	{
		if (this.classObj.options[box].selected)
		{
			selectedObj = this.classObj.options[box]
			break
		}
	}

	if (!selectedObj)
		return
}

// Cancel button clicked
function boxCancelButtonClicked ()
{
	div = addToBox.div
	div.style.visibility = "hidden"
}

