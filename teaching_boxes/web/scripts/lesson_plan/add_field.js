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
		if (optionalFields[field]['used'])
			continue

		div = document.createElement ("div")
		div.appendChild (document.createTextNode (field))
		div.fieldName = field
		div.selected = false
		div.style.padding = "3px"
		div.width = "100%"
		div.onclick = this.selectionChanged
		div.parentDiv = div
		this.options.push (div)
		this.select.appendChild (div)
	}

	// Connect events to the selector
	this.createButton.onclick = this.createButtonClicked
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

	target = parent

	// Clear the current selection
	for (option in parentDiv.options)
	{
		parentDiv.options[option].style.background = "#ffffff"
		parentDiv.options[option].selected = false
	}

	// Grab the description
	if (target.fieldName)
		text = optionalFields[target.fieldName]['desc']
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
	objectFields = new Array ()
	var firstField = null

	for (i in options)
	{
		if (options[i].selected)
		{
			field = div.options[i].fieldName
			objField = lessonPlan.addField (field, optionalFields[field], true, false)
			objectFields.push (optionalFields[field]['id'])
			optionalFields[field]['used'] = true
		}
	}

	for (id in objectFields)
	{
		objId = objectFields[id]

		tinyMCE.execCommand ("mceAddControl", true, objId)
		obj = document.getElementById (objId)
		obj.style.margin = "20px"
		obj.style.width = "90%"
	}
}

// Cancel button clicked
function addFieldCancelButtonClicked ()
{
	div = addField.div
	div.style.visibility = "hidden"
}


