cd ../rtmidi; cmake .; make; true; cd ../Client #will fail but thats fine
qmake
make
LD_LIBRARY_PATH=../rtmidi
./Client
