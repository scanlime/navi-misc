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
FIELD_LINKID = 3

// Stock Icons
STOCK_NEW = "images/new.gif"
STOCK_OPEN = "images/open.gif"
STOCK_SAVE = "images/save.gif"
STOCK_LINK = "images/link.gif"
STOCK_FIELD = "images/field.gif"
STOCK_NOTE = "images/note.gif"
STOCK_REF = "images/reference.gif"

// General stuff
var lessonPlan
var newBox
var addField
var focusCounter = 0
var firstBox = true

// Fields and their descriptions
// Required fields
requiredFields = new Array ()
requiredFields["Title"] = [
	"The title associated with this lesson",
	TYPE_ENTRY]

requiredFields["Author"] = [
	"The creator(s) of this lesson",
	TYPE_ENTRY]

requiredFields["Required Time"] = [
	"The amount of time required to teach this lesson",
	TYPE_ENTRY]

requiredFields["About"] = [
	"A brief description of the lesson",
	TYPE_TEXTAREA]

requiredFields["Standards"] = [
	"The school standards covered by this lesson",
	TYPE_TEXTAREA]

requiredFields["Linked Lessons"] = [
	"These lessons are linked to this one.",
	TYPE_LINK,
	"LinkedLessons"]

// Optional fields.  These have three fields:
//    Description - Description of the field
//    Type - Type of field to create
//    Used - Whether it has been used or not
optionalFields = new Array ()
optionalFields["Introduction"] = [
	"Introduce the topic",
	TYPE_TEXTAREA,
	false]

optionalFields["Materials"] = [
	"Materials (such as scissors or glue) that are required",
	TYPE_TEXTAREA,
	false]

optionalFields["Detailed Schedule"] = [
	"Provide a detailed schedule for the lesson",
	TYPE_TEXTAREA,
	false]

optionalFields["Prerequisites"] = [
	"The concepts that students should bring in to this lesson",
	TYPE_TEXTAREA,
	false]

optionalFields["Instructions (Methods)"] = [
	"How the lesson is to be taught",
	TYPE_TEXTAREA,
	false]


