/* 
 *   (C) 2002 Paul Wilkinson  wilko@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * TocResponseFactory.java
 *
 * Created on 4 May 2002, 12:05
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.5 $
 */

import java.util.Vector;

public abstract class TocResponseFactory {

    static Vector responseHandlers = new Vector();
    
    /** Creates new TocResponseFactory */
    public TocResponseFactory() {
    }

    public static void addResponseHandler(TocResponseHandler h)
    {
        synchronized (responseHandlers)
        {
            responseHandlers.add(h);
        }
    }
    
    static TocResponse createResponse(byte[] b)
    {
        TocResponse tr=null;
        String strversion=new String(b);
        int colonpos=strversion.indexOf(':');
        if (colonpos != -1)
        {
            String firstWord=strversion.substring(0,colonpos);
            int i=0;
            synchronized (responseHandlers)
            {
                while ((i<responseHandlers.size())&&(tr==null))
                {
                    TocResponseHandler h=(TocResponseHandler)responseHandlers.elementAt(i);
                    if (h.canHandle(firstWord))
                    {
                        tr=h.parseString(strversion);
                    }
                    i++;
                }
            }
        }
        if (tr==null)
        {
            GenericTocResponse gtr=new GenericTocResponse();
            tr=gtr.parseString(strversion);
        }
        return(tr);
    }
    
}
