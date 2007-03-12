-module(test).
-export([main/0]).

main() ->
    catch(cbot_logger:start()),
    {ok, Sup} = cbot_irc_supervisor:start_link({tcp, "irc.freenode.net", 6667}, [ {nick, "erlbot"} ]).


    {ok, B} = cbot_irc_conn:start(
    register(bot, B),

    io:format("Returned from start_link~n"),
    cbot_irc_conn:connect_behaviour(B, undefined),
    io:format("Connected~n"),
    cbot_irc_conn:send_data(B, "JOIN #tacobeam\r\n"),
    io:format("Sent join command~n").


