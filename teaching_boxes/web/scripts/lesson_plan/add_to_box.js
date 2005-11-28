//----------------------------
// AddToBoxDlg Class
//----------------------------
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

	// Clear the old DLG stuff out
	this.select.innerHTML = ""

	// Add the stuff to the box
	// TODO: Load this from the web server

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
	this.addButton.onclick = this.addButtonClicked
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
function boxSelectionChanged (event)
{
	parentDiv = addToBox.div
	select = addToBox.select
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
	for (option in parentDiv.options)
	{
		parentDiv.options[option].style.background = "#ffffff"
		parentDiv.options[option].selected = false
	}

	// Grab the description
	// TODO: We need to get this from the information we get from
	// the server.
	if (target.fieldName)
		text = ""
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

	// Everything we do here will be in the backend.  Tell the web-server
	// to add this lesson to the selected box.
}

// Cancel button clicked
function boxCancelButtonClicked ()
{
	div = addToBox.div
	div.style.visibility = "hidden"
}

