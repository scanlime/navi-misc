/**
 * generates the keys and decrypts the keys
 */
 
public class key
{
    public key(){}
    
    public static String keygen()
    {
        String newkey = "";
        keyinfo start, end;
        char i;
        int j;
        end = new keyinfo('0',null);
        start = end;
        for(i = '1'; i<='9';i++)
            start = new keyinfo(i,start);
        for(i = 'A'; i<='Z';i++)
            start = new keyinfo(i,start);
        for(i = 'a'; i<='z';i++)
            start = new keyinfo(i,start);
        end.next = start;
        while(start != start.next)
        {
            j = (int) (Math.random() * 64);
            for(i=0;i<j;i++) start = start.next;
            newkey = newkey + start.next.code;
            start.next = start.next.next;
        }
        newkey = newkey + start.code;
        return newkey;
    }
    
    public static String encrypt(String mykey, String password)
    {
        String toreturn = "";
        int i;
        int index;
        if(mykey.length() != 62) return null;
        for(i=0;i<password.length();i++)
        {
            index = password.charAt(i);
            if(index >= '0' && index <= '9') index -= '0';
            if(index >= 'A' && index <= 'Z') index = index - 'A' + 10;
            if(index >= 'a' && index <= 'z') index = index - 'a' + 36;
            toreturn = toreturn + mykey.charAt(index);
        }
        return toreturn;
    }
    
    public static String decrypt(String mykey, String encrypted)
    {
        String toreturn = "";
        int i;
        char j;
        char character;
        char alpha = 0;
        if(mykey.length() != 62) return null;
        for(i=0;i<encrypted.length();i++)
        {
            character = encrypted.charAt(i);
            j = 0;
            while(character != mykey.charAt(j)) j++;
            if(j >=  0 && j <=  9) alpha = '0';
            if(j >= 10 && j <= 35) alpha = (char) 'A' - (char) 10;
            if(j >= 36 && j <= 61) alpha = (char) 'a' - (char) 36;
            alpha += j;
            toreturn = toreturn + alpha;
        }
        return toreturn;
    }
}