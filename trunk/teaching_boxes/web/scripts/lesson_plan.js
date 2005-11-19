function LessonPlan ()
{
	document.write ("new plan")
	// Members
	this.fields = new Array ()

	// Methods
	this.addField = addField

	this.addField ("Author")
	this.addField ("Title")
	this.addField ("About")

	for (field in new_dialog ()) {
		document.write ("new field")
	}
}

function addField (name)
{
	document.write (name)
	field = createField (name, false)
	//this.fields.push (field)
	//document.body.appendChild (field)
}

function new_dialog ()
{
	document.write ("<h3>Testing</h3><br/>Testing...")
	return new Array ()
}

function createField (name, isEntry)
{
	document.write ("create " + name)
	//title = document.createElement ("b")
	//text  = document.createTextElement (name)
	//title.appendChild (text)

	//if (isEntry) {
		//entry = document.createElement ("input")
		//entry.setAttribute ("type", "text")
	//} else {
		//entry = document.createElement ("text-area")
	//}

	//div = document.createElement ("div")
	//div.appendChild (title)
	//div.appendChild (entry)

	//document.write(div)
}
