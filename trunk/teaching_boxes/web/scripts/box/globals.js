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
var box
var newBox
var addField
var focusCounter = 0
var firstBox = true

// Fields and their descriptions
requiredFields = new Array ()
requiredFields["Lessons"] = [
	"These lessons are contained within this box.",
	TYPE_LINK,
	"LinkedLessons"]

