#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <iostream>
#include <string>

namespace annie
{

/** A common exception class used by all classes in the annie library.
  *	You should place calls to functions that can throw exceptions in a try {} block,
  * and catch(Exception &e). Use cout<<e.what() to print out the details onto the screen.
  *
  * In version 1.0 of annie, this is the only class thrown by any function. Further refinements
  * will be made as and when required.
  */
class Exception
{
protected:
	std::string details;
public:
	///Creates an exception
	/** @param info A string specifying details of the error. */
	Exception(const char *info);

	///Creates an exception
	/** @param info A string specifying details of the error. */
	Exception(std::string info);

	///Returns a string consisting of the details specified when the Exception was created
	std::string what();
};

}; //namespace annie
#endif // define _EXCEPTION_H

