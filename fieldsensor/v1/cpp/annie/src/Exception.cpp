#include "../include/Exception.h"

using namespace std;

namespace annie
{

Exception::Exception(const char *info)
{	details.assign(info);	}

Exception::Exception(string info)
{	details=info;	}

// Exception::Exception(Exception &e)
// {	details.assign(e.details);	}

string
Exception::what()
{	return details;	}


}; //namespace annie

