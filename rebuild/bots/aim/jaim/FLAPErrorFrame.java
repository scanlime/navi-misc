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
 * FlapErrorFrame.java
 *
 * Created on 3 May 2002, 14:54
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class FLAPErrorFrame extends FLAPFrame {

    /** Creates new FLAPErrorFrame */
    public FLAPErrorFrame() {
         frame[1]=FLAP_FRAME_ERROR;

    }
    
    public FLAPErrorFrame(byte frameData[])
    {
         frame[1]=FLAP_FRAME_ERROR;
        setFrameData(frameData);
    }
    
    
    public int getFLAPFrameType() {
        return FLAPFrame.FLAP_FRAME_ERROR;
    }
    
}
