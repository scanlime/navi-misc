/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
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

/**
 * This class is the main class for the ssh tunnel.
 * @author Brandon Smith
 * @version 0.1
 */

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class TunnelMain
{
    public static void swingInit()
    {
	JFrame.setDefaultLookAndFeelDecorated(true);
	JFrame frame = new JFrame("SSH Tunnel");
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	
	TunnelList tunnelList = new TunnelList(frame);
	tunnelList.setOpaque(true);
	frame.setContentPane(tunnelList);

	frame.pack();
	frame.setVisible(true);	
    }
    
    public static void main(String[] args)
    {
	javax.swing.SwingUtilities.invokeLater(new Runnable() 
	    {
		public void run()
		{
		    swingInit();
		}
	    });
    }
}
