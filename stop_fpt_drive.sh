killall EyesBlinkDetect &
killall Distimer_CarDistance &

#kill gps server
kill -9 $(ps aux | grep gpsserver.js | awk '{print $2}')