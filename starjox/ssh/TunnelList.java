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

public class TunnelList extends JPanel implements ListSelectionListener
{
    private JFrame myFrame;
    private JList tList;
    private DefaultListModel tListModel;
    private JButton ssTunnel;
    private JButton newTunnel;

    public TunnelList(JFrame frame)
    {
	super(new BorderLayout());
	myFrame = frame;
	tListModel = new DefaultListModel();
	
	tListModel.addElement("Place Holder");
	

	tList = new JList(tListModel);
	tList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	tList.setSelectedIndex(0);
	tList.addListSelectionListener(this);
	tList.setVisibleRowCount(10);
	JScrollPane listScrollPane = new JScrollPane(tList);
	add(listScrollPane, BorderLayout.CENTER);

	
	newTunnel = new JButton("New Tunnel");
	newTunnel.addActionListener(new newTunnelListener());

	ssTunnel = new JButton("Start/Stop");
	ssTunnel.addActionListener(new ssTunnelListener());
	JPanel buttonPane = new JPanel();
	buttonPane.setLayout(new BoxLayout(buttonPane,BoxLayout.LINE_AXIS));
	buttonPane.add(newTunnel);
	buttonPane.add(ssTunnel);
	buttonPane.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
        add(buttonPane, BorderLayout.PAGE_END);
    }

    public void valueChanged(ListSelectionEvent e)
    {
	
    }

    class newTunnelListener implements ActionListener 
    {
	public void actionPerformed(ActionEvent e)
	{
	    System.out.println("New Tunnel Clicked");
	}
    }

    class ssTunnelListener implements ActionListener
    {
	public void actionPerformed(ActionEvent e)
	{
	    System.out.println("Start/Stop Clicked");
	}
    }
}
