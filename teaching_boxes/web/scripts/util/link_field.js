//-------------------
// Link Field Class
//-------------------
LINK_ID   = 0
LINK_NAME = 1
LINK_DESC = 2
LINK_TYPE = 3
LINK_ADDR = 4

function LinkField (entry, removeObj, fileObject)
{
	this.entry = entry
	this.removeObj = removeObj
	this.links = new Array ()
    this.fileObject = fileObject

	this.redraw = LinkRedraw
	this.addLink = LinkAddLink
	this.removeAllClick = LinkRemoveAll
	this.removeLink = LinkRemoveLink
    this.serialize = LinkSerialize
    this.onlinkschange = null

    if (this.fileObject)
    {
        this.form = document.createElement ("form")
        this.form.action = "cgi-bin/upload.py"
        this.form.encoding = "multipart/form-data"
        this.form.method = "post"
        this.form.target = "target_upload"

        this.input = document.createElement ("input")
        this.input.type = "file"
        this.input.name = "upfile"

        this.form.appendChild (this.input)

        this.upload = document.createElement ("input")
        this.upload.type = "submit"
        this.upload.value = "Upload"
        this.form.appendChild (this.upload)

        this.target = document.createElement ("iframe")
        this.target.name = "target_upload"
        this.target.style.display = "none"
        this.form.appendChild (this.target)
    }

	this.redraw ()
	this.removeObj.onclick = this.removeAllClick
	this.removeObj.classObj = this
}

function LinkSerialize ()
{
    var data
    data = "[links]\n"

    for (link in this.links)
    {
        var id   = this.links[link][LINK_ID]
        data += "link" + link + "=" + id + "\n"
    }

    return data
}

function LinkRedraw ()
{
	this.entry.innerHTML = ""

    if (this.fileObject)
    {
        div = document.createElement ("div")
        div.style.marginBottom = "10px"

        div.appendChild (this.form)
        this.entry.appendChild (div)
    }

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
		span3.onclick = this.removeLink
		span3.classObj = this
		span3.link = link

		this.entry.appendChild (table)
	}
}

function LinkAddLink (id, name, description, type, address)
{
	link = [id, name, description, type, address]
	this.links.push (link)

	this.redraw ()

    if (this.onlinkschange)
        this.onlinkschange ()
}

function LinkRemoveAll (event)
{
	if (confirm ("Are you sure you want to remove all links?"))
	{
		this.classObj.links = new Array ()
		this.classObj.redraw ()

        if (this.classObj.onlinkschange)
            this.classObj.onlinkschange ()
	}
}

function LinkRemoveLink (event)
{
	if (confirm ("Are you sure you want to remove this link?"))
	{
		this.classObj.links.splice (this.link, 1)
		this.classObj.redraw ()

        if (this.classObj.onlinkschange)
            this.classObj.onlinkschange ()
	}
}

