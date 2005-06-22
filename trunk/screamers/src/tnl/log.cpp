//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlLog.h"
#include "tnlDataChunker.h"
#include <stdarg.h>

namespace TNL
{

LogConsumer *LogConsumer::mLinkedList = NULL;

LogConsumer::LogConsumer()
{
   mNextConsumer = mLinkedList;
   if(mNextConsumer)
      mNextConsumer->mPrevConsumer = this;
   mPrevConsumer = NULL;
   mLinkedList = this;
}

LogConsumer::~LogConsumer()
{
   if(mNextConsumer)
      mNextConsumer->mPrevConsumer = mPrevConsumer;
   if(mPrevConsumer)
      mPrevConsumer->mNextConsumer = mNextConsumer;
   else
      mLinkedList = mNextConsumer;
}

LogType *LogType::linkedList = NULL;
LogType *LogType::current = NULL;

#ifdef TNL_ENABLE_LOGGING

LogType *LogType::find(const char *name)
{
   static ClassChunker<LogType> logTypeChunker(4096);

   for(LogType *walk = linkedList; walk; walk = walk->next)
      if(!strcmp(walk->typeName, name))
         return walk;
   LogType *ret = logTypeChunker.alloc();
   ret->next = linkedList;
   linkedList = ret;
   ret->isEnabled = false;
   ret->typeName = name;
   return ret;
}
#endif

void LogConsumer::logString(const char *string)
{
   // by default the LogConsumer will output to the platform debug 
   // string printer, but only if we're in debug mode
#ifdef TNL_DEBUG
   Platform::outputDebugString(string);
   Platform::outputDebugString("\n");
#endif
}

void logprintf(const char *format, ...)
{
   char buffer[4096];
   U32 bufferStart = 0;
   if(LogType::current)
   {
      strcpy(buffer, LogType::current->typeName);
      bufferStart = strlen(buffer);

      buffer[bufferStart] = ':';
      buffer[bufferStart+1] = ' ';
      bufferStart += 2;
   }
   va_list s;
   va_start( s, format );
   dVsprintf(buffer + bufferStart, sizeof(buffer) - bufferStart, format, s);
   for(LogConsumer *walk = LogConsumer::getLinkedList(); walk; walk = walk->getNext())
      walk->logString(buffer);
   va_end(s);
   Platform::outputDebugString(buffer);
   Platform::outputDebugString("\n");
}

};

