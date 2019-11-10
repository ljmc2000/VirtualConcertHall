unset CFLAGS CXXFLAGS
PATH=$PATH:$HOME/.local/share/mxe/usr/bin/:$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.static/bin/
mkdir rtmidiWindows rtmidiLinux

cd rtmidiWindows
~/.local/share/mxe/usr/bin/x86_64-w64-mingw32.static-cmake ../rtmidi
make
cd ..

cd rtmidiLinux
cmake ../rtmidi
make
cd ..
