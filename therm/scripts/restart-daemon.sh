cd /home/micah/therm
datadir=/home/httpd/html/therm
(ps ax | grep therm-daemon | grep -v -q grep) || nohup ./therm-daemon $datadir > $datadir/daemon.stdout 2> $datadir/daemon.stderr


