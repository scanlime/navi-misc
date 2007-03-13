-module(test).
-export([main/0]).

main() ->
    catch(cbot_logger:start()),
    {ok, Sup} = cbot_irc_supervisor:start_link({tcp, "localhost", 6667}, [ {nick, "erlbot"} ]),
    register(cbot_sup, Sup).
