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
 * FLAPFrame.java
 *
 * Created on 3 May 2002, 14:51
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.4 $
 */
public abstract class FLAPFrame {

    public static final int FLAP_FRAME_SIGNON=1;
    public static final int FLAP_FRAME_DATA=2;
    public static final int FLAP_FRAME_ERROR=3;
    public static final int FLAP_FRAME_SIGNOFF=4;
    public static final int FLAP_FRAME_KEEP_ALIVE=5;
    public static final int FLAP_DATA_OFFSET=6;
    
    protected byte[] frame;
    protected int fLen;
    
    /** Creates new FLAPFrame */
    public FLAPFrame() {
        initialise();
    }
    
    protected void setFrameData(byte b[])
    {
        frame=new byte[b.length];
        fLen=b.length;
        System.arraycopy(b,0,frame,0,b.length);
    }
    
    protected void initialise()
    {
        frame = new byte[8192];
        frame[0]=(byte)'*';
	frame[1]=0;
	frame[2]=0;
	frame[3]=0;
	frame[4]=0;
	frame[5]=0;
        fLen=6;
        
    }
    
    public void setSequence(int sequence)
    {
        frame[2]=(byte)((sequence/256)&0xff);
        frame[3]=(byte)(sequence&0xff);
    }
    
    public int getSequence()
    {
        return((frame[2]&0xff)*256+(frame[3]&0xff));
    }
    
    public int getLength()
    {
        return((frame[4]&0xff)*256+(frame[5]&0xff));
    }
    
    public void setLength(int length)
    {
        frame[4]=(byte)(length/256);
        frame[5]=(byte)(length&0xff);
        fLen=length+FLAP_DATA_OFFSET;
    }
    
    public byte[] getFrameData()
    {
        byte[] b=new byte[fLen];
        System.arraycopy(frame,0,b,0,fLen);
        return(b);
    }

    public String toString()
    {
        StringBuffer temp=new StringBuffer();
        for (int i=0;i<fLen;i++)
	{
		int k=frame[i]&0xff;
		if (k<16)
		{
			temp.append("0"+Integer.toHexString(k)+" ");
		}
		else
		{
			temp.append(Integer.toHexString(k)+" ");
		}
	}
	return(temp.toString());
}
    public abstract int getFLAPFrameType();
    
}
