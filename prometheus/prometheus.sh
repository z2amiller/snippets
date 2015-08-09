#! /bin/sh
# /etc/init.d/noip2.sh

# Supplied by no-ip.com
# Modified for Debian GNU/Linux by Eivind L. Rygge <eivind@rygge.org>
# corrected 1-17-2004 by Alex Docauer <alex@docauer.net>

# . /etc/rc.d/init.d/functions  # uncomment/modify for your killproc

BASEDIR=/opt/prometheus
DAEMONS="prometheus node_exporter pushgateway"
NAME=prometheus

extra_args () {
  EXTRA_ARGS=""
  case "$1" in
    prometheus)
      EXTRA_ARGS="-web.listen-address :8000 -config.file $BASEDIR/prometheus/prometheus.yml"
    ;;
  esac
}

pid_start () {
    echo -n "Starting $NAME: "
    for daemon in $DAEMONS
    do
      extra_args $daemon
      start-stop-daemon --start --chuid prometheus \
                        --background \
                        --chdir $BASEDIR/$daemon \
                        --exec $BASEDIR/$daemon/$daemon -- $EXTRA_ARGS
      echo -n "$daemon  "
    done
    echo "Done!"
}

pid_stop () {
    echo -n "Shutting down $NAME:"
    for daemon in $DAEMONS
    do
      start-stop-daemon --stop --oknodo --retry 30 --exec $BASEDIR/$daemon/$daemon
      echo "$daemon  "
    done
    echo "Done!"
}

case "$1" in
    start)
	pid_start
    ;;

    stop)
	pid_stop
    ;;

    restart)
	pid_stop
	pid_start
    ;;

    *)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
esac
exit 0

