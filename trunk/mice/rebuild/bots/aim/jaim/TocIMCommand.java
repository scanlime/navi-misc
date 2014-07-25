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
 * TocIMCommand.java
 *
 * Created on 4 May 2002, 15:18
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.4 $
 */
public class TocIMCommand extends TocCommand {

    private String recipient;
    private String msg;
    private String auto;
    
    /** Creates new TocIMCommand */
    public TocIMCommand(String recipient, String msg,boolean autoMessage) {
        this.recipient=Utils.normalise(recipient);
        this.msg=Utils.encodeText(msg);
        if (autoMessage)
            auto=" auto";
        else
            auto="";
    }

    public String toString()
    {
        return ("toc_send_im "+recipient+" "+msg+auto);
    }
    
    public byte[] getBytes() {
        return(this.toString().getBytes());
    }
    
}
