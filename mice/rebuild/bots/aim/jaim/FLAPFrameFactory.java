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
 * FLAPFrameFactory.java
 *
 * Created on 3 May 2002, 15:04
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public abstract class FLAPFrameFactory {

    /** Creates new FLAPFrameFactory */
    public FLAPFrameFactory() {
    }

    public static FLAPFrame createFLAPFrame(byte[] frameData) throws FLAPFrameException {
        FLAPFrame f=null;
        if (frameData[0]!='*')
        {
            throw new FLAPFrameException("Frame does not start with '*'");
        }
        
        switch (frameData[1])
        {
            case FLAPFrame.FLAP_FRAME_SIGNON:
                f=new FLAPSignonFrame(frameData);
                break;
            case FLAPFrame.FLAP_FRAME_DATA:
                f=new FLAPDataFrame(frameData);
                break;
            case FLAPFrame.FLAP_FRAME_ERROR:
                f=new FLAPErrorFrame(frameData);
                break;
            case FLAPFrame.FLAP_FRAME_SIGNOFF:
                f=new FLAPSignoffFrame(frameData);
                break;
            case FLAPFrame.FLAP_FRAME_KEEP_ALIVE:
                f=new FLAPKeepAliveFrame(frameData);
                break;
            default:
                throw new FLAPFrameException("Illegal FLAP Frame type: "+Integer.toString(frameData[1]));
        }
        return(f);
    }

}
