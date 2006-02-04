cd cia
rm *.log
rm bots.socket
twistd2.4 -oy conf/bot_server.tac -l /var/log/cia/bot_server.log --pidfile=bot_server.pid
cd
./restart_now.sh
