/*
 * TextUtils.cpp - a collection of handy functions for dealing with strings
 *
 * Much of this code is derived from BZFlag
 * Copyright (C) 1993 - 2005 Tim Riker
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "TextUtils.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <vector>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32

/* On windows, strcasecmp is really strcmp */
#define HAVE_STRICMP 1
/* Define to 1 if you have the `_stricmp' function */
#define HAVE__STRICMP 1
/* Define to 1 if you have the `_strnicmp' function */
#define HAVE__STRNICMP 1
/* Define to 1 if you have the `_vsnprintf' function */
#define HAVE__VSNPRINTF 1

#endif

#ifdef HAVE__STRICMP
#	define strcasecmp _stricmp
#endif
#ifdef HAVE__STRNICMP
#	define strncasecmp _strnicmp
#endif
#if !defined(HAVE_VSNPRINTF)
#	ifdef HAVE__VSNPRINTF
#		define vsnprintf _vsnprintf
#	else
#		define vsnprintf(buf, size, fmt, list) vsprintf(buf, fmt, list)
#	endif
#endif

namespace TextUtils
{

std::string vformat (const char* fmt, va_list args)
{
	const int fixedbs = 8192;
	char buffer[fixedbs];
	const int bs = vsnprintf(buffer, fixedbs, fmt, args) + 1;
	if (bs > fixedbs) {
		char *bufp = new char[bs];
		vsnprintf(bufp, bs, fmt, args);
		std::string ret = bufp;
		delete[] bufp;
		return ret;
	}

	return buffer;
}


std::string format (const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	std::string result = vformat (fmt, args);
	va_end (args);
	return result;
}

std::string replace_all (const std::string& in, const std::string& replaceMe, const std::string& withMe)
{
	std::string result;
	std::string::size_type beginPos = 0;
	std::string::size_type endPos = 0;
	std::ostringstream tempStream;

	if (replaceMe.empty ())
		return in; // can't replace nothing with something -- can do reverse
	endPos = in.find (replaceMe);
	if (endPos == std::string::npos)
		return in; // can't find anything to replace

	while (endPos != std::string::npos) {
		tempStream << in.substr (beginPos, endPos - beginPos);
		tempStream << withMe;
		beginPos = endPos + replaceMe.size();
		endPos = in.find(replaceMe,beginPos);
	}
	tempStream << in.substr(beginPos);
	return tempStream.str();
}

std::string no_whitespace (const std::string &s)
{
	const int sourcesize = (int) s.size ();

	int count = 0, i = 0, j = 0;
	for (i = 0; i < sourcesize; i++)
		if (!isWhitespace (s[i]))
			count++;

	// create result string of correct size
	std::string result(count, ' ');

	for (i = 0, j = 0; i < sourcesize; i++)
		if (!isWhitespace(s[i]))
			result[j++] = s[i];

	return result;
}


std::vector<std::string> tokenize (const std::string& in, const std::string &delims, const int maxTokens, const bool useQuotes)
{
	std::vector<std::string> tokens;
	int numTokens = 0;
	bool inQuote = false;

	std::ostringstream currentToken;

	std::string::size_type pos = in.find_first_not_of(delims);
	int currentChar  = (pos == std::string::npos) ? -1 : in[pos];
	bool enoughTokens = (maxTokens && (numTokens >= (maxTokens-1)));

	while (pos != std::string::npos && !enoughTokens) {

		// get next token
		bool tokenDone = false;
		bool foundSlash = false;

		currentChar = (pos < in.size()) ? in[pos] : -1;
		while ((currentChar != -1) && !tokenDone) {
			tokenDone = false;

			if (delims.find(currentChar) != std::string::npos && !inQuote) {
				// currentChar is a delim
				pos ++;
				break; // breaks out of while look
			}

			if (!useQuotes) {
				currentToken << char(currentChar);
			} else {
				switch (currentChar) {
				case '\\' :
					// found a backslash
					if (foundSlash) {
						currentToken << char(currentChar);
						foundSlash = false;
					} else {
						foundSlash = true;
					}
					break;
				case '\"' :
					// found a quote
					if (foundSlash) {
						// found \"
						currentToken << char(currentChar);
						foundSlash = false;
					} else {
						// found unescaped "
						if (inQuote) {
							// exiting a quote
							// finish off current token
							tokenDone = true;
							inQuote = false;
							//slurp off one additional delimeter if possible
							if (pos+1 < in.size() && delims.find(in[pos+1]) != std::string::npos)
								pos++;
						} else {
							// entering a quote
							// finish off current token
							tokenDone = true;
							inQuote = true;
						}
					}
					break;
				default:
					if (foundSlash) {
						// don't care about slashes except for above cases
						currentToken << '\\';
						foundSlash = false;
					}
					currentToken << char(currentChar);
					break;
				}
			}

			pos++;
			currentChar = (pos < in.size()) ? in[pos] : -1;
		} // end of getting a Token

		if (currentToken.str ().size () > 0) {
			// if the token is something add to list
			tokens.push_back (currentToken.str ());
			currentToken.str ("");
			numTokens ++;
		}

		enoughTokens = (maxTokens && (numTokens >= (maxTokens-1)));
		if (enoughTokens)
			break;
		else
			pos = in.find_first_not_of(delims,pos);
	} // end of getting all tokens -- either EOL or max tokens reached

	if (enoughTokens && pos != std::string::npos) {
		std::string lastToken = in.substr(pos);
		if (lastToken.size () > 0)
			tokens.push_back(lastToken);
	}

	return tokens;
}

int parseDuration (const std::string &duration)
{
	int durationInt = 0;
	int t = 0;

	int len = (int)duration.length();
	for (int i=0; i < len; i++) {
		if (isdigit(duration[i])) {
			t = t * 10 + (duration[i] - '0');
		} else if(duration[i] == 'h' || duration[i] == 'H') {
			durationInt += (t * 60);
			t = 0;
		} else if(duration[i] == 'd' || duration[i] == 'D') {
			durationInt += (t * 1440);
			t = 0;
		} else if(duration[i] == 'w' || duration[i] == 'w') {
			durationInt += (t * 10080);
			t = 0;
		}
	}
	durationInt += t;
	return durationInt;
}

std::string url_encode (const std::string &text)
{
	char hex[5];
	std::string destination;
	for (int i=0;  i < (int) text.size (); i++) {
		char c = text[i];
		if (isAlphanumeric(c)) {
			destination+=c;
		} else if (isWhitespace(c)) {
			destination+='+';
		} else {
			destination+='%';
			sprintf(hex, "%-2.2X", c);
			destination.append(hex);
		}
	}
	return destination;
}


std::string escape (const std::string &text, char escaper)
{
	std::string destination;
	for (int i = 0; i < (int) text.size (); i++) {
		char c = text[i];
		if (!isAlphanumeric (c))
			destination += escaper;
		destination += c;
	}
	return destination;
}

std::string unescape (const std::string &text, char escaper)
{
	const int len = (int) text.size ();
	std::string destination;
	for (int i = 0; i < len; i++) {
		char c = text[i];
		if (c == escaper) {
			if (i < len - 1)
				destination += text[++i];
			else
				std::cerr << "Found escape character at end of string\n";
		} else {
			destination += c;
		}
	}
	return destination;
}

int unescape_lookup (const std::string &text, char escaper, char sep)
{
	int position = -1;
	for (int i = 0; i < (int) text.size(); i++) {
		char c = text[i];
		if (c == sep) {
			position = i;
			break;
		}
		if (c == escaper)
			i++;
		}
		return position;
	}
}
