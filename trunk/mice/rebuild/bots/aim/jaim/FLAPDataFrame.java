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
 * FlapDataFrame.java
 *
 * Created on 3 May 2002, 14:54
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class FLAPDataFrame extends FLAPFrame {

    private int frameLen;
    
    /** Creates new FlapDataFrame */
    public FLAPDataFrame() {
          frame[1]=FLAP_FRAME_DATA;
        frameLen=1;
        frame[FLAP_DATA_OFFSET]=0;
    }
    
    public FLAPDataFrame(byte frameData[])
    {
        frame[1]=FLAP_FRAME_DATA;
        frameLen=1;
        frame[FLAP_DATA_OFFSET]=0;
        setFrameData(frameData);
    }
    
    
    public int getFLAPFrameType() {
        return(FLAPFrame.FLAP_FRAME_DATA);
    }
    
    public void addString(String s)
    {
        frameLen--;     // Backspace over '0'
        for (int i=0;i<s.length();i++)
        {
            frame[FLAP_DATA_OFFSET+frameLen++]=(byte)s.charAt(i);
        }
        frame[FLAP_DATA_OFFSET+frameLen++]=0;
        setLength(frameLen);
    }
    
    public byte[] getContent()
    {
        byte[] retarray = new byte[getLength()];
        
        System.arraycopy(frame,FLAPFrame.FLAP_DATA_OFFSET,retarray,0,getLength());
        return(retarray);
    }
    
}
