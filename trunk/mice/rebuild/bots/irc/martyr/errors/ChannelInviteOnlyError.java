/*
 * ChannelInviteOnlyError.java
 *
 *   Copyright (C) 2000, 2001, 2002, 2003 Ben Damm
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   See: http://www.fsf.org/copyleft/lesser.txt
 */
package martyr.errors;

import martyr.InCommand;
/**
 * @version $Id: ChannelInviteOnlyError.java,v 1.2 2004/04/01 22:02:33 bdamm Exp $
 */
public class ChannelInviteOnlyError extends GenericJoinError
{
	public ChannelInviteOnlyError()
	{
		// This one's for registering.
	}

	protected ChannelInviteOnlyError( String chan, String comment )
	{
		super(chan, comment);
	}

	public String getIrcIdentifier()
	{
		return "473";
	}

	protected InCommand create(String channel, String comment)
	{
		return new ChannelInviteOnlyError( channel, comment );
	}
}
