/* 
* Copyright (c) 2002 - 2003 Jeffrey Myers
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named license.txt that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "bznext.h"

#ifdef _WIN32
#include <windows.h>
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
  srand(GetTickCount());
  CBZNextLoop  loop;
  if (loop.Main(lpCmdLine))
    return 1;
  return 0;
}
#else
int main (int argc, char **argv)
{
  srand(time((time_t *)NULL));
  CBZNextLoop  loop;
  if (loop.Main(argc,argv))
    return 1;
  return 0;
}
#endif // WIN32
