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
	private JButton editTunnel;
	private JButton rmTunnel;
	public TunnelList(JFrame frame)
	{
		super(new BorderLayout());
		myFrame = frame;
		tListModel = new DefaultListModel();
	
		//tListModel.addElement("Place Holder");
	

		tList = new JList(tListModel);
		tList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		tList.setSelectedIndex(0);
		tList.addListSelectionListener(this);
		tList.setVisibleRowCount(10);
		JScrollPane listScrollPane = new JScrollPane(tList);
		add(listScrollPane, BorderLayout.CENTER);

		newTunnel = new JButton("New");
		newTunnel.addActionListener(new newTunnelListener());

		rmTunnel = new JButton("Del");

		editTunnel = new JButton("Edit");
		editTunnel.addActionListener(new editTunnelListener());

		ssTunnel = new JButton("Start/Stop");
		ssTunnel.addActionListener(new ssTunnelListener());
		
		JPanel buttonPane = new JPanel();
		buttonPane.setLayout(new BoxLayout(buttonPane,BoxLayout.LINE_AXIS));
		buttonPane.add(newTunnel);
		buttonPane.add(rmTunnel);
		buttonPane.add(editTunnel);
		editTunnel.setEnabled(false);
		buttonPane.add(ssTunnel);
		ssTunnel.setEnabled(false);
		buttonPane.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
		add(buttonPane, BorderLayout.PAGE_END);
	}

	public void valueChanged(ListSelectionEvent e)
	{
		if(e.getValueIsAdjusting() == false);
		{
			if(tList.getSelectedIndex() == -1)
			{
				ssTunnel.setText("Start/Stop");
				ssTunnel.setEnabled(false);
				editTunnel.setEnabled(false);
			}
			else
			{
				ssTunnel.setText("Start");
				ssTunnel.setEnabled(true);
				editTunnel.setEnabled(true);
			}
		}
	}

	class editTunnelListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{
			//Create the dialog.
		
			final JDialog dialog = new JDialog(myFrame,
																				"A Non-Modal Dialog");

                    //Add contents to it. It must have a close button,
                    //since some L&Fs (notably Java/Metal) don't provide one
                    //in the window decorations for dialogs.
                    JLabel label = new JLabel("<html><p align=center>"
                        + "This is a non-modal dialog.<br>"
                        + "You can have one or more of these up<br>"
                        + "and still use the main window.");
                    label.setHorizontalAlignment(JLabel.CENTER);
                    Font font = label.getFont();
                    label.setFont(label.getFont().deriveFont(font.PLAIN,
                                                             14.0f));

                    JButton closeButton = new JButton("Close");
                    closeButton.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                            dialog.setVisible(false);
                            dialog.dispose();
                        }
                    });
                    JPanel closePanel = new JPanel();
                    closePanel.setLayout(new BoxLayout(closePanel,
                                                       BoxLayout.LINE_AXIS));
                    closePanel.add(Box.createHorizontalGlue());
                    closePanel.add(closeButton);
                    closePanel.setBorder(BorderFactory.
                        createEmptyBorder(0,0,5,5));

                    JPanel contentPane = new JPanel(new BorderLayout());
                    contentPane.add(label, BorderLayout.CENTER);
                    contentPane.add(closePanel, BorderLayout.PAGE_END);
                    contentPane.setOpaque(true);
                    dialog.setContentPane(contentPane);

                    //Show it.
                    dialog.setSize(new Dimension(300, 150));
                    dialog.setLocationRelativeTo(myFrame);
                    dialog.setVisible(true);

			System.out.println("Edit Tunnel clicked");
		}
	}

	class newTunnelListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{
			tListModel.addElement("Place Holder");
			System.out.println("New Tunnel Clicked");
		} 
	}

	class ssTunnelListener implements ActionListener
	{
		public void actionPerformed(ActionEvent e)
		{
			tListModel.remove(0);
			if(tListModel.getSize() == 0)
				ssTunnel.setEnabled(false);
			
			System.out.println("Start/Stop Clicked");
		}
	}
}
