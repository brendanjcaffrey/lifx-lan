mkdir -p bin
gcc -std=c99 examples/lifx-lan.c -o bin/lifx-lan
gcc -std=c99 examples/get-power.c -o bin/get-power
gcc -std=c99 examples/get-light-state.c -o bin/get-light-state
gcc -std=c99 examples/set-color-redundant.c -o bin/set-color-redundant
gcc -std=c99 examples/set-power-redundant.c -o bin/set-power-redundant
gcc -std=c99 examples/daemon.c -o bin/daemon

xattr -csr bin/*
