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
 * FLAPFrameException.java
 *
 * Created on 3 May 2002, 15:05
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class FLAPFrameException extends java.lang.Exception {

    /**
     * Creates new <code>FLAPFrameException</code> without detail message.
     */
    public FLAPFrameException() {
    }


    /**
     * Constructs an <code>FLAPFrameException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public FLAPFrameException(String msg) {
        super(msg);
    }
}


