mkdir -p bin
gcc -std=c99 examples/lifx-lan.c -o bin/lifx-lan
gcc -std=c99 examples/get-power.c -o bin/get-power
gcc -std=c99 examples/get-light-state.c -o bin/get-light-state
