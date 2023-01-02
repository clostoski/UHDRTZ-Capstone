ps -aux | grep xwax
killall xwax inotifywait
ps -aux | grep getbluetooth
killall getbluetooth inotifywait
./getbluetooth &
./HDRTZ.out & 
#./audio/xwax-1.8/xwax -t serato_2a -a default -q 0 -l audio/RomanceAnonimo.m4a &