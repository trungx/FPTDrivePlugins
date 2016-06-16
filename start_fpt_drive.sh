#node-red-start &
~/Workspace/FPTDrive/fptdriveplugins/start_plugin_eyes_detect.sh &
~/Workspace/FPTDrive/fptdriveplugins/start_plugin_car_distance.sh &

#GPS server
node ~/Workspace/FPTDrive/FPTDrive-Gateway/gps_app/gpsserver.js &