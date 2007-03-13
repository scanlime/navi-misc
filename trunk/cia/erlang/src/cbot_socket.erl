%%
%% cbot_socket
%%
%% This is a library for generic socket operations. Using a
%% type-specific "connection spec", you can open a socket that can
%% then be used in identical ways regardless of how that socket is
%% implemented.
%%
%% This module gives us transparent support for protocols like SSL and IPv6.
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-module(cbot_socket).

-export([connect/1, connect/3, send/2, recv_message/1, recv_blocking/0]).

-define(CONNECT_TIMEOUT, 60000).
-define(RECV_TIMEOUT,    60000).

start_inet_db()->
    case inet_db:start() of
        {ok, _} ->
            ok;
        {error, {already_started, _}} ->
            ok;
        Error ->
            throw(Error)
    end.

start_ssl() ->
    case ssl:start() of
        ok ->
            ok;
        {ok, _} ->
            ok;
        {error, {already_started,_}} ->
            ok;
        Error ->
            throw(Error)
    end.

%%
%% Connect with default options and timeout
%%
connect(ConnectSpec) ->
    connect(ConnectSpec,
	    [list,
	     {packet, line},
	     {nodelay, true}],
	    ?CONNECT_TIMEOUT).

connect({ tcp, Host, Port }, Options, Timeout) ->
    start_inet_db(),
    case gen_tcp:connect(Host, Port,
			 Options ++ [inet,
				     {packet_size, 1024},
				     {recbuf, 16384},
				     {sndbuf, 16384}],
			 Timeout) of
	{ok, Sock} ->
	    {ok, {tcp, Sock}};
	Error ->
	    Error
    end;

connect({ ssl, Host, Port }, Options, Timeout) ->
    start_inet_db(),
    start_ssl(),
    case ssl:connect(Host, Port, Options, Timeout) of
	{ok, Sock} ->
	    {ok, {ssl, Sock}};
	Error ->
	    Error
    end.


send({ tcp, Sock }, Data) ->
    gen_tcp:send(Sock, Data);

send({ ssl, Sock }, Data) ->
    ssl:send(Sock, Data).

%%
%% Handle an incoming message.
%%
%%  - If the message does not represent received socket
%%    data, returns 'undefined'.
%%
%%  - If data was received, returns a {recv, Sock, Data} tuple.
%%
%%  - If a connection was closed, returns {closed, Sock}.
%%

recv_message({tcp_closed, Sock}) ->
    {closed, {tcp, Sock}};

recv_message({tcp, Sock, Data}) ->
    {data, {tcp, Sock}, Data};

recv_message({ssl_closed, Sock}) ->
    {closed, {ssl, Sock}};

recv_message({ssl, Sock, Data}) ->
    {data, {ssl, Sock}, Data};

recv_message(_) ->
    undefined.

%%
%% Blocking receive for an incoming message.
%% Return values are the same as recv_messge(), with
%% the addition of a 'timeout' atom.
%%
%% NB: We list each message type separately here in order
%%     to avoid dequeueing messages we don't want to process.
%%

recv_blocking() ->
    receive
	{'EXIT', _Parent, Reason} -> exit(Reason);
	{tcp_closed, _}=M -> recv_message(M);
	{tcp, _, _}=M -> recv_message(M);
	{ssl_closed, _}=M -> recv_message(M);
	{ssl, _, _}=M -> recv_message(M)
    after ?RECV_TIMEOUT -> timeout
    end.
	    

