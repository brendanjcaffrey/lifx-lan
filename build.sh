mkdir -p bin
gcc -std=gnu99 examples/lifx-lan.c -o bin/lifx-lan
gcc -std=gnu99 examples/get-power.c -o bin/get-power
gcc -std=gnu99 examples/get-light-state.c -o bin/get-light-state
gcc -std=gnu99 -pthread examples/daemon.c -o bin/daemon
