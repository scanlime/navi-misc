%%
%% cbot_irc_behaviour
%%
%% This is a gen_server process that connects to a single cbot_irc_connection
%% process, and implements all higher-level per-bot behaviours. This includes:
%%
%%   - Joining and parting channels
%%   - Responding to CTCP commands
%%   - Responding to user messages
%%   - Nickname policy
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-module(cbot_irc_behaviour).
-include("cbot_irc_protocol.hrl").
-behaviour(gen_server).

%% Public API
-export([start_link/2]).

%% gen_server callbacks
-export([init/1, terminate/2, code_change/3, handle_call/3, handle_cast/2, handle_info/2]).

-record(bh_state, {
	  conn     % Connection Pid
	 }).


%%---------------------------------------------------------------------------
%% Public API
%%---------------------------------------------------------------------------

start_link(Parent, Options) ->
    gen_server:start_link(?MODULE, {Parent, Options}, []).


%%---------------------------------------------------------------------------
%% Sent messages
%%---------------------------------------------------------------------------

%%
%% Send an arbitrary message
%%
%% FIXME: Bandwidth accounting
%%

send_message(#message{}=Message, #bh_state{conn=Conn}) ->
    {ok, Encoded} = cbot_irc_protocol:encode_message(Message),
    cbot_irc_connection:send_data(Conn, Encoded).


%%---------------------------------------------------------------------------
%% Received message handlers
%%---------------------------------------------------------------------------

%% The connection process already handles pings. Do nothing.
received_message(#message{command="PING"}, State) ->
    State;

%% Log unhandled messages
received_message(Message, State) ->
    cbot_logger:log(unhandled_message, Message),
    State.


%%---------------------------------------------------------------------------
%% Callback interface for gen_server
%%---------------------------------------------------------------------------

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    State.

%%
%% We have no synchronous initialization to perform.
%% Make sure the first message we process is the 'connect' cast.
%%

init(Args) ->
    cbot_logger:log(behaviour_state, init),
    ok = gen_server:cast(self(), {connect, Args}),
    {ok, not_connected}.

%%
%% 'connect' cast: Connect to the cbot_irc_connection process.
%%
%% We don't want to do this during init/1 for multiple reasons:
%%
%%   1. The call to cbot_irc_supervisor:get_child/2 would
%%      deadlock if we issued it within our synchronous init/1.
%%
%%   2. The call to cbot_irc_connection:connect_behaviour/2
%%      will block until the connection has logged in successfully,
%%      which could take a while.
%%

handle_cast({connect, {Parent, _Options}}, not_connected) ->
    %% This blocks until the supervisor is ready
    Connection = cbot_irc_supervisor:get_child(Parent, connection),

    %% Now this blocks until the connection is ready:
    %% which means it blocks until it's logged in.
    %% connect_behaviour/2 implements the login timeout for us.

    cbot_logger:log(behaviour_state, connecting),
    cbot_irc_connection:connect_behaviour(Connection, self()),
    cbot_logger:log(behaviour_state, connected),

    {noreply, #bh_state{conn=Connection}};

%%
%% 'irc_message' cast: Receive a raw IRC message
%%                     from the connection process.
%%

handle_cast({irc_message, Data}, #bh_state{}=State) ->
    case catch(cbot_irc_protocol:decode_message(Data)) of
	{ok, Message} ->
	    {noreply, received_message(Message, State)};
	Error ->
	    cbot_logger:log(decode_error, {Error, Data}),
	    {noreply, State}
    end;

%%
%% Log unhandled casts
%%

handle_cast(Cast, State) ->
    cbot_logger:log(unhandled_cast, Cast),
    {noreply, State}.

%%
%% Log unhandled calls
%%

handle_call(Call, From, State) ->
    cbot_logger:log(unhandled_call, {Call, From}),
    {reply, {error, unhandled_call}, State}.

%%
%% Log unhandled messages
%%

handle_info(Info, State) ->
    cbot_logger:log(unhandled_info, Info),
    {noreply, State}.
