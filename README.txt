Instructions -

    1. sysbench steps
        1. sudo rmmod OS
        2. make clean
        3. update changes to OS.c & user.c if required
        4. make all
        5. sysbench --test=cpu --cpu-max-prime=900000 run &
        6. ps
        7. sudo insmod OS.ko process_ID=
        8. dmesg
        9. ./user

    2. kernel compilation
        1. sudo rmmod OS
        2. make clean
        3. update changes to OS.c & user.c if required
        4. make all
        5. make & (in other installed linux terminal)
        6. sudo insmod OS.ko process_ID=
        7. dmesg
        8. ./user

    3. iperf
        1. sudo rmmod OS
        2. make clean
        3. update changes to OS.c & user.c if required
        4. make all
        5. iperf -s (in third terminal)
        6. sleep 40 && iperf -c ipaddress & (in other linux terminal with the same basic profile)
        7. sudo insmod OS.ko process_ID=
        8. dmesg
        9. ./user