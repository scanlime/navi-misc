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
 * FlapSignonFrame.java
 *
 * Created on 3 May 2002, 14:54
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class FLAPSignonFrame extends FLAPFrame {

    /** Creates new FlapSignonFrame */
    public FLAPSignonFrame() {
        frame[1]=FLAP_FRAME_SIGNON;
    }
    
    public FLAPSignonFrame(byte frameData[])
    {
        frame[1]=FLAP_FRAME_SIGNON;
        setFrameData(frameData);
    }
    
    public int getFLAPVersion()
    {
        return(((frame[6]&0xff)*16777216)+((frame[7]&0xff)*65536)+((frame[8]&0xff)*256)+(frame[9]&0xff));
    }
    
    public void setFLAPVersion(int version)
    {
        for (int i=3;i>=0;i--)
        {
            frame[6+i]=(byte)(version&0xff);
            version=version>>8;
        }
    }
    
    public void setTLVTag(int tag)
    {
        for (int i=1;i>=0;i--)
        {
            frame[10+i]=(byte)(tag&0xff);
            tag=tag>>8;
        }
    }
    
    public void setUserName(String name)
    {
       
        int len=0;
        for (int i=0;i<name.length();i++)
        {   
           char c = name.charAt(i);
           if (c != ' ')
           {
               frame[FLAP_DATA_OFFSET+8+len++]=(byte)c;
           }
        }
        setLength(8+len);
        frame[FLAP_DATA_OFFSET+6]=(byte)(len/256);
        frame[FLAP_DATA_OFFSET+7]=(byte)(len&0xff);
    }
                  
    public int getFLAPFrameType() {
        return(FLAPFrame.FLAP_FRAME_SIGNON);
    }
    
}