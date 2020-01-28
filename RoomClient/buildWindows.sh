MXE_PREFIX=$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.static/
PATH=$PATH:$HOME/.local/share/mxe/usr/bin/:$MXE_PREFIX/bin/
$MXE_PREFIX/qt5/bin/qmake
make
wine release/Client.exe
