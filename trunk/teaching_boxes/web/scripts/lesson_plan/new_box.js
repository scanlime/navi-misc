//-----------------------------
// Class NewBox
//-----------------------------

// Create a new dialog
function NewBox ()
{
	// Members
	this.div = top.document.getElementById ('new_box')
	this.select = top.document.getElementById ('fieldsSelect')
	this.createButton = top.document.getElementById ('newCreate')
	this.cancelButton = top.document.getElementById ('newCancel')
	this.options = new Array ()
	this.div.options = this.options

	// Methods
	this.selectionChanged = newBoxSelectionChanged
	this.createButtonClicked = newBoxCreateButtonClicked
	this.cancelButtonClicked = newBoxCancelButtonClicked

	// Add the stuff to the box
	for (field in optionalFields)
	{
		if (optionalFields[field][FIELD_USED])
			continue

		div = document.createElement ("div")
		tbl = document.createElement ("table")
		div.appendChild (tbl)

		tr = document.createElement ("tr")
		tr.style.margin = "0px"
		tr.style.border = "0px"
		tr.style.padding = "0px"
		tbl.style.margin = "0px"
		tbl.style.border = "0px"
		tbl.style.padding = "0px"

		tbl.appendChild (tr)

		td = document.createElement ("td")
		td.setAttribute ("valign", "middle")
		tr.appendChild (td)

		optionCheck = document.createElement("input")
		optionCheck.type = "checkbox"
		optionCheck.style.margin = "0px"
		optionCheck.checked = false
		optionCheck.fieldName = field
		td.appendChild (optionCheck)

		td = document.createElement ("td")
		td.setAttribute ("valign", "middle")
		tr.appendChild (td)

		optionText = document.createTextNode (field)
		td.appendChild (optionText)

		div.fieldName = field
		div.myCheck = optionCheck

		optionCheck.parentDiv = div

		this.options.push (div)
		this.select.appendChild (div)
	}

	// Connect events to the selector
	this.select.onclick = this.selectionChanged
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
function newBoxSelectionChanged (event)
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

	while (target.tagName == "TD" || target.tagName == "TR" || target.tagName == "TABLE")
	{
		target = target.parentNode
	}

	// Clear the current selection
	for (option in parentDiv.options)
		parentDiv.options[option].style.background = "#ffffff"

	// Grab the description
	if (target.fieldName)
		text = optionalFields[target.fieldName][FIELD_DESC]
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
function newBoxCreateButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"

	// Figure out which fields were selected and add them to the view
	options = div.options
	objectFields = new Array ()
	var firstField = null

	for (i in options)
	{
		if (options[i].myCheck.checked)
		{
			field = options[i].fieldName
			objField = lessonPlan.addField (field, optionalFields[field], true, true)
			optionalFields[field][FIELD_USED] = true;
			objectFields.push (objField.id)

			// Create a second copy of this box to work around a bug in rendering the
			// first dynamically generated text box
			if (!firstField)
			{
				firstField = objField
				objField2 = lessonPlan.addField (field, optionalFields[field], true, true)
				objectFields.push (objField2.id)
			}
		}
	}

	for (id in objectFields)
	{
		objId = objectFields[id]

		tinyMCE.execCommand ("mceAddControl", true, "mceReplaceMe" + objId)

		obj = document.getElementById ("mceReplaceMe" + objId)
		obj.style.margin = "20px"
		obj.style.width = "90%"
	}

	// Delete the mis-rendered box.  Note that this has to be done in a timeout
	// loop due to a bug in Firefox that would cause a crash if done immediately.
	deleteBox = function ()
	{
		var mainDiv = document.getElementById ("mainDiv")

		mainDiv.removeChild (lessonPlan.fields[firstField.id])
		delete lessonPlan.fields[firstField.id]
		firstBox = false
	}

	if (firstBox && firstField)
		setTimeout (deleteBox, 100)
}

// Cancel button clicked
function newBoxCancelButtonClicked ()
{
	div = newBox.div
	div.style.visibility = "hidden"
	history.back ()
}

