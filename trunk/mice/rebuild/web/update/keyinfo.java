/**
 * Makes the key
 */

public class keyinfo
{
    public char code;
    public keyinfo next;
    
    public keyinfo(char in, keyinfo prev)
    {
        code = in;
        next = prev;
    }
    
    public String toString()
    {
        if(next != null)
            return code + "" + next.toString();
        else
            return code + "";
    }
}