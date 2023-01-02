ps -aux | grep getbluetooth
killall getbluetooth inotifywait
./getbluetooth &
./HDRTZ.out & 