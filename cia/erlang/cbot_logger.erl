%%
%% cbot_logger
%%
%% This is a registered gen_server process which handles log messages
%% on behalf of all IRC bots.
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-module(cbot_logger).
-behaviour(gen_server).

-export([start_link/0, start/0, log/2]).
-export([init/1, terminate/2, code_change/3, handle_call/3, handle_cast/2, handle_info/2]).


%%---------------------------------------------------------------------------
%% Public API
%%---------------------------------------------------------------------------

start_link() ->
    gen_server:start_link(?MODULE, undefined, []).

start() ->
    gen_server:start(?MODULE, undefined, []).


%%
%% Send a log message from the current process.  Should never fail: if
%% the log server is not running, the message is ignored.
%%

log(Type, Message) ->
    gen_server:cast(cbot_logger, {log, self(), Type, Message}).


%%---------------------------------------------------------------------------
%% Callback interface for gen_server
%%---------------------------------------------------------------------------

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    State.

init(_) ->
    register(cbot_logger, self()),
    log(logger, started),
    {ok, default}.

handle_cast({log, From, Type, Message}, State) ->
    io:format("[~p ~p] ~p~n", [From, Type, Message]),
    {noreply, State};
handle_cast(_, State) ->
    {noreply, State}.

handle_call(_, _From, State) ->
    {noreply, State}.

handle_info(_, State) ->
    {noreply, State}.
