%%
%% cbot_irc_proto
%%
%% Library implementing the IRC protocol, mostly-according to RFC 1459.
%% This includes message quoting, encoding, and decoding.
%%
%% Copyright (C) 2007 Micah Dowty <micah@navi.cx>
%%
%%---------------------------------------------------------------------------

-module(cbot_irc_proto).
-include("cbot_irc_proto.hrl").

-export([encode_message/1, decode_message/1]).


%%---------------------------------------------------------------------------
%% IRC Protocol: Low-level quoting
%%---------------------------------------------------------------------------

%%
%% Low-level message quoting, as defined by the CTCP spec:
%%  http://www.irchelp.org/irchelp/rfc/ctcpspec.html
%%

-define(M_QUOTE, 8#020).
-define(SPACE,   32).

%%
%% Quote a low-level IRC/CTCP message. Operates on deep lists.
%%
%% > cbot_irc_conn:low_quote("foo\020\nbar").
%%   [102,111,111,16,16,16,110,98,97,114]
%%

low_quote(String) when is_list(String) ->
    lists:reverse(low_quote(String, [])).

low_quote([H|T], Tail) when is_list(H) ->
    low_quote(T, low_quote(H, Tail));

low_quote([?M_QUOTE|T], Tail) ->
    low_quote(T, [?M_QUOTE, ?M_QUOTE|Tail]);
low_quote([$\0|T], Tail) ->
    low_quote(T, [$0, ?M_QUOTE|Tail]);
low_quote([$\n|T], Tail) ->
    low_quote(T, [$n, ?M_QUOTE|Tail]);
low_quote([$\r|T], Tail) ->
    low_quote(T, [$r, ?M_QUOTE|Tail]);

low_quote([H|T], Tail) ->
    low_quote(T, [H|Tail]);
low_quote([], Tail) ->
    Tail.

%%
%% Dequote a low-level IRC/CTCP message. This is the inverse of
%% low_quote/1.  Does not operate on deep lists. Strips out
%% unrecognized escape sequences or partial escape sequences.
%%

low_dequote(String) when is_list(String) ->
    lists:reverse(low_dequote(String, [], default)).

low_dequote([?M_QUOTE|T], Tail, default) ->
    low_dequote(T, Tail, escaped);

low_dequote([?M_QUOTE|T], Tail, escaped) ->    
    low_dequote(T, [?M_QUOTE|Tail], default);
low_dequote([$0|T], Tail, escaped) ->    
    low_dequote(T, [$\0|Tail], default);
low_dequote([$n|T], Tail, escaped) ->    
    low_dequote(T, [$\n|Tail], default);
low_dequote([$r|T], Tail, escaped) ->    
    low_dequote(T, [$\r|Tail], default);

low_dequote([H|T], Tail, escaped) when is_integer(H) ->
    %% Ignore unrecognized escape sequence
    low_dequote(T, Tail, default);
low_dequote([H|T], Tail, default) when is_integer(H) ->
    low_dequote(T, [H|Tail], default);
low_dequote([], Tail, _State) ->
    Tail.


%%---------------------------------------------------------------------------
%% IRC Protocol: Message codec
%%---------------------------------------------------------------------------

%%
%% State machine for quickly parsing a (dequoted) IRC message, as a
%% string which may include \r and \n termination.
%%
%% Returns either {ok, #message} or an error.
%% Does not accept deep lists.
%%

decode_message(String) when is_list(String) ->
    catch(decode_message(default, low_dequote(String),
			 #message{prefix = [], command = [], params = []})).

%% Finish a parameter (trailing or not) when we encounter end-of-line
decode_message({params, Cp, _IsTrailing}, [$\n|T], M) ->
    decode_message(trailing_whitespace, T,
		   M#message { params = [lists:reverse(Cp)|M#message.params] });
decode_message({params, Cp, _IsTrailing}, [$\r|T], M) ->
    decode_message(trailing_whitespace, T,
		   M#message { params = [lists:reverse(Cp)|M#message.params] });

%% Slurp all extra EOL characters into the 'trailing_whitespace' state
decode_message(_State, [$\n|T], M) ->
    decode_message(trailing_whitespace, T, M);
decode_message(_State, [$\r|T], M) ->
    decode_message(trailing_whitespace, T, M);

%% Detect the existence of a prefix
decode_message(default, [$:|T], M) ->
    decode_message(prefix, T, M);
decode_message(default, T, M) ->
    decode_message(command, T, M);

%% Decode the prefix until we hit a SPACE
decode_message(prefix, [?SPACE|T], M) ->
    decode_message(command, T, M);
decode_message(prefix, [H|T], M) ->
    decode_message(prefix, T, M#message{ prefix = [H|M#message.prefix] });

%% Decode the command until we hit a SPACE. Ignore extra leading spaces.
decode_message(command, [?SPACE|T], #message{ command = [] }=M) ->
    decode_message(command, T, M);
decode_message(command, [?SPACE|T], M) ->
    decode_message({params, [], false}, T, M);
decode_message(command, [H|T], M) ->
    decode_message(command, T, M#message{ command = [H|M#message.command] });

%% Decode parameters. Our state here is a {params, Current, IsTrailing}
%% tuple, where we keep track of the current parameter and its type.
%% Trailing parameters begin with a ':', and they may contain spaces.

%% We ignore extra leading whitespace on non-trailing params.
decode_message({params, [], false}, [?SPACE|T], M) ->
    decode_message({params, [], false}, T, M);

%% Beginning a trailing parameter?
decode_message({params, [], false}, [$:|T], M) ->
    decode_message({params, [], true}, T, M);

%% Finish non-trailing parameters when we encounter a SPACE
decode_message({params, Cp, false}, [?SPACE|T], M) ->
    decode_message({params, [], false}, T,
		   M#message {params = [lists:reverse(Cp)|M#message.params] });

%% Next character in a parameter...
decode_message({params, Cp, IsTrailing}, [H|T], M) ->
    decode_message({params, [H|Cp], IsTrailing}, T, M);

%% The one valid end-state: we just finished processing trailing
%% spaces, and the string is now empty.
decode_message(trailing_whitespace, [], M) ->
    {ok, #message{ prefix  = lists:reverse(M#message.prefix),
		   command = lists:reverse(M#message.command),
		   params  = lists:reverse(M#message.params) }};

%% Catch-all for bad states.
decode_message(State, String, _M) ->
    {bad_message, State, String}.

%%
%% Encode a message: the inverse of decode_message. Accepts a
%% #message record, returns a CRLF-terminated deep list.
%%
%% Returns either {ok, DeepList} or an error.
%%

encode_message(Message) ->
    encode_message(Message, true).

encode_message(#message{}=Message, AllowTrailingParam) ->
    catch({ok, [ low_quote([ case Message#message.prefix of
				 [] -> [];
				 undefined -> [];
				 Prefix ->
				     [$:, no_spaces(Prefix), ?SPACE]
			     end,
			     no_spaces(Message#message.command),
			     encode_params(Message#message.params, AllowTrailingParam)
			    ]),
		 "\r\n" ]}).

%% Encode a parameter list, optionally supporting "trailing" parameters
%% which begin with a ':' and may contain whitespace. Each parameter
%% (including the first!) will begin with ?SPACE.
encode_params(undefined, _Trailing) ->
    [];
encode_params(Params, Trailing) ->
    lists:reverse(encode_params(Params, Trailing, [])).
encode_params([H|[]], true, Tail) ->
    [H, $:, ?SPACE|Tail];
encode_params([H|T], Trailing, Tail) ->
    encode_params(T, Trailing, [no_spaces(H), ?SPACE|Tail]);
encode_params([], _Trailing, Tail) ->
    Tail.

%% Ensure that a string has no spaces. Returns the
%% string on success, throws on error.
no_spaces(String) when is_list(String) ->
    no_spaces(String, String).
no_spaces([?SPACE|_], String) ->
    throw({spaces_not_allowed, String});
no_spaces([_|T], String) ->
    no_spaces(T, String);
no_spaces([], String) ->
    String.
