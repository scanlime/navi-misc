//------------------------------------
// Global variables
//------------------------------------

// Window size
windowWidth = 0
windowHeight = 0
bannerHeight = 0
toolbarHeight = 0

// Field types
TYPE_ENTRY = 1
TYPE_TEXTAREA = 2
TYPE_LINK = 3

// Field parts
FIELD_DESC = 0
FIELD_TYPE = 1
FIELD_USED = 2
FIELD_ID   = 2

// Stock Icons
STOCK_NEW = "images/new.gif"
STOCK_OPEN = "images/open.gif"
STOCK_SAVE = "images/save.gif"
STOCK_LINK = "images/link.gif"
STOCK_FIELD = "images/field.gif"
STOCK_NOTE = "images/note.gif"
STOCK_REF = "images/reference.gif"
STOCK_BOX = "images/box.gif"

// General stuff
var lessonPlan
var newBox
var addInitFieldsDlg
var addField
var addToBox
var focusCounter = 0
var firstBox = true

// Fields and their descriptions
// Required fields
requiredFields = new Array ()
requiredFields["Title"] = {
    desc: "The title of this lesson",
    type: TYPE_ENTRY,
    id:   "Title"}

requiredFields["Author"] = {
	desc: "The creator(s) of this lesson",
	type: TYPE_ENTRY,
    id:   "Author"}

requiredFields["Brief Description"] = {
    desc: "A brief, one- or two-sentence description of this lesson",
    type: TYPE_ENTRY,
    id:   "Description"}

requiredFields["Required Time"] = {
	desc: "The amount of time required to teach this lesson",
	type: TYPE_ENTRY,
    id:   "RequiredTime"}

requiredFields["About"] = {
	desc: "Elaborate more about what this box is about.  This forms\
           the preface of the lesson.",
	type: TYPE_TEXTAREA,
    id:   "About"}

requiredFields["Standards"] = {
	desc: "The school standards covered by this lesson",
	type: TYPE_TEXTAREA,
    id:   "Standards"}

requiredFields["Linked Lessons"] = {
	desc: "These lessons are linked to this one.",
	type: TYPE_LINK,
	id:   "LinkedLessons"}

requiredFields["Resources"] = {
	desc: "External documents that have been uploaded into the system.",
	type: TYPE_LINK,
	id:   "Resources"}

requiredFields["Notes"] = {
	desc: "Extra notes pertaining to this lesson (i.e. a particular teaching style\
	       worked better than another, etc.)",
	type: TYPE_LINK,
	id:   "Notes"}

// Optional fields.  These have three fields:
//    Description - Description of the field
//    Type - Type of field to create
//    Used - Whether it has been used or not
optionalFields = new Array ()
optionalFields["Introduction"] = {
	desc: "Introduce the topic",
	type: TYPE_TEXTAREA,
    id:   "Introduction",
	used: false}

optionalFields["Materials"] = {
	desc: "Materials (such as scissors or glue) that are required",
	type: TYPE_TEXTAREA,
    id:   "Materials",
	used: false}

optionalFields["Detailed Schedule"] = {
	desc: "Provide a detailed schedule for the lesson",
	type: TYPE_TEXTAREA,
    id:   "DetailSchedule",
	used: false}

optionalFields["Prerequisites"] = {
	desc: "The concepts that students should bring in to this lesson",
	type: TYPE_TEXTAREA,
    id:   "Prereqs",
	used: false}

optionalFields["Instructions (Methods)"] = {
	desc: "How the lesson is to be taught",
	type: TYPE_TEXTAREA,
    id:   "Instructions",
	used: false}


