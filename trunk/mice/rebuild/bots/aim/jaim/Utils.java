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
 * Utils.java
 *
 * Created on 3 May 2002, 17:19
 */

package jaim;

/**
 *
 * @author paulw
 * @version $Revision: 1.4 $
 */
public class Utils {

    private static final String roastKey="Tic/Toc";
    private static final int roastLen=7;

    /** convert a buddy name to normalised format - remove spaces and convert to lower case
     * @param input The un-normalised buddy name
     * @return the normalised buddy name
     */    
    public static String normalise(java.lang.String input) {
        StringBuffer output=new StringBuffer();
        String temp=input.toLowerCase();
        for (int i=0;i<input.length();i++)
        {
            char c=temp.charAt(i);
            if ((c>= '0' && c<='9')||(c>='a' && c<='z'))
            {
                output.append(c);
            }
        }
        
        return(output.toString());
    }
    
    /** Roast a password using the AOL roasting protocol
     * @param password The password to be roasted
     * @return The roasted password
     */    
    public static String roast(java.lang.String password) {
        char[] hexChars={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
        
        StringBuffer temppw=new StringBuffer();
        temppw.append("0x");
        for (int i=0;i<password.length();i++)
        {
            int roastedByte=password.charAt(i)^roastKey.charAt(i%roastLen);
            
            temppw.append(hexChars[(roastedByte>>4)&0x0f]);
            temppw.append(hexChars[roastedByte&0x0f]);
        }
        return(temppw.toString());
    }
    
    /** This method performs a simple HTML strip on text.  It looks for < characters and then skips input until a matching > is found.
     * This may fail if the HTML tag contains an embedded '>'
     * @param input The text to have HTML stripped
     * @return The text stripped of html
     */    
    public static String stripHTML(java.lang.String input)
    {
        StringBuffer output=new StringBuffer();
        boolean inHTML=false;
        for (int i=0;i<input.length();i++)
        {
            char c=input.charAt(i);
            if (c=='<')
            {
                inHTML=true;
            }
            else
            {
                if (c=='>') {
                    inHTML=false;
                }
                else
                {
                    if (!inHTML)
                    {
                        output.append(c);
                    }
                }
            }
        }
        return(output.toString());
    }
    
    
    /** Encode a text message so that it is suitable for transmission using toc_send_im
     *
     * @param input The text to be encoded
     * @return The encoded text
     */    
    public static String encodeText(String input)
    {
        StringBuffer output=new StringBuffer("\"");
        for (int i=0;i<input.length();i++)
        {
            char c=input.charAt(i);
            switch (c)
            {
                case '\"':
                case '(':
                case ')':
                case '$':
                case '\\':
                case '{':
                case '}':
                case '[':
                case ']':
                    output.append('\\');
                    break;
            }
            output.append(c);
        }
         
        output.append('\"');
        return(output.toString());
    }
        
    
}
