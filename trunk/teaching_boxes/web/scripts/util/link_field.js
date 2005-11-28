//-------------------
// Link Field Class
//-------------------
LINK_NAME = 0
LINK_DESC = 1
LINK_TYPE = 2
LINK_ADDR = 3

function LinkField (entry)
{
	this.entry = entry
	this.links = new Array ()

	this.redraw = LinkRedraw
	this.addLink = LinkAddLink

	this.redraw ()

	this.addLink ("Test link", "This is a test link to something", 0, "")
}

function LinkRedraw ()
{
	this.entry.innerHTML = ""

	if (this.links.length == 0)
	{
		this.entry.appendChild (document.createTextNode ("No links present."))
		return
	}

	for (link in this.links)
	{
		name = this.links[link][LINK_NAME]
		desc = this.links[link][LINK_DESC]
		type = this.links[link][LINK_TYPE]
		addr = this.links[link][LINK_ADDR]

		table = document.createElement ("table")
		tr = document.createElement ("tr")
		table.appendChild (tr)

		td1 = document.createElement ("td")
		tr.appendChild (td1)
		td1.width = "100%"

		span1 = document.createElement ("span")
		td1.appendChild (span1)
		span1.setAttribute ("class", "linkField")
		span1.appendChild (document.createTextNode (name))

		span2 = document.createElement ("span")
		td1.appendChild (span2)
		span2.style.marginLeft = "10px"
		span2.appendChild (document.createTextNode (desc))

		td2 = document.createElement ("td")
		tr.appendChild (td2)

		span3 = document.createElement ("span")
		td2.appendChild (span3)
		span3.setAttribute ("class", "removeButton")
		span3.appendChild (document.createTextNode ("Remove"))

		this.entry.appendChild (table)
	}
}

function LinkAddLink (name, description, type, address)
{
	link = [name, description, type, address]
	this.links.push (link)

	this.redraw ()
}
