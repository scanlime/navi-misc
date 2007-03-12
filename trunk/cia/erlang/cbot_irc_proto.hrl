%%
%% cbot_irc_proto Header
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-record(message, {
	  prefix,    % String
	  command,   % String
	  params     % List of strings
	  }).
