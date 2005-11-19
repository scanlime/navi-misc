function LessonPlan ()
{
	// Members
	this.fields = new Array ()

	// Methods
	this.addField = addField

	this.addField (createField ("Author", true)
	this.addField (createField ("Title", true)
	this.addField (createField ("About", false)
}

function addField (name)
{
	field = createField (name, false)
	this.fields.push (field)
	document.body.appendChild (field)
}

function new_dialog ()
{
	document.write ("<h3>Testing</h3><br/>Testing...")
	return new Array ()
}

function createField (name, isEntry)
{
	title = document.createElement ("b")
	text  = document.createTextElement (name)
	title.appendChild (text)

	if (isEntry) {
		entry = document.createElement ("input")
		entry.setAttribute ("type", "text")
	} else {
		entry = document.createElement ("text-area")
	}

	div = document.createElement ("div")
	div.appendChild (title)
	div.appendChild (entry)

	return div
}
