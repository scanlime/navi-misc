/*
 * TextUtils.h - a collection of handy functions for dealing with strings
 *
 * Much of this code is derived from BZFlag, Copyright (C) 1993 - 2005 Tim Riker
 * The rest Copyright (C) 2005 Screamers Group (see AUTHORS)
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

#ifndef _TEXTUTILS_H_
#define	_TEXTUTILS_H_

/* system interface headers */
#include <algorithm>
#include <ctype.h>
#ifdef HAVE_DEFINED_TOLOWER
#undef tolower
#undef toupper
#endif
#include <string>
#include <stdarg.h>
#include <vector>

/** This namespace provides basic functionality to parse and
 * format strings
 */
namespace TextUtils {

std::string vformat (const char* fmt, va_list args);
std::string format (const char* fmt, ...);

// returns a string converted to lowercase
inline std::string
tolower (const std::string& s)
{
	std::string trans = s;

	for (std::string::iterator i = trans.begin (), end = trans.end (); i != end; ++i)
		*i = ::tolower (*i);
	return trans;
}

// returns a string converted to uppercase
inline std::string
toupper (const std::string& s)
{
	std::string trans = s;
	for (std::string::iterator i = trans.begin (), end = trans.end(); i != end; ++i)
		*i = ::toupper(*i);
	return trans;
}

// replace all of in in replaceMe with withMe
std::string replace_all (const std::string& in, const std::string& replaceMe, const std::string& withMe);

// return copy of string with all whitespace stripped
std::string no_whitespace (const std::string &s);

/*
 * Get a vector of strings from a string, using all of characters in the delims
 * string as separators. If maxTokens > 0, then the last 'token' may contain
 * delimiters as it just returns the rest of the line.  If you specify useQuotes
 * then tokens can be grouped in quotes and delimeters inside quotes are ignored.
 * Hence /ban "Mr Blahblah" isajerk parses to "/ban", "Mr Blahlah" and "isajerk"
 * but so does "Mr Blahblah" "isajerk", so if you want 3 tokens and a delimeter
 * is in one of the tokens, by putting quotes around it you can get the correct
 * parsing.  When useQuotes is enabled, \'s and "'s should/can be escaped with \
 * Escaping is not currently done for any other character.
 * Should not have " as a delimeter if you want to use quotes
 */
std::vector<std::string> tokenize (const std::string& in, const std::string &delims, const int maxTokens = 0, const bool useQuotes = false);

/* Convert a string represantation of some duration into minutes
 *	example: "1d2h16m" -> 1500
 */
int parseDuration (const std::string &duration);

/* compare_nocase is straight from Stroustrup.  This implementation uses
 * strings instead of char arrays and includes a maxlength bounds check.
 * It compares two strings and returns 0 if equal, <0 if s1 is less than
 * s2, and >0 if s1 is greater than s2.
 */
inline int
compare_nocase (const std::string& s1, const std::string &s2, int maxlength = 4096)
{
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	int i = 0;
	while (p1 != s1.end () && p2 != s2.end ()) {
		if (i >= maxlength)
			return 0;
		if (::tolower (*p1) != ::tolower (*p2))
			return (::tolower(*p1) < ::tolower(*p2)) ? -1 : 1;
		++p1;
		++p2;
		++i;
	}
	return (s2.size() == s1.size()) ? 0 : (s1.size() < s2.size()) ? -1 : 1; // size is unsigned
}



// Utility function returns whether given character is a letter.
inline bool
isAlphabetic (const char c)
{
	return ((c > 64 && c < 91) || (c > 96 && c < 123));
}


// Utility function returns whether given character is a number.
inline bool
isNumeric (const char c)
{
	return ((c > 47 && c < 58));
}


/* Utility function returns whether a given character is printable whitespace.
 * This includes newline, carriage returns, tabs and spaces.
 */
inline bool
isWhitespace (const char c)
{
	return ((( c >= 9 ) && ( c <= 13 )) || (c == 32));
}


// Utility function returns whether a given character is punctuation.
inline bool
isPunctuation (const char c)
{
	return ((c > 32 && c < 48) ||
		(c > 57 && c < 65) ||
		(c > 90 && c < 97) ||
		(c > 122 && c < 127));
}


// Utility function returns whether given character is an alphanumeric.
// This is strictly letters and numbers.
inline bool
isAlphanumeric (const char c)
{
	return (isAlphabetic(c) || isNumeric(c));
}


/* Utility function returns whether given character is printable.  This
 * includes letters, numbers, and punctuation, but not whitespace.
 */
inline bool
isVisible (const char c)
{
	return (isAlphanumeric(c) || isPunctuation(c));
}


/* Utility function returns whether given character is printable.  This
 * includes letters, numbers, punctuation, and whitespace
 */
inline bool
isPrintable (const char c)
{
	return (!(isVisible(c) || isWhitespace(c)));
}

// Utility method that returns the position of the first alphanumeric character from a string
inline int
firstAlphanumeric (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length())
		max = (unsigned short)input.length();

	for (unsigned short i = 0; i < max; i++)
		if (isAlphanumeric(input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first non-alphanumeric character from a string
inline int
firstNonalphanumeric (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length())
		max = (unsigned short)input.length();

	for (unsigned short i = 0; i < max; i++)
		if (!isAlphanumeric(input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first printable character from a string
inline int
firstPrintable (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length ())
		max = (unsigned short) input.length ();

	for (unsigned short i = 0; i < max; i++)
		if (isPrintable (input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first non-printable character from a string
inline int
firstNonprintable (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length ())
		max = (unsigned short) input.length ();

	for (unsigned short i = 0; i < max; i++)
		if (!isPrintable (input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first visible character from a string
inline int
firstVisible (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length ())
		max = (unsigned short) input.length ();

	for (unsigned short i = 0; i < max; i++)
		if (isVisible (input[i]))
			return i;

	return -1;
}


/* Utility method that returns the position of the first non visible character
 * from a string (control codes or whitespace)
 */
inline int
firstNonvisible (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length())
		max = (unsigned short) input.length ();

	for (unsigned short i = 0; i < max; i++)
		if (!isVisible (input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first alphabetic character from a string
inline int
firstAlphabetic (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length())
		max = (unsigned short)input.length();

	for (unsigned short i = 0; i < max; i++)
		if (isAlphabetic (input[i]))
			return i;

	return -1;
}


// Utility method that returns the position of the first printable character from a string
inline int
firstNonalphabetic (const std::string &input, unsigned short int max=4096)
{
	if (max > input.length ())
		max = (unsigned short) input.length ();

	for (unsigned short i = 0; i < max; i++)
		if (!isAlphabetic (input[i]))
			return i;

	return -1;
}

// url-encodes a string
std::string url_encode (const std::string &text);

// escape a string
std::string escape (const std::string &text, char escaper);

// un-escape a string
std::string unescape (const std::string &text, char escaper);

// lookup for an un-escaped separator
int unescape_lookup (const std::string &text, char escaper, char sep);

}

#endif // _TEXTUTILS_H_
