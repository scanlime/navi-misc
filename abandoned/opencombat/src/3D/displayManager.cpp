/* bzflag
* Copyright (c) 1993 - 2004 Tim Riker
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named COPYING that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

// display manager

#include "displayManager.h"

/*const int NO_VARIANT = (-1); */

// initialize the singleton
template <>
DisplayManager* Singleton<DisplayManager>::_instance = (DisplayManager*)0;

DisplayManager::DisplayManager()
{
}

DisplayManager::~DisplayManager()
{
}

void DisplayManager::init ( void )
{
}

// resoultion enumeration
int DisplayManager::getResCount ( void )
{
}

std::string	DisplayManager::getResName ( int res )
{
}

int DisplayManager::getBPPCount ( void )
{
}

std::string	DisplayManager::getBPPName ( int BPP )
{
}

// resolution switching
bool DisplayManager::setRes ( int res, int BPP, bool fullScreen )
{
}

bool DisplayManager::setRes ( int h, int v, int BPP, bool fullScreen )
{
}

bool DisplayManager::setRes ( std::string res, std::string BPP, bool fullScreen )
{
}

bool DisplayManager::setFullScreen ( bool fullScreen )
{
}

// info methods
int DisplayManager::getWidth ( void )
{
}

int DisplayManager::getHeight ( void )
{
}

int DisplayManager::getBPP ( void )
{
}

int DisplayManager::getResMode ( void )
{
}

int DisplayManager::getBPPMode ( void )
{
}

// state stuff
void DisplayManager::activate ( void )
{
}

void DisplayManager::deactivate ( void )
{
}


void DisplayManager::enable ( void )
{
}

void DisplayManager::yeld ( void )
{
}

void DisplayManager::release ( void )
{
}

void DisplayManager::flip ( void )
{
}

// gamma
void	DisplayManager::setGamma ( float gamma )
{
}

float	DisplayManager::getGamma ( void )
{
}


