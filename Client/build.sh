cd rtmidi; cmake .; make; true; cd .. #will fail but thats fine
qmake
make
LD_LIBRARY_PATH=rtmidi
./Client
