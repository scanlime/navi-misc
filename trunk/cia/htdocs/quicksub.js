//\//////////////////////////////////////////////////////////////////////////////////
//\  quickSub 0.3.3
//\  Copyright Jason Brome 2003,2004. All rights reserved.
//\
//\  By Jason Brome (jason@methodize.org).  Last modified 2004-01-31
//\  http://www.methodize.org/quicksub/
//\
//\  Inspired by the overLIB script by Erik Bosrup (erik@bosrup.com)
//\  http://www.bosrup.com/web/overlib/
//\
//\  This program is free software; you can redistribute it and/or
//\  modify it under the terms of the GNU General Public License
//\  as published by the Free Software Foundation; either version 2
//\  of the License, or (at your option) any later version.
//\
//\  This program is distributed in the hope that it will be useful,
//\  but WITHOUT ANY WARRANTY; without even the implied warranty of
//\  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//\  GNU General Public License for more details.
//\
//\  You should have received a copy of the GNU General Public License
//\  along with this program; if not, write to the Free Software
//\  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//\
//\  Revision History
//\  0.3.3 Added NewsGator Online Services, RSS Bandit, Python Desktop Server, and others

var ESCAPED = 1;
var UNESCAPED = 2;

var aggregators = new Array(
	"AmphetaDesk", "http://127.0.0.1:8888/index.html?add_url=", ESCAPED,
	"Awasu", "http://127.0.0.1:2604/subscribe?url=", ESCAPED,
	"Bloglines", "http://www.bloglines.com/sub/", UNESCAPED,
	"Bot A Blog", "http://www.botablog.com/botthisblog.php?blog=", ESCAPED,
	"BottomFeeder", "http://127.0.0.1:8666/btf?rss=", ESCAPED,
	"Fyuze", "http://fyuze.com/customize/clickthru.php?url=", ESCAPED,
	"Headline Viewer", "http://127.0.0.1:8900/add_provider?url=", ESCAPED,
	"mobilerss", "http://www.mobilerss.net/fastfeed.php?url=", ESCAPED,
	"NetNewsWire", "feed:", UNESCAPED,
	"NewsGator Online Services", "http://services.newsgator.com/subscriber/subext.aspx?url=", ESCAPED,
	"NewsIsFree", "http://www.newsisfree.com/sources/info/?url=", ESCAPED,
	"NewsMonster", "newsmonster-subscription:", UNESCAPED,
	"nntp//rss", "http://127.0.0.1:7810/?action=addform&URL=", ESCAPED,
	"Python Desktop Server", "http://127.0.0.1:4334/aggregator/add_redir?url=", ESCAPED,
	"Radio UserLand", "http://127.0.0.1:5335/system/pages/subscriptions?url=", ESCAPED,
	"RSS Bandit", "feed:", UNESCAPED,
	"SharpReader", "http://127.0.0.1:5335/system/pages/subscriptions?url=", ESCAPED,
	"Shrook", "feed:", UNESCAPED,
	"Syndirella", "http://127.0.0.1:5335/system/pages/subscriptions?url=", ESCAPED,
	"Vox Lite", "feed:", UNESCAPED,
	"Wildgrape", "http://127.0.0.1:8888/NewsDesk.html?add_url=", ESCAPED,
	"WINKsite", "http://winksite.com/site/quick_add_logon_form.cfm?partner_name=&feed_url=", ESCAPED,
	"WinRSS", "rss://addrss/", ESCAPED,
	"Others...", "http://www.methodize.org/quicksub/subscribe.php?url=", ESCAPED
);

var txtSubscribe = 'Subscribe to this feed:';
var txtContentHeader = 'Newsreader/Aggregator:';
var txtWhatIsThis = 'What is this?';
var txtVersion = "0.3.3";

// Color and font settings for Netscape 4 (CSS Style Sheet used for modern browsers)
var q_hdrfont = 'Verdana';
var q_hdrsize = '1';
var q_hdrcolor = '#FFFFFF';
var q_bgcolor = '#006699';
var q_contentfont = 'Verdana';
var q_contentsize = '1';
var q_contentbgcolor = '#EEEEEE';

// Internal vars
var q_frame = self;
var over = null;
var oTimer;

// Determine browser type
var ns4 = (navigator.appName == 'Netscape' && parseInt(navigator.appVersion) == 4);
var ns6 = (document.getElementById)? true:false;
var ie4 = (document.all)? true:false;
if (ie4) {
	var docRoot = 'document.body';
}

if (ns4) {
	var oW = window.innerWidth;
	var oH = window.innerHeight;
	window.onresize = function () {if (oW!=window.innerWidth||oH!=window.innerHeight) location.reload();}
}

if (ie4) {
	if ((navigator.userAgent.indexOf('MSIE 5') > 0) || (navigator.userAgent.indexOf('MSIE 6') > 0)) {
		if(document.compatMode && document.compatMode == 'CSS1Compat') {
			docRoot = 'document.documentElement';
		}
	}
	if (ns6) {
		ns6 = false;
	}
}

// quicksub(url)
// Loads parameters into global runtime variables.
function quicksub(oLink, originalUrl) {

	var url = escape(originalUrl);

	var text = '<b>' + txtContentHeader + '</b><br>';
	for(var iCount = 0; iCount < aggregators.length; iCount +=3) {
		text += ('<a href=\''
			+ aggregators[iCount + 1]
			+ (aggregators[iCount + 2] == ESCAPED ? url : originalUrl)
			+ '\'>'
			+ aggregators[iCount]
			+ '</a><br>');
	}

	if ( (ns4) || (ie4) || (ns6) ) {
		q_frame = self;
		if (ns4) over = q_frame.document.quickSub
		if (ie4) over = q_frame.quickSub.style
		if (ns6) over = q_frame.document.getElementById("quickSub");
	}

	if(!ns4) {
		var layerhtml = '<div class=\'qsheader\'>' + txtSubscribe + '&nbsp;&nbsp;<A HREF=\"javascript:return closeqs();\" onMouseOver=\"return closeqs();\">[<b>x</b>]</A></div>'
			+ '<div class=\'qscontent\'>' + text + '</div>'
			+ '<div class=\'qsfooterWhat\'><a class=\'quicksub\' target=\'quickSub\' href=\'http://www.methodize.org/quicksub/whatisthis.html\' onclick=\'return closeqs();\'>[' + txtWhatIsThis + ']</a></div>'
			+ '<div class=\'qsfooter\'><a class="quicksub" target="quickSub" href=\'http://www.methodize.org/quicksub/\' onclick=\'return closeqs();\'>quickSub ' + txtVersion + '</a></div>';
	} else {
		var closing = "<TD ALIGN=RIGHT><A HREF=\"javascript:return closeqs();\" onMouseOver=\"return closeqs();\"><FONT COLOR=\""+q_hdrcolor+"\" FACE=\""+q_hdrfont+"\" SIZE=\""+q_hdrsize+"\">[<b>x</b>]</FONT></A></TD>";
		var layerhtml = "<TABLE BORDER=0 CELLPADDING=1 CELLSPACING=0 BGCOLOR=\'"+q_bgcolor+"\'><TR><TD>"
			+"<TABLE WIDTH=100% BORDER=0 CELLPADDING=0 CELLSPACING=0><TR><TD>"
			+"<B><FONT COLOR=\""+q_hdrcolor+"\" FACE=\""+q_hdrfont+"\" SIZE=\""+q_hdrsize+"\">" + txtSubscribe + "</FONT></B></TD>"+closing+"</TR></TABLE>"
			+"<TABLE WIDTH=100% BORDER=0 CELLPADDING=2 CELLSPACING=0 BGCOLOR=\'"+ q_contentbgcolor + "\'\">"
			+"<TR><TD VALIGN=TOP><FONT FACE=\""+q_contentfont+"\" SIZE=\""+q_contentsize+"\">"+text+"</FONT></TD></TR>"
			+"<TR><TD VALIGN=TOP ALIGN=RIGHT><FONT FACE=\""+q_contentfont+"\" SIZE=\""+q_contentsize+"\"><a target=\'quickSub\' href=\'http://www.methodize.org/quicksub/whatisthis.html\' onclick=\'return closeqs();\'>[" + txtWhatIsThis + "]</a></FONT></TD></TR>"
			+"</TABLE>"
			+"<TABLE WIDTH=100% BORDER=0 CELLPADDING=0 CELLSPACING=0><TR><TD ALIGN=RIGHT>"
			+"<B><a target='quickSub' href=\'http://www.methodize.org/quicksub/\' onclick=\'return closeqs();\'><FONT COLOR=\""+q_hdrcolor+"\" FACE=\""+q_hdrfont+"\" SIZE=\""+q_hdrsize+"\">quickSub " + txtVersion + "</FONT></a></TD></TR></TABLE>"
			+"</TD></TR></TABLE>";
	}
	layerWrite(layerhtml);
	placeLayer(oLink);
	showObject(over);

	return true;
}

function closeqs() {
	if (over != null) hideObject(over);
	return true;
}

function placeLayer(oLink) {
	var placeX, placeY, iOffset, aPosition;

	aPosition = findPosition(oLink);
	placeX = eval(aPosition[0]);
	placeY = eval(aPosition[1]) + 20;

	if ( (ns4) || (ie4) ) {
		over.left = (ie4 ? placeX + 'px' : placeX);
		over.top = (ie4 ? placeY + 'px' : placeY);
	} else if (ns6) {
		over.style.left = placeX + "px";
		over.style.top = placeY + "px";
	}
}

function layerWrite(txt) {
	if (ns4) {
		var lyr = q_frame.document.quickSub.document
		lyr.write(txt)
		lyr.close()
	} else if (ie4) {
		q_frame.document.all["quickSub"].innerHTML = txt
	} else if (ns6) {
		range = q_frame.document.createRange();
		range.setStartBefore(over);
		domfrag = range.createContextualFragment(txt);
		while (over.hasChildNodes()) {
			over.removeChild(over.lastChild);
		}
		over.appendChild(domfrag);
	}
}

function showObject(obj) {
	if (ns4) obj.visibility = "show";
	else if (ie4) obj.visibility = "visible";
	else if (ns6) obj.style.visibility = "visible";
}

function hideObject(obj) {
	if (ns4) obj.visibility = "hide";
	else if (ie4) obj.visibility = "hidden";
	else if (ns6) obj.style.visibility = "hidden";
}

function findPosition (oLink) {
	if (oLink.offsetParent) {
		for (var iPosX = 0, iPosY = 0; oLink.offsetParent; oLink = oLink.offsetParent) {
			iPosX += oLink.offsetLeft;
			iPosY += oLink.offsetTop;
		}
		return [iPosX, iPosY];
	} else {
		return [oLink.x, oLink.y];
	}
}

function timeqs () {
	if(!ns4) {
		var iDelay = 500;

		if (oTimer) {
			clearTimeout(oTimer);
		}

		if ((ns4 && over.visibility != 'hide') ||
			(ie4 && over.visibility != 'hidden') ||
			(ns6 && over.style.visibility != 'hidden')) {
			oTimer = setTimeout(hideqs, iDelay);
		}
	}
}

function hideqs () {
	if (oTimer) {
		clearTimeout(oTimer);
	}

	closeqs();
}

function delayqs () {
	if (oTimer) {
		clearTimeout(oTimer);
	}
}