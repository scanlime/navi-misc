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
 * This class keeps track of a single connection
 */

public class TunnelSingleConnection
{
    /** The username for this session. */
    private String username;
    
    /** The password for this session. */
    private String password;

    /** The local port that is bound to (or connected to) .*/
    private int localPort;
    
    /** The hostname of the ssh server. */
    private String serverHost;

    /** The remote ssh server port (default 22) */
    private int serverPort;

    /** The target host for the end of the tunnel.  */
    private String targetHost;

    /** The target port for the end of the tunnel. */
    private int targetPort;

    /** True if local forwarding (local to remote)
     *	False if remote forwarding (remote to local)
     */
    private boolean localFw;

    /** True if it should bind to other than localhost. */
    private boolean offHost;
}
