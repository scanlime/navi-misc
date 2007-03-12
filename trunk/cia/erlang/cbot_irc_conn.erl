%%
%% cbot_irc_conn
%%
%% This is a gen_server process which maintains a single IRC client
%% connection. This process is responsible for:
%%
%%   - Establishing a connection
%%   - Logging in
%%   - Responding to PINGs
%%
%% It does NOT include any more state than necessary. This process
%% should be very robust.
%%
%% Once the connection is established and the bot is logged in, this
%% process directs incoming messages to a separate per-bot
%% cbot_irc_behaviour process, which connects by calling
%% connect_behaviour/2.
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-module(cbot_irc_conn).
-include("cbot_irc_proto.hrl").
-behaviour(gen_server).

%% Public API
-export([start_link/2, start/2, send_data/2, connect_behaviour/2, get_login_messages/1]).

%% gen_server callbacks
-export([init/1, terminate/2, code_change/3, handle_call/3, handle_cast/2, handle_info/2]).

-record(conn_state, {
	  sock,            % cbot_socket tuple
	  behaviour,       % Pid of connected behaviour process.
	  login_messages   % List of messages that came during login
	 }).


%%---------------------------------------------------------------------------
%% Public API
%%---------------------------------------------------------------------------

%%
%% Start a new bot. ConnectInfo is a connection tuple, passed directly
%% to cbot_socket. Options is a property list with bot login options.
%%
%% The process will be creates synchronously, but connection and login
%% both happen asynchronously. Note that the server will not respond
%% to any other messages, including connect_behaviour/2, until login
%% is complete.
%%
%% Options: nick, username, hostname, servername, realname,
%%          temp_nick_prefix
%%   

start_link(ConnectInfo, Options) ->
    gen_server:start_link(?MODULE, {ConnectInfo, Options}, []).

start(ConnectInfo, Options) ->
    gen_server:start(?MODULE, {ConnectInfo, Options}, []).

%%
%% Send raw data to this bot's socket. The caller is expected to
%% encode the data. This improves reliability of this process, and
%% it's necessary if the caller wishes to do traffic shaping.
%%

send_data(Pid, Data) ->
    gen_server:call(Pid, {send_data, Data}).

%%
%% Connect a new behaviour process. Behaviour processes are notified
%% of incoming messages via an 'irc_message' cast with the raw message
%% data.
%%
%% NB: This process does have to decode messages, however we choose
%%     to only provide the raw messages to the behaviour process.
%%     This requires parsing every message twice, but it insulates
%%     the interface between this process and the behaviour process
%%     from changes in the message parsing code.
%%
%% We can only have one behaviour process at a time. This call
%% replaces the existing behaviour process, if there was any.
%%
%% Note that all raw lines are delivered to the behaviour process,
%% even lines from messages that we also handle internally (pings).
%%

connect_behaviour(Pid, BehaviourPid) ->
    gen_server:call(Pid, {connect_behaviour, BehaviourPid}).

%%
%% Retrieve a list of the messages processed and stored during login.
%% This will include the MOTD and the server capabilities.
%%

get_login_messages(Pid) ->
    gen_server:call(Pid, get_login_messages).


%%---------------------------------------------------------------------------
%% Local message processing
%%---------------------------------------------------------------------------

send_data_local(Line, #conn_state{sock=Sock}) ->
    cbot_logger:log(send_data, Line),
    cbot_socket:send(Sock, Line).

received_line(Line, #conn_state{behaviour=Behaviour}=State) ->
    cbot_logger:log(receive_data, Line),
    gen_server:cast(Behaviour, {'irc_message', Line}),

    case catch(deliver_local_message(Line, State)) of
	ok -> ok;
	Error -> cbot_logger:log(local_delivery_error, Error)
    end.

send_message(#message{}=Message, State) ->
    {ok, Encoded} = cbot_irc_proto:encode_message(Message),
    send_data_local(Encoded, State).

deliver_local_message(Line, State) ->
    case cbot_irc_proto:decode_message(Line) of
	{ok, #message{command="PING"}=M} ->
	    handle_local_message(M, State);
	_ ->
	    ok
    end.

%%
%% Always respond to pings locally: we don't want the connection to
%% die if the behaviour process is unavailable for a while.
%%
handle_local_message(#message{ command="PING", params=Params }, State) ->
    send_message(#message{ command="PONG", params=Params }, State).


%%---------------------------------------------------------------------------
%% IRC Login
%%---------------------------------------------------------------------------

%%
%% Blocking wait for an IRC message. This is for use only during
%% login, when we want the server to finish before processing
%% any other messages.
%%
%% This function includes a timeout between individual messages,
%% however we still rely on an external supervisor to enforce an
%% overall connect/login timeout.
%%

wait_for_message(#conn_state{sock=Sock}) ->
    case cbot_socket:recv_blocking() of
	{data, Sock, Line} ->
	    {ok, #message{}=Message} = cbot_irc_proto:decode_message(Line),
	    Message;
	Other -> exit(Other)
    end.

%%
%% Synchronously log in to the IRC server.
%%

log_in(Options, State) ->
    case proplists:get_value(password, Options) of
	undefined -> ok;
	Password ->
	    send_message(#message{ command="PASS", params=[Password] }, State)
    end,

    %% This is just an initial guess at a nickname.  If there's a
    %% collision, we'll respond by picking a random nick.  It's the
    %% behaviour process's job to make sure we got a nick we like, and
    %% to change it if necessary.

    send_message(#message{command="NICK",
			  params=[ proplists:get_value(nick, Options, "CIA") ]},
		 State),

    {ok, Hostname} = inet:gethostname(),
    Username = case os:getenv("USER") of
		   false -> "cia";
		   User -> User
	       end,

    send_message(#message{command="USER",
			  params=[ proplists:get_value(username, Options, Username),
				   proplists:get_value(hostname, Options, Hostname),
				   proplists:get_value(servername, Options, "server"),
				   proplists:get_value(realname, Options, "CIA Bot")]},
		 State),

    cbot_logger:log(state, wait_for_login),
    State2 = wait_for_login(Options, State#conn_state{ login_messages=[] }),
    cbot_logger:log(state, logged_in),

    %% Put the login_messages list back in order
    State2#conn_state{ login_messages=lists:reverse(State2#conn_state.login_messages) }.


%%
%% Tail-recursive loop to synchronously wait for
%% login to complete. Parses IRC messages as they
%% arrive, and adds them to login_messages (in
%% reverse order)
%%

wait_for_login(Options, State) ->
    ok, Message = wait_for_message(State),
    cbot_logger:log(login_message, Message),

    %% Store every message that came during login.  Other processes
    %% can query for these messages later, for example to retrieve the
    %% MOTD or the list of server capabilities.

    State2 = State#conn_state{ login_messages=[Message|State#conn_state.login_messages] },

    case Message of

	%% Always handle pings, even during login
	#message{command="PING"} ->
	    handle_local_message(Message, State2),
	    wait_for_login(Options, State2);

	%% "Nickname is already in use." Pick a random nickname
	%% so we can finish logging in. Finding an available nick
	%% might require performing WHOIS queries, which we can't
	%% do before logging in.
	#message{command="433"} ->
	    send_message(#message{command="NICK",
				  params=[ proplists:get_value(temp_nick_prefix, Options, "CIA-temp")
					   ++ integer_to_list(100 + random:uniform(999 - 100)) ]},
			 State2),
	    wait_for_login(Options, State2);
	
	%% "MOTD file is missing." Done with login.
	#message{command="422"} ->
	    State2;

	%% "End of MOTD." Done with login.
	#message{command="376"} ->
	    State2;

	_ ->
	    wait_for_login(Options, State2)
    end.


%%---------------------------------------------------------------------------
%% Callback interface for gen_server
%%---------------------------------------------------------------------------

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    State.

%%
%% This is the synchronous initialization function invoked by
%% gen_server on our server's process, just after it's been spawned
%% but before it begins its event loop.
%%
%% gen_server:start_link will not return until this function finishes,
%% so we must not wait for the bot to log on before
%% returning. However, we do want to wait for logon before processing
%% any outside messages.  To solve this problem, we'll send ourselves
%% an asynchronous message in order to start the login process.
%%
%% The initial state indicates that we're unconnected. This state
%% will change as soon as we process the asynchronous 'connect' message.
%%

init({ ConnectInfo, Options }) ->
    cbot_logger:log(state, started),
    ok = gen_server:cast(self(), {connect, ConnectInfo, Options}),
    {ok, not_connected}.

%%
%% 'connect' broadcast message.
%%
%% This message is only valid in the 'not_connected' state.
%% It is sent by init(), so it should be the first message we
%% process. This will synchronously connect and log in.
%%

handle_cast({connect, ConnectInfo, Options}, not_connected) ->
    cbot_logger:log(state, connecting),

    Sock = cbot_socket:connect(ConnectInfo),
    cbot_logger:log(state, connected),

    {noreply, log_in(Options, #conn_state{sock=Sock})};

%%
%% Log unhandled broadcast messages
%%

handle_cast(Cast, State) ->
    cbot_logger:log(unhandled_cast, Cast),
    {noreply, State}.

%%
%% 'send_data' call: Send raw data to our socket.
%%

handle_call({send_data, Data}, _From, #conn_state{}=State) ->
    {reply, catch(send_data_local(Data, State)), State};

%%
%% 'connect_behaviour' call: Change the current behaviour process.
%%

handle_call({connect_behaviour, Pid}, _From, #conn_state{}=State) ->
    cbot_logger:log(connect_behaviour, Pid),
    {reply, ok, State#conn_state{behaviour=Pid}};

%%
%% 'get_login_messages' call
%%

handle_call(get_login_messages, _From, #conn_state{login_messages=Messages}=State) ->
    {reply, Messages, State};

%%
%% Log unhandled calls
%%

handle_call(Call, From, State) ->
    cbot_logger:log(unhandled_call, {Call, From}),
    {noreply, {error, unhandled_call}, State}.

%%
%% Handle generic Erlang messages: in this case, they should
%% always be socket messages. Ask cbot_socket to decode them,
%% then call received_line() to process the incoming data.
%%

handle_info(M, State) ->
    case cbot_socket:recv_message(M) of
	{closed, _Sock} ->
	    {stop, socket_closed, State};
	{data, _Sock, Data} ->
	    received_line(Data, State),
	    {noreply, State};
	_ ->
	    cbot_logger:log(unhandled_info, M),
	    {noreply, State}
    end.
