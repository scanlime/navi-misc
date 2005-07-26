#ifndef _DIALOGDATA_H_
#define _DIALOGDATA_H_

#include <vector>
#include <string>

typedef struct 
{
	float bgColor[3];
}trFilePrefsData;

typedef struct 
{
	std::string					filename;
	std::vector<std::string>	extenstions;
	int							extenstion;
	std::string					title;
}trStandardFileOpen;

#endif //_DIALOGDATA_H_