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
 * FLAPInputFrame.java
 *
 * Created on 3 May 2002, 15:52
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class FLAPInputFrame extends FLAPFrame {

    /** Creates new FLAPInputFrame */
    private int frameLen; 
    
    public FLAPInputFrame() {
        frameLen=0;
        super.initialise();
    }
    
    public void addFrameData(byte b)
    {
        frame[frameLen++]=b;
    }

    public byte[] getFrameData()
    {
        byte[] b=new byte[frameLen];
        System.arraycopy(frame,0,b,0,frameLen);
        return(b);
    }
    
    public void resetInputFrame()
    {
        frameLen=0;
    }
    
    public boolean completeFrameRead()
    {
        if (frameLen > 5)
        {
            if (frameLen-6 == getLength())
            {
                return(true);
            }
        }
        return(false);
    }
    
    public int getFLAPFrameType() {
        return(-1);
    }
    
}
